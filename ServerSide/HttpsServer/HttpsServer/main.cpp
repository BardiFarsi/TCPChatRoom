#include "LOGGER.h"

#include "TCP_Connection.h"
#include "All_Clients.h"
#include "Master_Server.h"
#include <exception>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include <boost/version.hpp>
#include <nlohmann/json.hpp>

#ifndef WIN32
#define WIN32 0x0A00  
#endif

#if BOOST_VERSION < 108600
#   error "This program need boost 1.86.0 or higher!"
#endif

#ifndef _HAS_CXX20
#define _HAS_CXX20 1 
#endif

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using io_context = asio::io_context;
using error_code = boost::system::error_code;
using resolver = tcp::resolver;

constexpr unsigned short DEFAULT_PORT = 3000;

int main() {
	try {
		io_context io_context;
		/*
		TCP_Server server(io_context, DEFAULT_PORT);
		std::jthread t_TCP([&io_context]() { io_context.run(); });
		
		*/
		Master_Server masterServer(io_context, DEFAULT_PORT);
		std::jthread t_TCP([&io_context]() { io_context.run(); });

	}
	catch (const std::exception& e) {
		console.log("Unexpected error! ", e.what());
	}
	return 0;
}