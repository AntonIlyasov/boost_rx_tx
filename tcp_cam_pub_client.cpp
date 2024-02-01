#include <iostream>
#include <boost/asio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/videoio.hpp>   // Запись видео

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

bool firstSendFrame = true;

bool frameReadedByServer(tcp::socket& socket){
    boost::asio::streambuf buf;
    boost::asio::read_until( socket, buf, "\n" );
    string data = boost::asio::buffer_cast<const char*>(buf.data());
    if (data == "frame readed\n") {
        return true;
    } else {
        return false;
    }
}

int main() {
    std::cout << "start client\n";

    std::vector<uchar> imgBufferFromImencode, msgSendBuf;
    cv::VideoCapture cap("/dev/video0");
    if (cap.isOpened()){
        std::cout << "cap.isOpened()" << std::endl;
    } else {
        std::cout << "cap.isNOTOpened()" << std::endl;     
        return -1;
    }
    sleep(2);
    io_service io_service;
    ip::tcp::socket socket(io_service);

    // no error handling, if server is not listening - this call throws
    socket.connect(ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));

    while (1) {

        if (firstSendFrame || frameReadedByServer(socket)){
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

                std::string frameSizeFromBufStr;
                frameSizeFromBufStr.resize(8);
                for(int i = 0; i < imgSizeBuf.size(); i++){
                    frameSizeFromBufStr[i] = imgSizeBuf[i];
                }

                frameSizeFromBuf = std::stoi(frameSizeFromBufStr);

                msgSendBuf.resize(imgSizeBuf.size() + imgBufferFromImencode.size());
                std::move(imgSizeBuf.begin(), imgSizeBuf.end(), msgSendBuf.begin());
                std::move(imgBufferFromImencode.begin(), imgBufferFromImencode.end(), msgSendBuf.begin() + imgSizeBuf.size());        

                size_t write_bytes = socket.write_some(boost::asio::buffer(msgSendBuf), error);
                
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
    return 0;
}