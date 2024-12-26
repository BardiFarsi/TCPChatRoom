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
	static std::shared_ptr<TCP_Connection> create(io_context& io_context);
	tcp::socket& socket();
	void start();
private:
	void handle_write(const error_code& ec, size_t bytes_transferred);
	std::string set_time();
	std::string response();
	std::mutex date_mtx_;
	std::mutex response_mtx_;
	tcp::socket socket_;
	std::string message_;
	asio::strand<io_context::executor_type> strand_;
};