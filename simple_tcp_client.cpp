#include <iostream>
#include <boost/asio.hpp>
#include "umba_crc_table.h"

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

#define MY_CLIENT_PORT 1234
#define MY_CLIENT_IP "127.0.0.1"

const uint8_t DATA_LEN = 9;
uint32_t keepalive = 0;
uint8_t kpl_bytes[4] = {0};
bool array = false;

uint32_t inline make_uint32(uint8_t* buf) {
    return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}

inline void int_to_bytes(uint32_t &val, uint8_t *bytes) {
  // printf("OUTPUT = %d\n", val);
  bytes[0] = val & 0xFF;                /*0-7 bits*/
  bytes[1] = (val & 0xFF00) >> 8;       /*8-15 bits*/
  bytes[2] = (val & 0xFF0000) >> 16;    /*16-23 bits*/
  bytes[3] = (val & 0xFF000000) >> 24;  /*24-31 bits*/
  // printf("bytes\n[%u][%u][%u][%u]\n", bytes[0],bytes[1],bytes[2],bytes[3]);
}

void sendMsg(uint8_t num, ip::tcp::socket& socket) {
  auto start = std::chrono::high_resolution_clock::now();
  int_to_bytes(keepalive, kpl_bytes);
  uint8_t msg_raw[] = {0xAA, 0xBB, DATA_LEN, num, kpl_bytes[3], kpl_bytes[2], kpl_bytes[1], kpl_bytes[0]};
  uint8_t msg[] = {0xAA, 0xBB, DATA_LEN, num, kpl_bytes[3], kpl_bytes[2], kpl_bytes[1], kpl_bytes[0], umba_crc8_table(msg_raw, DATA_LEN - 1)};

  std::cout << "\nInput...\n";
  
  static uint32_t send_count = 0;
  boost::system::error_code err;
  
  // size_t write_bytes = socket.write_some(boost::asio::buffer(msg), err);

  auto sent = socket.send(boost::asio::buffer(msg), 0, err);

  std::cout << "sent = " << sent << "\n";
  if (!err && sent > 0){
      std::cout << "\nSEND TO TCP: ";
      for (int i = 0; i < sent; i++){
          printf("[%u]", msg[i]);
      }
      std::cout << std::endl;
      send_count++;
      std::cout << "\033[1;32msend_count\033[0m = " << send_count << "\n";
      // std::this_thread::sleep_for(std::chrono::microseconds(1000000));
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> duration = end - start;
  float mls = duration.count() * 1000;
  std::cout << "mls: " << mls << std::endl;
}

void recvdMsg(ip::tcp::socket& socket){
  socket.wait(socket.wait_read);
  std::size_t bytes = socket.available();
  if (bytes > 0) {
    uint8_t response[100];
    boost::system::error_code ec;
    auto recvd = socket.read_some(boost::asio::buffer(response), ec);

    std::cout << "recvd = " << recvd << "\n";
    if (!ec && recvd > 0)
    {
      std::cout << "\nRECVD FROM TCP: ";
      for (int i = 0; i < recvd; i++){
          printf("[%u]", response[i]);
      }
      std::cout << std::endl;
    }
  }
}

int main(int argc, char* argv[]) {
  std::cout << "start simple_tcp_client\n";
  if (argc < 2){
    std::cerr << "Usage: " << argv[0] << " cmd" << std::endl;
    return 1;
  }

  // <-----------------------connect to tcp---------------------------->
  boost::system::error_code ec;
  boost::asio::io_context context;
  boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(MY_CLIENT_IP), MY_CLIENT_PORT);
  boost::asio::ip::tcp::socket socket(context);
  socket.connect(endpoint, ec);
  if (socket.is_open()){
    while(1){
      // <-------------------------send msg to TCP----------------------------->
      uint8_t cmd  = (uint8_t)std::stoi(argv[1]);
      sendMsg(cmd, socket);
      recvdMsg(socket);
      std::this_thread::sleep_for(std::chrono::microseconds(1000));
    }
  }
  return 0;
}