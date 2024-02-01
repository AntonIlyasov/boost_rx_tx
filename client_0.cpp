#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <string>
#include <vector>


using boost::asio::ip::tcp;

using namespace std;
using namespace cv;

// source https://www.programmerall.com/article/7721373696/
// source https://cppsecrets.com/users/14041151035752494957504952535764103109971051084699111109/Programming-in-C00-using-boostasio.php

struct image_metadata_t {
    int width;
    int height;
    size_t image_size_bytes;
};

image_metadata_t parse_header(boost::asio::streambuf &buffer){

    std::string data_buff_str = std::string(boost::asio::buffer_cast<const char*>(buffer.data()));
    cout << data_buff_str << endl;

    int width_pos = data_buff_str.find("W"); 
    int x_pos = data_buff_str.find("x"); 
    int height_pos = data_buff_str.find("H"); 
    int comma_pos = data_buff_str.find(","); 

    std::cout << "data_buff_str.substr(x_pos+1,height_pos) " << data_buff_str.substr(x_pos+1,height_pos) <<std::endl;

    image_metadata_t meta_data;
    meta_data.width = std::stoi(data_buff_str.substr(0,width_pos));
    meta_data.height = std::stoi(data_buff_str.substr(x_pos+1,height_pos));
    meta_data.image_size_bytes = std::stoi(data_buff_str.substr(data_buff_str.find(",") + 1));

    return meta_data;
}


cv::Mat GetImageFromMemory(uchar* image, int length, int flag) {
    std::vector<uchar> data = std::vector<uchar>(image, image + length);
    cv::Mat ImMat = imdecode(data, flag);

    return ImMat;
}

int main()
{
    try{
        boost::asio::io_service io_service;
        tcp::endpoint end_point(boost::asio::ip::address::from_string("127.0.0.1"), 3200);
        tcp::socket socket(io_service);
        socket.connect(end_point);
        boost::system::error_code ignored_error;
        boost::asio::streambuf receive_buffer;

        // Now we retrieve the message header of 64 bytes
        size_t header_size = 64;
        boost::asio::read(socket, receive_buffer, boost::asio::transfer_exactly(header_size), ignored_error);

        if ( ignored_error && ignored_error != boost::asio::error::eof ) {
            cout << "first receive failed: " << ignored_error.message() << endl;
        } else {
            image_metadata_t header_data = parse_header(receive_buffer);

            const int IMAGE_WIDTH = header_data.width;
            const int IMAGE_HEIGHT = header_data.height;

            // Now we retrieve the frame itself
            std::cout << "Now asing for image bytes of size " << std::to_string(header_data.image_size_bytes) << std::endl;
            //boost::asio::streambuf second_receive_buffer;
            std::vector<std::uint8_t> buff(header_data.image_size_bytes);
            boost::asio::read(socket, boost::asio::buffer(buff), boost::asio::transfer_exactly(header_data.image_size_bytes), ignored_error);
            if( ignored_error && ignored_error != boost::asio::error::eof ) {
                cout << "SECOND receive failed: " << ignored_error.message() << endl;
            }
            else {
                std::cout << "[IMAGE_WIDTH]  = " << std::to_string(IMAGE_WIDTH) << std::endl;
                std::cout << "[IMAGE_HEIGHT] = " << std::to_string(IMAGE_HEIGHT) << std::endl;
                std::cout << "image bytes  = " << std::to_string(header_data.image_size_bytes) << std::endl;

                cv::Mat img(cv::imdecode(buff, cv::IMREAD_ANYCOLOR));

                imshow("client", img);
                waitKey(5000);
            }
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
 
    return 0;
}