#pragma once
#include "TCP_Connection.h"
#include "Message.h"
#include "Client.h"
#include "Online_Clients.h"
#include <utility>
#include <memory>
#include <string>
#include <memory>
#include <span>

class TCP_Connection;
class User_Manager;

class Pair_Chat_Server
{
public:
	template <Message T>
		requires Message<T>
	void send_message_to_partner(T& message, std::shared_ptr<Client> sender) {
		static_assert(has_data_method::value, 
			"Type must have a data() method");
		static_assert(has_size_method::value, 
			"Type must have a size() method");
		std::lock_guard<std::mutex> lock(pair_session_mtx_);
		std::span<const std::byte> bytes = Span_Factory()(message);
		std::string messageStr(
			reinterpret_cast<const char*>(bytes.data()), 
			byte.size()
		);
		if (sender == pair_chat_session_.first) {
			error_code ec; 
			// if (sender == nullptr)
			if (!ec) {
				tcp::socket socket = pair_chat_session_.first->connection_->socket();
				pair_chat_session_.first->connection_->do_write_partner(socket, messageStr);
			}
		}
		else {
			pair_chat_session_.second->connection_->do_write_partner(socket, messageStr);
		}
	}
	bool remove_client_pair_chat(const std::string& id);
	bool add_client_pair_chat(std::shared_ptr<Client> client);
private:
	std::pair<std::shared_ptr<Client>, std::shared_ptr<Client>> pair_chat_session_;
	std::mutex pair_session_mtx_;
};

