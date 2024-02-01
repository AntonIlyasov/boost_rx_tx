#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#define ASIO_STANDALONE
#include <boost/asio.hpp>

int main(int argc, char* argv[])
{
    // Check the number of parameters
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " IP" << " FilePath" << std::endl;
        return 1;
    }

    std::filesystem::path filePath{argv[2]};
    if (!std::filesystem::exists(filePath))
    {
        std::cerr << "File don't exist!" << std::endl;
        return 1;
    }

    boost::system::error_code ec;
    boost::asio::io_context context;
    boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(argv[1]), 1234);
    
    boost::asio::ip::tcp::socket socket(context);

    socket.connect(endpoint, ec);

    if (!ec)
    {
        std::cout << "Connected!\n";
    }
    else
    {
        std::cerr << "Connection Failed:\n" << ec.message();
        return 2;
    }

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
        std::ifstream input(argv[2], std::ios::binary);
        std::string buffer(std::istreambuf_iterator<char>(input), {});
        std::cout << "Sending file to server...\n";
        socket.send(boost::asio::buffer(buffer.data(), buffer.size()));

        input.close();
        std::cout << "File sent.\n";
    }

    return 0;
}