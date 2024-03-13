#include <iostream>
#include <optional>
#include <boost/asio.hpp>
#include <vector>
#include <fstream>
#include <filesystem>

#define PORT  1111

using namespace boost::asio;

const std::string savingDirectory{"./received/"};

std::vector<char> vBuffer(20 * 1024); // 20 Kilobytes

class session : public std::enable_shared_from_this<session>
{
public:

    session(boost::asio::ip::tcp::socket&& socket)
    : socket(std::move(socket))
    {
        this->clientIP = this->socket.remote_endpoint().address().to_string();
        std::cout << this->clientIP << ": Connected!\n";
    }

    // Get incoming file's name and return OK if server is ready
    void get_name()
    {
        socket.async_read_some(boost::asio::buffer(vBuffer.data(), vBuffer.size()), [self = shared_from_this()](std::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    std::string name(vBuffer.begin(), vBuffer.begin()+length);
                    std::cout << self->clientIP << ": Downloading " << savingDirectory+name << std::endl;
                    self->outfile.open(savingDirectory+name, std::ios::out | std::ios::binary);
                    const std::string response{"OK"};
                    self->socket.async_send(boost::asio::buffer(response.data(), response.size()), [](std::error_code ec, std::size_t length){});
                    self->get_file();
                }
            } 
        );

    }

    // Recieve file
    void get_file()
    {
        socket.async_read_some(boost::asio::buffer(vBuffer.data(), vBuffer.size()), [self = shared_from_this()](std::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    self->outfile.write(vBuffer.data(), length);
                    self->get_file();
                }
            } 
        );
    }

    ~session() {
        outfile.close();
        std::cout << this->clientIP << ": Disconnected!\n";
    }

private:
    std::ofstream outfile;
    boost::asio::ip::tcp::socket socket;
    boost::asio::streambuf streambuf;
    std::string clientIP;
};

class server
{
public:

    server(boost::asio::io_context& io_context, std::uint16_t port)
    : io_context(io_context)
    , acceptor  (io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
    }

    void async_accept()
    {
        socket.emplace(io_context);

        acceptor.async_accept(*socket, [&] (boost::system::error_code error)
        {
            std::make_shared<session>(std::move(*socket))->get_name();
            async_accept();
        });
    }

private:

    boost::asio::io_context& io_context;
    boost::asio::ip::tcp::acceptor acceptor;
    std::optional<boost::asio::ip::tcp::socket> socket;
};

int main()
{
    std::filesystem::path path(savingDirectory);
    if (!std::filesystem::is_directory(path))
    {
        std::cerr << savingDirectory << " directory doesn't exist. Creating one!\n";
        std::filesystem::create_directories(path);
    }

    boost::asio::io_context io_context;
    server srv(io_context, PORT);
    srv.async_accept();
    io_context.run();

    return 0;
}
