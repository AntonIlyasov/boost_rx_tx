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
  std::cout << "start recv_img_tcp\n";

  // <-----------------------connect to tcp---------------------------->
  boost::system::error_code error;
  io_service io_service;
  tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));
  auto socket = acceptor.accept();

  std::vector<uint8_t> msgRecBuf;
  size_t length = socket.read_some(boost::asio::buffer(msgRecBuf), error);
  if (error) throw boost::system::system_error(error);

  // std::move(msgRecBuf.begin(), msgRecBuf.begin() + imgSizeBuf.size(), imgSizeBuf.begin());
  std::cout << "here1\n";
  cv::Mat img = cv::imdecode(msgRecBuf, cv::IMREAD_ANYCOLOR);
  std::cout << "here2\n";
  imshow("img", img);
  cv::waitKey(0);

  return 0;
}