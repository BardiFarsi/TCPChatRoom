#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "LOGGER.h"
#include "Buffer_Sanitizer.h"
#include "Span_Factory.h"
#include <vector>
#include <exception>
#include <string>
#include <algorithm>
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using io_context = asio::io_context;
using error_code = boost::system::error_code;
using resolver = tcp::resolver;
using std::cin;

constexpr size_t BUFF_SIZE{ 1024 };

class Client
{
public:
    Client(io_context& io_context);
    ~Client();
    void start();
    void connect(const std::string& host, const std::string& port);
    void stop(); 

private:
    void do_read();
    void do_write();
    std::string set_time();
    std::string response();
    void stop_process();
    tcp::socket socket_;
    asio::io_context& io_context_;
    asio::strand<asio::io_context::executor_type> strand_;
    std::string message_;
    std::string userInput_;
    std::string username_;
    std::vector<char> writeData_;
    std::vector<char> readData_;
    std::atomic<bool> running_;
    std::once_flag stop_flag_;
    std::mutex write_mtx_;
    std::mutex date_mtx_;
    std::mutex read_mtx_;
    std::mutex response_mtx_;
    std::mutex start_mtx_;
    std::mutex stop_mtx_;
    std::thread read_thread_;
    std::thread write_thread_;
};

