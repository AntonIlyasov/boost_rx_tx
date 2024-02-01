#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <stdio.h>
#include <string.h>

using boost::asio::ip::udp;
using boost::asio::ip::address;

#define PORT 1234

class UDPServer{
public:
  UDPServer(boost::asio::io_service& io_service)
  : socket_(io_service, udp::endpoint(udp::v4(), PORT)){
    std::cout << "UDP SERVER IS RUNNING\n";
    boost::bind(&UDPServer::udp_handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
    read_msg_udp();
  }

  void read_msg_udp(){
  socket_.async_receive_from(boost::asio::buffer(data_, sizeof(data_)), sender_endpoint_,
      boost::bind(&UDPServer::udp_handle_receive, this, boost::asio::placeholders::error, 
      boost::asio::placeholders::bytes_transferred));
  }


  void do_smth(){

  }

private:
  udp::socket socket_;
  udp::endpoint sender_endpoint_;
  enum {max_length = 60};
  uint8_t data_[max_length] = {0};
  uint32_t recvd_count = 0;

  void udp_handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) {
      std::cout << "Receive failed: " << error.message() << "\n";
      return;
    } else {
      std::cout << "bytes_transferred = " << bytes_transferred << std::endl;
      for (int i = 0; i < bytes_transferred; i++){
        printf("[%u]", data_[i]);
      }
      read_msg_udp();
    }
  }



};

int main(int argc, char* argv[]){
  try{
    // 1) instantiate io_searvice
    // 2) make a customerized server
    // 3) start io_service
    
    boost::asio::io_service io_service;
    UDPServer udpServer(io_service);
    while(1){
      udpServer.do_smth();
      io_service.poll_one();
    }
  } catch (std::exception e){
    std::cerr << "Exeption: " << e.what() << std::endl;
  }
  return 0;
}