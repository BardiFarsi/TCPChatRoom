#include "Master_Server.h"

Master_Server::Master_Server(io_context& io_context, const unsigned short port) :
	io_context_(io_context),
	acceptor_v4_(io_context, tcp::endpoint(tcp::v4(), port))
{
	console.log("Server starts running");

	start_accept_v4();
}

Master_Server::~Master_Server() {
	std::lock_guard<std::mutex> lock(connections_mtx_);
	for (auto& connection : active_connections_) {
		connection->stop_process();
	}
	active_connections_.clear();
}

void Master_Server::start_accept_v4() {
	auto newConnection = TCP_Connection::create(io_context_, *this);
	acceptor_v4_.async_accept(newConnection->socket(),
		std::bind(&Master_Server::handle_accept_v4, this, newConnection, asio::placeholders::error));
}

void Master_Server::handle_accept_v4(std::shared_ptr<TCP_Connection> newConnection, const error_code& ec) {
	if (!ec) {
		console.log("Connection from IPv4 accepted.");
		std::string message;
		{
			std::lock_guard<std::mutex> lock(connections_mtx_);
			active_connections_.push_back(newConnection);
		} 
		//GateWay.master_entrance_handeler(newConnection);
	}
	start_accept_v4();
}

void Master_Server::remove_connection(std::shared_ptr<TCP_Connection> connection) {
	std::unique_lock<std::mutex> lock(connections_mtx_);
	auto it = std::find(active_connections_.begin(), active_connections_.end(), connection);

	if (it != active_connections_.end()) {
		active_connections_.erase(it);
	}
	lock.unlock();
}


