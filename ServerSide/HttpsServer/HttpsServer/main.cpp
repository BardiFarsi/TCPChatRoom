#include "LOGGER.h"
#include "TCP_Server.h"
#include "TCP_Connection.h"
#include <exception>
#include <string>
#include <concepts>
#include <memory>
#include <chrono>
#include <mutex>
#include <thread>
#include <iomanip>
#include <ctime>
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

constexpr short DEFAULT_PORT = 3000;

int main() {
	try {
		io_context io_context_v4;
		io_context io_context_v6;
		TCP_Server server_IPV4(io_context_v4, DEFAULT_PORT);
		TCP_Server server_IPV6(io_context_v6, DEFAULT_PORT);
		std::jthread t_v4([&io_context_v4]() { io_context_v4.run(); });
		std::jthread t_v6([&io_context_v6]() { io_context_v6.run(); });
	}
	catch (const std::exception& e) {
		console.log("Unexpected error! ", e.what());
	}
	return 0;
}
