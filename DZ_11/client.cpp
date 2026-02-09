#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    try {
        std::string host = "127.0.0.1";
        std::string port = "8080";
        std::string name = "Alex";

        if (argc >= 2) host = argv[1];
        if (argc >= 3) port = argv[2];
        if (argc >= 4) name = argv[3];

        boost::asio::io_context io;

        tcp::resolver resolver(io);
        tcp::socket socket(io);

        boost::asio::connect(socket, resolver.resolve(host, port));

        std::string message = "Hello, Server, I'm " + name + "\n";
        boost::asio::write(socket, boost::asio::buffer(message));

        boost::asio::streambuf buffer;
        boost::asio::read_until(socket, buffer, '\n');

        std::istream input(&buffer);
        std::string response;
        std::getline(input, response);

        std::cout << "Server response: " << response << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << "\n";
        return 1;
    }
}
