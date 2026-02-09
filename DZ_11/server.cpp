#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {
    try {
        unsigned short port = 8080;
        if (argc >= 2) {
            port = static_cast<unsigned short>(std::stoi(argv[1]));
        }

        boost::asio::io_context io;

        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), port));
        std::cout << "Server started on port " << port << "\n";

        while (true) {
            tcp::socket socket(io);
            acceptor.accept(socket);

            boost::asio::streambuf buffer;
            boost::asio::read_until(socket, buffer, '\n');

            std::istream input(&buffer);
            std::string request;
            std::getline(input, request);

            std::cout << "Received: " << request << "\n";

            std::string name = "Unknown";
            const std::string prefix = "Hello, Server, I'm ";

            if (request.rfind(prefix, 0) == 0) {
                name = request.substr(prefix.size());
            }

            std::string response = "Hello, " + name + "\n";
            boost::asio::write(socket, boost::asio::buffer(response));
        }
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << "\n";
        return 1;
    }
}
