#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

int main() {
  std::cout << "start send_img_tcp\n";

  // <-----------------------connect to tcp---------------------------->
  io_service io_service;
  ip::tcp::socket socket(io_service);
  socket.connect(ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));

  // <------------------------open img file---------------------------->
  cv::Mat img = cv::imread("1920_1280.bmp");;
  boost::system::error_code error;
  std::vector<uchar> img_imencode, img_to_send;
  // std::vector<uchar> img_buf_size(8);

  // <--------------------imencode & send to TCP------------------------>
  cv::imencode(".bmp", img, img_to_send, std::vector<int> {cv::IMWRITE_WEBP_QUALITY, 101});
  // img_to_send.resize(img_buf_size.size() + img_imencode.size());
  // std::move(img_buf_size.begin(), img_buf_size.end(), img_to_send.begin());
  // std::move(img_imencode.begin(), img_imencode.end(), img_to_send.begin() + img_buf_size.size());

  size_t write_bytes = socket.write_some(boost::asio::buffer(img_to_send), error);

  if( !error ) {
    std::cout << "write_bytes = " << write_bytes << std::endl;
    // std::cout << "img_buf_size.size() = " << img_buf_size.size() << std::endl;
    std::cout << "img_imencode.size() = " << img_imencode.size() << std::endl;
    std::cout << "Client send img to Server!" << std::endl << std::endl;
  }
  else {
    std::cout << "img send failed: " << error.message() << std::endl;
  }
  
  return 0;
}