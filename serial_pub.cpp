#include <boost/asio.hpp>
#include <iostream>
#include <chrono>
#define BOUDRATE 115200
#define DEVICE "/dev/ttyUSB0"

class ITransport
{
public:
    ITransport() {}
    virtual ~ITransport() {}
    virtual bool sendData() = 0;
};

class Server{
public:
    Server(){}

    void SendData(ITransport* transport_ptr){
        transport_ptr->sendData();
    }

private:
    ITransport* it_transport;
};


class Bus: public ITransport{
public:
    Bus(boost::asio::io_service& io_service_)
    : m_port(io_service_){
        boost::system::error_code error;
        m_port.open(DEVICE, error);
        if (!error)
        {
            // Configure basic serial port parameters: 115.2kBaud, 8N1
            m_port.set_option(boost::asio::serial_port_base::baud_rate(BOUDRATE));
            m_port.set_option(boost::asio::serial_port_base::character_size(8 /* data bits */));
            m_port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
            m_port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        }
    }

    virtual bool sendData() override {
        size_t count = 0;
        while(true) {
            char data[] = "Bus!!! Bus!!!";
            size_t n = m_port.write_some(boost::asio::buffer(data));
            // Write data to stdout
            std::cout << "count: " << count << std::endl;
            std::cout << "data: " << data << std::endl;
            std::this_thread::sleep_for (std::chrono::milliseconds(200));
            count++;
        }
        return true;
    }

private:
    boost::asio::io_service io_service;
    boost::asio::serial_port m_port;
};

class Plane: public ITransport{
public:
    Plane(boost::asio::io_service& io_service_)
    : m_port(io_service_){
        boost::system::error_code error;
        m_port.open(DEVICE, error);
        if (!error)
        {
            // Configure basic serial port parameters: 115.2kBaud, 8N1
            m_port.set_option(boost::asio::serial_port_base::baud_rate(BOUDRATE));
            m_port.set_option(boost::asio::serial_port_base::character_size(8 /* data bits */));
            m_port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
            m_port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        }
    }

    virtual bool sendData() override {
        size_t count = 0;
        while(true) {
            char data[] = "Plane!!! Plane!!!";
            size_t n = m_port.write_some(boost::asio::buffer(data));
            // Write data to stdout
            std::cout << "count: " << count << std::endl;
            std::cout << "data: " << data << std::endl;
            std::this_thread::sleep_for (std::chrono::milliseconds(200));
            count++;
        }
        return true;
    }

private:
    boost::asio::io_service io_service;
    boost::asio::serial_port m_port;
};


int main() {
    boost::asio::io_service io;
    Bus bus(io);
    Plane plane(io);
    Server myServ;
    myServ.SendData(&plane);

}