#include <boost/asio.hpp>
#include <iostream>

#define BUFSIZE 256

int main(int argc, char *argv[]) {

    if(argc < 3){
        std::cout << "[./serial_pub_simple][/dev/ttyX][baudrate]\n";
        return -1;
    }

    std::string port = argv[1];
    std::string baudrate = argv[2];

    boost::asio::io_service io;
    // Open serial port
    boost::asio::serial_port serial(io, "/dev/tty" + port);

    // Configure basic serial port parameters
    termios t;
    int m_fd;
    m_fd = serial.native_handle();
    if (tcgetattr(m_fd, &t) < 0) { /* handle error */ }
    if (cfsetspeed(&t, std::stoi(baudrate)) < 0) { /* handle error */ }
    if (tcsetattr(m_fd, TCSANOW, &t) < 0) { /* handle error */ }
    serial.set_option(boost::asio::serial_port_base::baud_rate(std::stoi(baudrate)));
    serial.set_option(boost::asio::serial_port_base::character_size(8 /* data bits */));
    serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    
    std::cout << "\nserial_pub_simple is running.\n";
    try {
        while(true) {
            uint8_t dataS[] = {0x01, 0x06, 0x20, 0x01, 0x02, 0xA6};
            size_t n = serial.write_some(boost::asio::buffer(dataS));
            static uint32_t count = 0;
            if(n > 0){
                count++;
                printf("\nsend new pack\n");
                printf("count = %u\n", count);
                for(int i = 0; i < n; i++){
                    printf("[%u]", dataS[i]);
                }
                std::cout << std::endl;
                std::cout << "n = " << n << std::endl;
            }
            uint8_t dataR[BUFSIZE] = {0};
            size_t r = serial.read_some(boost::asio::buffer(dataR));
            // Write data to stdout
            if(r > 0){
                count++;
                printf("\nresvd new pack\n");
                printf("count = %u\n", count);
                for(int i = 0; i < r; i++){
                    printf("[%u]", dataR[i]);
                }
                std::cout << std::endl;
                printf("r = %u\n", r);
            }
            std::this_thread::sleep_for (std::chrono::milliseconds(100));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

}
