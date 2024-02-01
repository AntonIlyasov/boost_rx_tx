#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp> 
#include <string>
#include <vector>

using boost::asio::ip::tcp;
using namespace std;
using namespace cv;
 
// source https://www.programmerall.com/article/7721373696/
// source https://cppsecrets.com/users/14041151035752494957504952535764103109971051084699111109/Programming-in-C00-using-boostasio.php
bool flag = false;    

void servershow()
{
    while (true)
    {
        if (flag)
        {
            //imshow("server",img);
            waitKey(1);
        }   
    }
}

cv::Mat retrieve_data(){

    std::string image_path = "/YOUR_IMAGE.jpg";
    cv::Mat image;
    image = imread(image_path, cv::IMREAD_COLOR);
    if(! image.data ) {
        std::cout << "Could not open or find the image" << std::endl;
    }
    return image;
}


int main()
{
    boost::thread thrd(&servershow);
    try
    {
        boost::asio::io_service io_service;
        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 3200));
        
        for (;;) {
            tcp::socket socket(io_service);
            acceptor.accept(socket);
            boost::system::error_code ignored_error;

            //retrieve the frame to be sent
            cv::Mat frame = retrieve_data();
            std::vector<std::uint8_t> buff;
            cv::imencode(".jpg", frame, buff, param);

            // now we send the header message
            std:: string image_dimensions = "6016Wx3384H";
            std:: string image_buff_bytes = std::to_string(buff.size());
            std::string message_header = image_dimensions + "," +  image_buff_bytes;
            std::cout << "sending measage header of " << std::to_string(message_header.length()) << " bytes...." << std::endl;
            message_header.append(63 - message_header.length(), ' ');
            message_header = message_header + '\0';

            socket.write_some(boost::asio::buffer(message_header), ignored_error);

            socket.write_some(boost::asio::buffer(buff), ignored_error);

            flag = true;
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    thrd.join();

    return 0; 
}