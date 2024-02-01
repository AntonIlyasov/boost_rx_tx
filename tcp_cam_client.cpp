#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp>   // Запись видео

#define CAM_ADR "/dev/video0"
#define IP "192.168.151.68"
#define PORT 1234

using namespace boost::asio;
using ip::tcp;
using boost::asio::ip::address;
using std::string;
using std::cout;
using std::endl;

class TCPClient{
private:
    boost::asio::io_context& io_context_;
    tcp::socket socket_;
public:

    TCPClient(boost::asio::io_context& io_context)
        : io_context_(io_context), socket_(io_context){
            socket_.connect(ip::tcp::endpoint(boost::asio::ip::address::from_string(IP), PORT));
    }

    bool frameReadedByServer(){
        boost::asio::streambuf buf;
        boost::asio::read_until( socket_, buf, "\n" );
        string data = boost::asio::buffer_cast<const char*>(buf.data());
        if (data == "frame readed\n") {
            return true;
        } else {
            return false;
        }
    }

    void send_video(){
        std::vector<uchar> imgBufferFromImencode, msgSendBuf;
        cv::VideoCapture cap(CAM_ADR);
        if (cap.isOpened()){
            std::cout << "cap.isOpened()" << std::endl;
        } else {
            std::cout << "cap.isNOTOpened()" << std::endl;     
        }
        sleep(2);
        bool firstSendFrame = true;

        while (1) {

            if (firstSendFrame || frameReadedByServer()){
                cv::Mat frameFromWeb;
                boost::system::error_code error;
                cap.read(frameFromWeb);
                std::vector<uchar> imgSizeBuf(8);
                size_t frameSizeFromBuf = 0;

                if (!frameFromWeb.empty()){
                    
                    cv::imencode(".jpg", frameFromWeb, imgBufferFromImencode, std::vector<int> {cv::IMWRITE_JPEG_QUALITY, 100});  
                    
                    const std::string frameSizeStringFromImencode = std::to_string(imgBufferFromImencode.size());

                    for(int i = 0; i < imgSizeBuf.size(); i++){
                        if(i < frameSizeStringFromImencode.size()){
                            imgSizeBuf[i] = frameSizeStringFromImencode[i];
                        } else {
                            imgSizeBuf[i] = '\n';
                        }
                    }

                    msgSendBuf.resize(imgSizeBuf.size() + imgBufferFromImencode.size());
                    std::move(imgSizeBuf.begin(), imgSizeBuf.end(), msgSendBuf.begin());
                    std::move(imgBufferFromImencode.begin(), imgBufferFromImencode.end(), msgSendBuf.begin() + imgSizeBuf.size());        

                    size_t write_bytes = socket_.write_some(boost::asio::buffer(msgSendBuf), error);
                    
                    if( !error ) {
                        std::cout << "write_bytes = " << write_bytes << std::endl;
                        std::cout << "imgSizeBuf.size() = " << imgSizeBuf.size() << std::endl;
                        std::cout << "frameSizeFromBuf = " << frameSizeFromBuf << std::endl;
                        std::cout << "imgBufferFromImencode.size() = " << imgBufferFromImencode.size() << std::endl;
                        std::cout << "Client send frameFromWeb to Server!" << std::endl << std::endl;
                    }
                    else {
                        std::cout << "frameFromWeb send failed: " << error.message() << std::endl;
                    }
                }
            }
            firstSendFrame = false;
        }
    }
    ~TCPClient(){
		socket_.close();
	}
};

int main() {
    boost::asio::io_context io_context;
    TCPClient tcpClient(io_context);
    tcpClient.send_video();
}