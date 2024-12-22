#define _CRT_SECURE_NO_WARNINGS
#include "LOGGER.h"
#include "Span_Factory.h"
#include "Buffer_Sanitizer.h"
#include <vector>
#include <exception>
#include <string>
#include <span>
#include <concepts>
#include <memory>
#include <chrono>
#include <mutex>
#include <thread>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/version.hpp>
#include <nlohmann/json.hpp>

#ifndef WIN32
#define WIN32 0x0A00  
#endif

#if BOOST_VERSION < 108600
#   error "This program need boost 1.86.0 or higher!"
#endif


namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using io_context = asio::io_context;
using error_code = boost::system::error_code;
using resolver = tcp::resolver;

std::mutex date_mtx;
std::mutex response_mtx;

std::string reset_time();
void response(tcp::socket& socket); 

int main() {
	try {
		io_context io_context; 
		// Windows can only handle IPV4 AND IPV6 Separately
		tcp::acceptor acceptor_v4(io_context, tcp::endpoint(tcp::v4(), 3000));
		tcp::acceptor acceptor_v6(io_context, tcp::endpoint(tcp::v6(), 3000));
		std::mutex acceptorV4Mtx;
		std::mutex acceptorV6Mtx;

		std::jthread t_v4([&]() {
			while (true) {
				std::lock_guard<std::mutex> lock(acceptorV4Mtx);
				tcp::socket socket(io_context);
				acceptor_v4.accept(socket);
				response(socket);
			}
		});

		std::jthread t_v6([&]() {
			std::lock_guard<std::mutex> lock(acceptorV6Mtx);
			tcp::socket socket(io_context);
			acceptor_v6.accept(socket);
			response(socket);
		});

		t_v4.join();
		t_v6.join();
	}
	catch (const std::exception& e) {
		console.log("Unexpected error! ", e.what());
	}
	return 0;
}


std::string reset_time() {
	try {
		auto now = std::chrono::system_clock::now();
		std::time_t time = std::chrono::system_clock::to_time_t(now);
		std::tm localTime;
		{
			std::lock_guard<std::mutex> lock(date_mtx);
			std::tm* LTPtr = std::localtime(&time);
			if (!LTPtr) {
				return "Error! std::localtime returned nullptr.";
			}
			else {
				localTime = *LTPtr;
			}
			std::stringstream ss;
			ss << std::put_time(&localTime, "%a %b %d %H:%M:%S %Y");
			return ss.str();
		}
	}
	catch (const std::exception& e) { // for std::bad_alloc & etc
		std::string err = "Caught error in time conversion: ";
		err += e.what();
		return err;
	}
}

void response(tcp::socket& socket) {
	try {
		std::lock_guard<std::mutex> lock(response_mtx);
		std::string dayTime = reset_time();
		error_code ignored_error;
		asio::write(socket, asio::buffer(dayTime), ignored_error);
	}
	catch (const std::exception& e) {
		console.log("Caught error in Response function: ", e.what());
	}
}