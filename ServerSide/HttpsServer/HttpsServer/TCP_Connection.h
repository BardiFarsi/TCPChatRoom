#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "LOGGER.h"
#include "Buffer_Sanitizer.h"
#include "TCP_Server.h"
#include <memory>
#include <chrono>
#include <mutex>
#include <thread>
#include <iomanip>
#include <ctime>
#include <functional>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using io_context = asio::io_context;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

constexpr size_t BUFF_SIZE{ 1024 };

class TCP_Server;

class TCP_Connection : public std::enable_shared_from_this<TCP_Connection>
{
public:
	TCP_Connection(io_context& io_context, TCP_Server& server);
	~TCP_Connection();
	static std::shared_ptr<TCP_Connection> create(io_context& io_context, TCP_Server& server);
	tcp::socket& socket();
	void start(const std::string& message);
	void do_read();
	void do_write(const std::string& message);
	void stop();

private:
	void handle_communication();
	std::string set_time();
	std::string response();
	TCP_Server& server_;
	tcp::socket socket_;
	error_code ec_;
	asio::strand<io_context::executor_type> strand_;
	TCP_Connection* otherUser_;
	bool running_; 
	std::mutex date_mtx_;
	std::mutex response_mtx_;
	std::mutex read_mtx_;
	std::mutex write_mtx_;
	std::thread read_thread_;
	std::thread write_thread_;
	std::string userId_;
	std::string message_;

	std::vector<char> readData_;
	std::vector<char> writeData_;
};