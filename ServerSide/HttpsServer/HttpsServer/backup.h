#pragma once

// This is a draft for debugging purposes - it won't compile
#if 0


#include "TCP_Connection.h"
#include <boost/asio.hpp>
#include <memory>
#include <functional>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using io_context = asio::io_context;
using error_code = boost::system::error_code;

class TCP_Server {
public:
    TCP_Server(io_context& io_context, unsigned short port) :
        io_context_(io_context),
        acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
    }

    void start_accept() {
        auto new_connection = TCP_Connection::create(io_context_);
        acceptor_.async_accept(new_connection->socket(),
            [this, new_connection](const error_code& ec) {
                if (!ec) {
                    std::cout << "New connection accepted." << std::endl;
                    handle_new_connection(new_connection);
                }
                start_accept(); // Accept the next client
            });
    }

    void handle_new_connection(std::shared_ptr<TCP_Connection> new_connection) {
        if (client_1_ == nullptr) {
            client_1_ = new_connection;
            std::cout << "Client 1 connected." << std::endl;
        }
        else if (client_2_ == nullptr) {
            client_2_ = new_connection;
            std::cout << "Client 2 connected." << std::endl;

            // Pair the two clients together
            client_1_->set_partner(client_2_);
            client_2_->set_partner(client_1_);

            // Start communication between the two clients
            client_1_->start();
            client_2_->start();
        }
        else {
            std::cerr << "Max clients reached. Rejecting new connection." << std::endl;
            new_connection->send_message("Server is full. Try again later.");
        }
    }

private:
    io_context& io_context_;
    tcp::acceptor acceptor_;
    std::shared_ptr<TCP_Connection> client_1_;
    std::shared_ptr<TCP_Connection> client_2_;
};





#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <array>

using boost::asio::ip::tcp;

class RelayServer {
public:
    RelayServer(boost::asio::io_context& io_context, unsigned short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        accept_clients();
    }

private:
    tcp::acceptor acceptor_;
    std::array<tcp::socket, 2> clients_;
    int connected_clients_ = 0;

    void accept_clients() {
        if (connected_clients_ < 2) {
            acceptor_.async_accept(
                [this](boost::system::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::cout << "Client connected.\n";
                        clients_[connected_clients_] = std::move(socket);
                        connected_clients_++;

                        // Accept the next client if less than two are connected
                        accept_clients();

                        // Start communication if two clients are connected
                        if (connected_clients_ == 2) {
                            start_relay();
                        }
                    }
                    else {
                        std::cerr << "Error accepting client: " << ec.message() << "\n";
                    }
                });
        }
    }

    void start_relay() {
        read_from_client(0);
        read_from_client(1);
    }

    void read_from_client(int client_index) {
        auto& client = clients_[client_index];
        auto buffer = std::make_shared<std::array<char, 1024>>();

        client.async_read_some(boost::asio::buffer(*buffer),
            [this, client_index, buffer](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::cout << "Received from client " << client_index << ": "
                        << std::string(buffer->data(), length) << "\n";

                    // Relay the message to the other client
                    int other_client = 1 - client_index;
                    boost::asio::async_write(clients_[other_client],
                        boost::asio::buffer(buffer->data(), length),
                        [this](boost::system::error_code ec, std::size_t /*length*/) {
                            if (ec) {
                                std::cerr << "Error relaying message: " << ec.message() << "\n";
                            }
                        });

                    // Continue reading from the current client
                    read_from_client(client_index);
                }
                else {
                    std::cerr << "Error reading from client " << client_index << ": " << ec.message() << "\n";
                }
            });
    }
};

int main() {
    try {
        boost::asio::io_context io_context;

        RelayServer server(io_context, 12345);

        io_context.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
    }

    return 0;
}


#endif