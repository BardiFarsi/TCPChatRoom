#include "TCP_Server.h"

TCP_Server::TCP_Server(io_context& io_context, const unsigned short port) :
	io_context_(io_context),
	acceptor_v4_(io_context, tcp::endpoint(tcp::v4(), port))
{
	start_accept_v4();
}

TCP_Server::~TCP_Server() {
	std::lock_guard<std::mutex> lock(connections_mutex_);
	for (auto& connection : active_connections_) {
		connection->stop();
	}
	active_connections_.clear();
}

void TCP_Server::start_accept_v4() {
	auto newConnection = TCP_Connection::create(io_context_, *this);
	acceptor_v4_.async_accept(newConnection->socket(),
		std::bind(&TCP_Server::handle_accept_v4, this, newConnection, asio::placeholders::error));
}

void TCP_Server::handle_accept_v4(std::shared_ptr<TCP_Connection> newConnection, const error_code& ec) {
	if (!ec) {
		console.log("Connection from IPv4 accepted.");
		{
			std::lock_guard<std::mutex> lock(connections_mutex_);
			active_connections_.push_back(newConnection);
		}
		newConnection->start();
	}
	start_accept_v4();
}

void TCP_Server::handle_new_connection(std::shared_ptr<TCP_Connection>& newConnection) {
	console.log("New connection accepted");
	active_connections_.push_back(newConnection);
	if (active_connections_.size() >= 2) {
		auto& user1 = active_connections_[0];
		auto& user2 = active_connections_.back();

		user1->set_partner(user2.get());
		user2->set_partner(user1.get());
	}
	console.log("Chat session started between two users");
}

void TCP_Server::remove_connection(std::shared_ptr<TCP_Connection> connection) {
	std::lock_guard<std::mutex> lock(connections_mutex_);
	active_connections_.erase(
		std::remove(active_connections_.begin(), active_connections_.end(), connection),
		active_connections_.end()
	);
}
