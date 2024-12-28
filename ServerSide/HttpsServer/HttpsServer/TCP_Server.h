#pragma once
#include "TCP_Connection.h"
#include "LOGGER.h"
#include <boost/asio.hpp>

namespace asio = boost::asio;
using io_context = asio::io_context;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

class TCP_Connection; 

class TCP_Server
{
public:
	TCP_Server(io_context& io_context, const unsigned short port);
	~TCP_Server();
	void remove_connection(std::shared_ptr<TCP_Connection> connection);
private:
	io_context& io_context_;
	// Windows can only handle IPV4 AND IPV6 Separately
	tcp::acceptor acceptor_v4_;
	void start_accept_v4();
	void handle_accept_v4(std::shared_ptr<TCP_Connection> newConnection, const error_code& ec);
	void handle_new_connection(std::shared_ptr<TCP_Connection>& newConnection);
	std::vector<std::shared_ptr<TCP_Connection>> active_connections_;
	std::mutex connections_mutex_;
};