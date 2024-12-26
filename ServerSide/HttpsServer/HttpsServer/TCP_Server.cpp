#include "TCP_Server.h"

TCP_Server::TCP_Server(io_context& io_context, const unsigned short port) :
	io_context_(io_context),
	acceptor_v4_(io_context, tcp::endpoint(tcp::v4(), port)),
	acceptor_v6_(io_context, tcp::endpoint(tcp::v6(), port))
{
	start_accept_v4();
	start_accept_v6();
}

void TCP_Server::start_accept_v4() {
	auto new_connection = TCP_Connection::create(io_context_);
	acceptor_v4_.async_accept(new_connection->socket(),
		[this, new_connection](const error_code& ec) {
			if (!ec) {
				console.log("New connection accepted with IPv4.");
				handle_new_connection(new_connection);
			}
			start_accept_v4(); // Accept the next client
		});
}

void TCP_Server::start_accept_v6() {
	auto new_connection = TCP_Connection::create(io_context_);
	acceptor_v6_.async_accept(new_connection->socket(),
		[this, new_connection](const error_code& ec) {
			if (!ec) {
				console.log("New connection accepted with IPv6.");
				handle_new_connection(new_connection);
			}
			start_accept_v6(); // Accept the next client
		});
}

void TCP_Server::handle_new_connection(std::shared_ptr<TCP_Connection> new_connection) {
	if (client_1_ == nullptr) {
		client_1_ = new_connection;
		std::cout << "Client 1 connected." << std::endl;
	}
	else if (client_2_ == nullptr) {
		client_2_ = new_connection;
		std::cout << "Client 2 connected." << std::endl;

		// Pair the two clients together
		client_1_->set_partner(client_2_);
		client_2_->set_partner(client_1_);

		// Start communication between the two clients
		client_1_->start();
		client_2_->start();
	}
	else {
		std::string warning = "Max clients reached. Rejecting new connection.";
		new_connection->send_message(warning);
	}
}


void TCP_Server::handle_accept_v4(std::shared_ptr<TCP_Connection> newConnection, const error_code& ec) {
	if (!ec) {
		console.log("Connection from IPv4 accepted.");
		newConnection->start();
	}
	start_accept_v4();
}

void TCP_Server::handle_accept_v6(std::shared_ptr<TCP_Connection> newConnection, const error_code& ec) {
	if (!ec) {
		console.log("Connection from IPv6 accepted.");
		newConnection->start();
	}
	start_accept_v6();
}