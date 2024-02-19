#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <stdio.h>
#include <string.h>
#include "umba_crc_table.h"
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::address;
using boost::asio::ip::tcp;

#define PORT 1234

uint32_t keepalive = 0;

uint32_t make_uint32(uint8_t* buf) {
  return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}

class Session
{
public:
  Session(boost::asio::io_context& io_context)
  : socket_(io_context) {
    recvd_count = 0;
    send_count  = 0;
  }

  tcp::socket& socket()
  {
    return socket_;
  }

  void async_read()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&Session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void async_write(size_t bytes_transferred)
  {
    boost::asio::async_write(socket_,
        boost::asio::buffer(data_, bytes_transferred),
        boost::bind(&Session::handle_write, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

private:
  uint32_t recvd_count;
  uint32_t send_count;

  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      std::cout << "NEW MESSAGE RECEIVED!!!!!!)))))" << std::endl;
      recvd_count++;
      std::cout << "bytes_transferred = " << bytes_transferred << std::endl;
      for (int i = 0; i < bytes_transferred; i++){
        printf("[%u]", data_[i]);
      }
      std::cout << "\nKEEPALIVE = " << make_uint32(data_ + 4) << std::endl;
      std::cout << "\n\033[1;32mRECEIVED \033[0m= " << recvd_count << std::endl;
      async_write(bytes_transferred);
    }
    else
    {
      delete this;
    }
  }

  void handle_write(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error && bytes_transferred > 0)
    {
      std::cout << "\nSEND TO TCP: ";
      for (int i = 0; i < bytes_transferred; i++){
          printf("[%u]", data_[i]);
      }
      std::cout << std::endl;
      send_count++;
      std::cout << "\033[1;32msend_count\033[0m = " << send_count << "\n";
      async_read();
    }
    else
    {
      delete this;
    }
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  uint8_t data_[max_length];
};

class TCPServer{
public:
  TCPServer(boost::asio::io_context& io_context)
  : io_context(io_context),
    acceptor(io_context, tcp::endpoint(tcp::v4(), PORT))
  {
    std::cout << "TCP SERVER IS RUNNING\n";
    async_accept();
  }
private:
  boost::asio::io_context& io_context;
  tcp::acceptor acceptor;

  void async_accept()
  {
    Session* new_Session = new Session(io_context);
    acceptor.async_accept(new_Session->socket(),
        boost::bind(&TCPServer::handle_accept, this, new_Session,
        boost::asio::placeholders::error));
  }

  void handle_accept(Session* new_Session,
      const boost::system::error_code& error){
    if (!error){
      new_Session->async_read();
    }
    else {
      delete new_Session;
    }
    async_accept();
  }
};



int main(int argc, char* argv[])
{
  try{
    boost::asio::io_context io_context;
    TCPServer tcpServer(io_context);
    io_context.run();
  } catch (std::exception e){
    std::cerr << "Exeption: " << e.what() << std::endl;
  }
  return 0;
}