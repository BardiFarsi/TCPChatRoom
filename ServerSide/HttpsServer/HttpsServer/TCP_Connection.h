#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "LOGGER.h"
#include <memory>
#include <chrono>
#include <mutex>
#include <thread>
#include <iomanip>
#include <ctime>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using io_context = asio::io_context;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

class TCP_Connection : public std::enable_shared_from_this<TCP_Connection>
{
public:
	TCP_Connection(io_context& io_context);
	~TCP_Connection() = default;
	void start();
	tcp::socket& socket();
	static std::shared_ptr<TCP_Connection> create(io_context& io_context);
	void send_message(const std::string& message); 
	void set_partner(std::shared_ptr<TCP_Connection> partner);
	std::string message_;

	void handle_write(const error_code& ec, size_t bytes_transferred);
	std::string set_time();
	std::string response();
	void do_read(); 
	std::mutex date_mtx_;
	std::mutex response_mtx_;
	tcp::socket socket_;
	asio::strand<io_context::executor_type> strand_;
	std::array<char, 1024> data_;
	std::shared_ptr<TCP_Connection> partner_; 
};