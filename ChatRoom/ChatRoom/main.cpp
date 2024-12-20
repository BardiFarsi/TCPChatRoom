#include "LOGGER.h"
#include "Buffer_Sanitizer.h"
#include "Make_Span.h"
#include <vector>
#include <exception>
#include <string>
#include <span>
#include <concepts>
#include <boost/asio.hpp>
#include <boost/version.hpp>
#include <nlohmann/json.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#if BOOST_VERSION < 108600
#   error "This program need boost 1.86.0 or higher!"
#endif



namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using io_context = asio::io_context;
using error_code = boost::system::error_code;
using resolver = tcp::resolver;
using tcpSocket = tcp::socket;

constexpr size_t BUFF_SIZE{ 1024 };

int main(int argc, char* argv[]) {
	try {
		if (argc != 2) {
			console.log("Invalid argument. Please provide a valid host.");
		}
		else {
			std::string serviceType{"getRequest"};
			io_context io_context;
			tcp::resolver resolver(io_context);
			tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1:3000", "serviceType"); //argv[1]
			tcpSocket socket(io_context);
			connect(socket, endpoints);
			while (true) {
				std::vector<char> buffVec(BUFF_SIZE);
				error_code ec; 
				size_t length = socket.read_some(asio::buffer(buffVec), ec);
				if (ec == asio::error::eof) {
					break; 
				} 
				else if (ec) {
					throw boost::system::system_error(ec);
				}
				else {
					std::span<std::byte> buffSpan = Make_Span(buffVec);
					asio::mutable_buffer mtBuffer(buffVec.data(), length); 
					Buffer_Sanitizer sanitizer;
					std::string responseMT = sanitizer(mtBuffer);
					std::string responseSpan = sanitizer(buffSpan);
					std::string responseVec = sanitizer(buffVec);
					console.log("The response from Boost Mutable Obj:", responseMT);
					console.log("The response from span:", responseSpan);
					console.log("The response from Vector:", responseVec);
				}
			}
		}
	}
	catch (const std::exception& e) {
		console.log("Unexpected error!", e.what());
	}


	
	return 0;
}