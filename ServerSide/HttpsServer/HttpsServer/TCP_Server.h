#if 0

#pragma once
#include "TCP_Connection.h"
#include "LOGGER.h"
#include "Span_Factory.h"
#include "Master_Server.h"
#include "Message.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <array>
#include <span>
#include <iostream>
#include <memory>
#include <ranges>
#include <type_traits>
#include <random>
#include <format>
#include <chrono>
#include <mutex>
#include <cstdint>
#include <stdexcept>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using io_context = asio::io_context;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

class TCP_Connection; 
class Client;
class Online_Client;

class TCP_Server
{
public:
	template<Message T>
		requires Message<T>
	void broadcast_message(T& message, std::shared_ptr<TCP_Connection> sender) {
		static_assert(has_data_method<T>::value,
			"Type must have a data() method");
		static_assert(has_size_method<T>::value,
			"Type must have a size() method");
		std::lock_guard<std::mutex> lock(connections_mtx_);
		std::span<const std::byte> bytes = Span_Factory()(message);
		std::string broadcast_str(
			reinterpret_cast<const char*>(bytes.data()),
			bytes.size()
		);
		for (auto& connection : active_connections_) {
			if (sender == nullptr || connection != sender) {
				error_code ec;
				if (!ec) {
					tcp::socket& socket = connection->socket();
					asio::write(socket, asio::buffer(broadcast_str), ec);
				}
				else {
					console.log("Error broadcasting a message! ", ec.message());
				}
			}
		}
	}

	void remove_connection(std::shared_ptr<TCP_Connection> connection);
	TCP_Server(io_context& io_context, const unsigned short port);
	~TCP_Server();

private:
	static constexpr size_t TIMESTAMP_LENGTH = 10;
	static constexpr size_t RANDOM_LENGTH = 6;
	static constexpr size_t PREFIX_LENGTH = 3;
	io_context& io_context_;
	tcp::acceptor acceptor_v4_;
	void start_accept_v4();
	void handle_accept_v4(std::shared_ptr<TCP_Connection> newConnection, const error_code& ec);
	std::vector<std::shared_ptr<TCP_Connection>> active_connections_;
	std::mutex connections_mtx_;
	std::mutex id_mutex_;
	std::random_device rd;
	std::mt19937_64 gen;
};

#endif