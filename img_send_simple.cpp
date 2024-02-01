#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#define ASIO_STANDALONE
#include <boost/asio.hpp>

int main(int argc, char* argv[])
{
    std::filesystem::path filePath{"/home/anton202/boost_rx_tx/build/1920_1280.bmp"};
    assert(std::filesystem::exists(filePath));

    boost::system::error_code ec;
    boost::asio::io_context context;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address("127.0.0.1"), 1234);
    boost::asio::ip::tcp::socket socket(context);
    socket.connect(endpoint, ec);

    if (socket.is_open()) {
        // Send file name
        socket.send(boost::asio::buffer(filePath.filename().string().data(), filePath.filename().string().size()));
        
        // Wait for response from server
        socket.wait(socket.wait_read);
        std::size_t bytes = socket.available();
        if (bytes > 0) {
            std::string response;
            response.resize(bytes);
            socket.read_some(boost::asio::buffer(response.data(), bytes), ec);

            if (response != "OK")
            {
                std::cerr << "Unexpected server Error!\n";
                return 2;
            }
        }

        // Send File
        std::ifstream input(filePath.filename().string().data(), std::ios::binary);
        std::string buffer(std::istreambuf_iterator<char>(input), {});
        std::cout << "Sending file to server...\n";
        socket.send(boost::asio::buffer(buffer.data(), buffer.size()));

        input.close();
        std::cout << "File sent.\n";
    }

    return 0;
}