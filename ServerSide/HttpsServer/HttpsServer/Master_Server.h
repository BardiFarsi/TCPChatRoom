#pragma once
#include "TCP_Connection.h"
#include "LOGGER.h"
#include "Span_Factory.h"
#include "Client.h"
#include "Online_Clients.h"
#include "All_Clients.h"
#include "Message.h"
#include "TCP_Server.h"
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

template <typename T>
concept validMessageType =
std::same_as<T, std::string> ||
std::same_as<T, std::string_view>;

template <typename T>
concept validContainers =
std::ranges::range<T> &&
(std::same_as<std::ranges::range_value_t<T>, char> ||
	std::same_as<std::ranges::range_value_t<T>, unsigned char> ||
	std::same_as<std::ranges::range_value_t<T>, signed char>);

template <typename T>
concept Message = validMessageType<T> || validContainers<T>;

template<typename T, typename = void>
struct has_data_method : std::false_type {};

template<typename T>
struct has_data_method<T, std::void_t<decltype(std::declval<T>().data())>> : std::true_type {};

template<typename T, typename = void>
struct has_size_method : std::false_type {};

template<typename T>
struct has_size_method<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {};


class Master_Server : public std::enable_shared_from_this<Master_Server>
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
	Master_Server(io_context& io_context, const unsigned short port);
	~Master_Server();

private:
	io_context& io_context_;
	// Windows can only handle IPV4 AND IPV6 Separately
	tcp::acceptor acceptor_v4_;
	void start_accept_v4();
	void handle_accept_v4(std::shared_ptr<TCP_Connection> newConnection, const error_code& ec);
	std::vector<std::shared_ptr<TCP_Connection>> active_connections_;
	std::mutex connections_mtx_;
	std::mutex id_mutex_;
};