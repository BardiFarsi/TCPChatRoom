#include "LOGGER.h"
#include "Buffer_Sanitizer.h"
#include "boost/asio.hpp"
#include <boost/asio/ts/buffer.hpp>
#include <boost/asio/ts/internet.hpp>
#include <boost/thread.hpp>

namespace asio = boost::asio;
using io_context = asio::io_context;
using error_code = boost::system::error_code;
using endPoint = asio::ip::tcp::endpoint;
using tcpSocket = asio::ip::tcp::socket; //https
using tcp = asio::ip::tcp;
using resolver = asio::ip::tcp::resolver;

constexpr size_t BUFF_SIZE{ 1024 };

int main() {
	std::string ipAddress = "127.0.0.1";
	std::string portNum = "3000";



	console.log("TEST");
	return 0;
}