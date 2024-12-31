#include "TCP_Server.h"

TCP_Server::TCP_Server(io_context& io_context, const unsigned short port) :
	io_context_(io_context),
	acceptor_v4_(io_context, tcp::endpoint(tcp::v4(), port))
{
	console.log("Server starts running");

	start_accept_v4();
}

TCP_Server::~TCP_Server() {
	std::lock_guard<std::mutex> lock(connections_mtx_);
	for (auto& connection : active_connections_) {
		connection->stop_process();
	}
	active_connections_.clear();
}

void TCP_Server::start_accept_v4() {
	auto newConnection = TCP_Connection::create(io_context_, *this);
	acceptor_v4_.async_accept(newConnection->socket(),
		std::bind(&TCP_Server::handle_accept_v4, this, newConnection, asio::placeholders::error));
}
//Here we need a login
void TCP_Server::handle_accept_v4(std::shared_ptr<TCP_Connection> newConnection, const error_code& ec) {
	if (!ec) {
		console.log("Connection from IPv4 accepted.");
		std::string message;
		{
			std::lock_guard<std::mutex> lock(connections_mtx_);
			active_connections_.push_back(newConnection);
			message = " New user joined the chat room. Total users: " +
				std::to_string(active_connections_.size());
		}
		newConnection->start(message);
	}
	start_accept_v4();
}


void TCP_Server::remove_connection(std::shared_ptr<TCP_Connection> connection) {
	std::unique_lock<std::mutex> lock(connections_mtx_);
	auto it = std::find(active_connections_.begin(), active_connections_.end(), connection);

	if (it != active_connections_.end()) {
		active_connections_.erase(it);
	}
	lock.unlock();
}

std::string TCP_Server::client_id_generator(Online_Client& client) {
	try {
		if (!client.clientHasId.load(std::memory_order_acquire)) {
			{
				std::lock_guard<std::mutex> lock(id_mutex_);
				if (std::any_of(online_clients_id.begin(), online_clients_id.end(),
					[&client](const auto& pair) { return pair.second == &client; })) {
					throw std::runtime_error("Error! Client already exists in map!");
				}
			}

			std::string id;
			bool is_unique = false;

			while (!is_unique) {
				id.clear(); 
				id = create_new_id();
				std::lock_guard<std::mutex> lock(id_mutex_);
				if (online_clients_id.find(id) == online_clients_id.end()) {
					online_clients_id[id] = &client;
					is_unique = true;
				}
			}

			return id;
		}
		else {
			throw std::runtime_error("Error! Client already has an ID!");
		}
	}
	catch (const std::exception& e) {
		throw std::runtime_error(std::format("Failed to generate client ID: {}", e.what()));
	}
}

std::string TCP_Server::create_new_id() {
	auto now = std::chrono::system_clock::now();
	auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
		now.time_since_epoch()
	).count();

	std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFF);  
	uint32_t random_component = dist(gen);

	return std::format("uid{:0{}}:{:0{}x}",
		timestamp, TIMESTAMP_LENGTH,
		random_component, RANDOM_LENGTH);
}