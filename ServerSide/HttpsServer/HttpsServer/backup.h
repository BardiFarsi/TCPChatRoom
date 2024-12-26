#pragma once

// This is a draft for debugging purposes - it won't compile
#if 0
#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <mutex>
#include <iostream>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using io_context = asio::io_context;
using error_code = boost::system::error_code;

class TCP_Connection : public std::enable_shared_from_this<TCP_Connection> {
public:
    TCP_Connection(io_context& io_context) :
        socket_(io_context),
        strand_(asio::make_strand(io_context)) {
    }

    static std::shared_ptr<TCP_Connection> create(io_context& io_context) {
        return std::make_shared<TCP_Connection>(io_context);
    }

    tcp::socket& socket() {
        return socket_;
    }

    void start() {
        do_read();
    }

    void send_message(const std::string& message) {
        asio::async_write(socket_, asio::buffer(message),
            asio::bind_executor(strand_,
                [this](const error_code& ec, size_t bytes_transferred) {
                    if (ec) {
                        std::cerr << "Error sending message: " << ec.message() << std::endl;
                    }
                    std::cout << "Sent " << bytes_transferred << " bytes to client." << std::endl;
                }));
    }

    // Set the other user (client) to relay messages
    void set_partner(std::shared_ptr<TCP_Connection> partner) {
        partner_ = partner;
    }

private:
    void do_read() {
        asio::async_read(socket_, asio::buffer(data_),
            asio::bind_executor(strand_,
                [this](const error_code& ec, size_t bytes_transferred) {
                    if (!ec) {
                        std::string message(data_.data(), bytes_transferred);
                        std::cout << "Received: " << message << std::endl;
                        if (partner_) {
                            partner_->send_message(message);
                        }
                        do_read(); // Keep listening for the next message
                    }
                    else {
                        std::cerr << "Error reading data: " << ec.message() << std::endl;
                    }
                }));
    }

    tcp::socket socket_;
    asio::strand<io_context::executor_type> strand_;
    std::shared_ptr<TCP_Connection> partner_; // Reference to the other user
    std::array<char, 512> data_; // Buffer for reading data
};





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


#endif