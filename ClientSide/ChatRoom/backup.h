#pragma once
// just a draft file. **it won't compile
// will be deleted at the production time
#if 0

// https://www.boost.org/doc/libs/1_87_0/doc/html/boost_asio/tutorial/tutdaytime2.html


#include <boost/asio.hpp>
#include <iostream>
#include <thread>

using boost::asio::ip::tcp;

void run_client(const std::string& server_ip, unsigned short port) {
    try {
        boost::asio::io_context io_context;

        // Connect to the server
        tcp::socket socket(io_context);
        socket.connect(tcp::endpoint(boost::asio::ip::make_address(server_ip), port));

        std::cout << "Connected to the server.\n";

        // Start a thread to read messages from the server
        std::thread reader_thread([&socket]() {
            try {
                char buffer[1024];
                while (true) {
                    boost::system::error_code ec;
                    size_t length = socket.read_some(boost::asio::buffer(buffer), ec);

                    if (ec == boost::asio::error::eof) {
                        std::cout << "Connection closed by server.\n";
                        break;
                    }
                    else if (ec) {
                        std::cerr << "Error reading from server: " << ec.message() << "\n";
                        break;
                    }

                    std::cout << "Received: " << std::string(buffer, length) << "\n";
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Reader thread error: " << e.what() << std::endl;
            }
            });

        // Main thread sends messages
        while (true) {
            std::string message;
            std::cout << "Enter message: ";
            std::getline(std::cin, message);

            if (message == "quit") {
                break;
            }

            boost::asio::write(socket, boost::asio::buffer(message));
        }

        // Wait for the reader thread to finish
        reader_thread.join();

    }
    catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
    }
}

int main() {
    run_client("127.0.0.1", 12345); // Connect to server on localhost:12345
    return 0;
}



std::this_thread::sleep_for(std::chrono::microseconds(100));
std::jthread t_write([&]() {
    write_message(std::ref(socket));
    });

#endif