#include "LOGGER.h"
#include "Buffer_Sanitizer.h"
#include "Span_Factory.h"
#include <vector>
#include <exception>
#include <string>
#include <span>
#include <concepts>
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

constexpr size_t BUFF_SIZE{ 1024 };
std::mutex date_mtx;

int main(int argc, char* argv[]) {
	try {
		if (argc != 3) {
			console.log("Invalid argument. Please provide a valid host.");
			return 1;
		}
		else {
			std::string host = argv[1];
			std::string port = argv[2]; 
			io_context io_context;
			error_code ec; 
			tcp::resolver resolver(io_context);
			tcp::resolver::results_type endpoints = resolver.resolve(host, port, ec);
			tcp::socket socket(io_context);
			connect(socket, endpoints, ec);
			if (!ec) {
				while (true) {
					std::vector<char> buffVec(BUFF_SIZE);
					size_t length = socket.read_some(asio::buffer(buffVec), ec);
					if (ec == asio::error::eof) {
						break; 
					}
					else if (ec) {
						console.log("Unexpected error from error_code! ", ec.what());
						throw boost::system::system_error(ec);
					}
					else {
						Span_Factory make_span;
						std::span<std::byte> spanBuffer = make_span(buffVec);
						Buffer_Sanitizer sanitizer;
						std::string response = sanitizer(spanBuffer);
						console.log("The response from span: ", response);
					}
				}
			}
			else {
				console.log("Unexpected error from error_code! ", ec.what());
			}
		}
	}
	catch (const std::exception& e) {
		console.log("Unexpected error! ", e.what());
	}

	return 0;
}
