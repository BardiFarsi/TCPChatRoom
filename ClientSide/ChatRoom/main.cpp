#include "LOGGER.h"
#include "Buffer_Sanitizer.h"
#include "Span_Factory.h"
#include "Client.h"
#include <exception>
#include <string>
#include <chrono>
#include <mutex>
#include <thread>
#include <boost/asio.hpp>
#include <boost/version.hpp>

#ifndef WIN32
#define WIN32 0x0A00  
#endif

#if BOOST_VERSION < 108600
#   error "This program need boost 1.86.0 or higher!"
#endif

namespace asio = boost::asio;
using tcp = asio::ip::tcp;
using io_context = asio::io_context;

// Fof the comment section below
using std::cin;

static std::string HOST { "127.0.0.1" }; 
static std::string PORT { "3000" };

int main() {
	console.log("Welcome to bare-metal chat.");
	console.log("In a moment you will be connected to server and may be able to chat to your partner.");
	console.log("To exit the program please enter 'Exit++'.");
	
	// To define the Host and Port on run time, uncomment the code below.
	/*
	console.log("Enter the Host you want to connect: ");
	cin >> HOST;
	console.log("Enter the Port for the service type you would like to connect: ");
	cin >> PORT;
	console.log("Connecting to the Host: ", HOST, " and the Port: ", PORT);
	*/

	try {
		io_context io_context;
		asio::executor_work_guard<io_context::executor_type>work_guard(io_context.get_executor());
		tcp::socket socket(io_context);
		Client client(io_context);
		client.connect(HOST, PORT);

		std::jthread io_thread([&io_context]() {
			io_context.run();
			});
		io_thread.join();
		client.stop(); 
	}
	catch (const std::exception& e) {
		console.log("Unexpected error! ", e.what());
		return 1;
	}

	return 0;
}
