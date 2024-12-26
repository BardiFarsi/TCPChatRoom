#pragma once
#include "TCP_Connection.h"
#include "LOGGER.h"
#include <boost/asio.hpp>
#include <memory>
#include <functional>

namespace asio = boost::asio;
using io_context = asio::io_context;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

class TCP_Server
{
public:
	TCP_Server(io_context& io_context, const unsigned short port);
	~TCP_Server() = default;
	void handle_new_connection(std::shared_ptr<TCP_Connection> new_connection);
private:
	void start_accept_v4();
	void start_accept_v6();
	void handle_accept_v4(std::shared_ptr<TCP_Connection> newConnection, const error_code& ec);
	void handle_accept_v6(std::shared_ptr<TCP_Connection> newConnection, const error_code& ec);
	io_context& io_context_;
	// Windows can only handle IPV4 AND IPV6 Separately
	tcp::acceptor acceptor_v4_;
	tcp::acceptor acceptor_v6_;
	std::shared_ptr<TCP_Connection> client_1_;
	std::shared_ptr<TCP_Connection> client_2_;
};