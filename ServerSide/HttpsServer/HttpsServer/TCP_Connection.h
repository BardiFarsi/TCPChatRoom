#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "LOGGER.h"
#include "Buffer_Sanitizer.h"
#include "User_Manager.h"
#include "Message.h"
#include <memory>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <iomanip>
#include <ctime>
#include <functional>
#include <type_traits>
#include <utility>
#include <boost/asio.hpp>


namespace asio = boost::asio;
using io_context = asio::io_context;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

constexpr size_t BUFF_SIZE{ 1024 };
static std::once_flag stop_flag_;

class Master_Server;

class TCP_Connection : public std::enable_shared_from_this<TCP_Connection>
{
public:
	TCP_Connection(io_context& io_context, Master_Server& masterServer);
	virtual  ~TCP_Connection();
	static std::shared_ptr<TCP_Connection> create(io_context& io_context, Master_Server& masterServer);
	tcp::socket& socket();
	void start(const std::string& message);
	void do_read();
	void do_write(const std::string& message);
	void do_prompt_user(const std::string& message);
	std::string read_from_user();
	void stop_process();
	TCP_Connection(TCP_Connection&& other) noexcept = default;
	TCP_Connection& operator=(TCP_Connection&& other) noexcept = default;
	std::atomic<bool> running_;
private:
	void stop();
	void handle_communication();
	std::string set_time();
	std::string response_time();
	Master_Server& masterServer_;
	tcp::socket socket_;
	error_code ec_;
	asio::strand<io_context::executor_type> strand_;
	std::mutex date_mtx_;
	std::mutex response_mtx_;
	std::mutex read_mtx_;
	std::mutex write_mtx_;
	std::mutex stop_mtx_;
	std::thread read_thread_;
	std::thread write_thread_;
	std::string userId_;
	std::string message_;
	std::string readError_{ "Read Error!" };
	std::vector<char> readData_;
	std::vector<char> writeData_;
};