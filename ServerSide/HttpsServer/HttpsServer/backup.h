#pragma once

// This is a draft for debugging purposes - it won't compile
#if 0

#pragma once
#include "LOGGER.h"
#include "Client.h"
#include "Online_Clients.h"
#include "Message.h"
#include "Span_Factory.h"
#include <utility>
#include <memory>
#include <string>
#include <span>
#include <vector>
#include <unordered_map>
#include <array>
#include <iostream>
#include <ranges>
#include <type_traits>
#include <cstdint>
#include <boost/asio.hpp> 

namespace asio = boost::asio;
using io_context = asio::io_context;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

class User_Manager;

class Pair_Chat_Session
{
public:
    Pair_Chat_Session();
    ~Pair_Chat_Session();

    template <typename T>
        requires Message<T>
    void send_message_to_partner(T& message, std::shared_ptr<Client> sender) {
        static_assert(has_data_method<T>::value,
            "Type must have a data() method");
        static_assert(has_size_method<T>::value,
            "Type must have a size() method");
        std::lock_guard<std::mutex> lock(pair_session_mtx_);
        std::span<const std::byte> bytes = Span_Factory()(message);
        std::string messageStr(
            reinterpret_cast<const char*>(bytes.data()),
            bytes.size()
        );

        error_code ec;
        try {
            if (sender == pair_chat_session_.first) {
                // if (sender == nullptr) for server
                if (!ec) {
                    tcp::socket& socket = sender_->socket();
                    asio::write(socket, asio::buffer(broadcast_str), ec);
                }
            }
            else {
                if (!ec) {


                }
            }
        }
        catch (...) {
            catch_handler(sender->connection_);
        }
    }

    bool add_owner_to_chat(std::shared_ptr<Client> client);
    bool remove_owner_to_chat();
    bool remove_partner_pair_chat();
    bool add_client_pair_chat(std::shared_ptr<Client> partner);
    void catch_handler(std::shared_ptr<TCP_Connection> connection);

private:
    std::pair<std::shared_ptr<Client>, std::shared_ptr<Client>> pair_chat_session_;
    std::mutex pair_session_mtx_;
    std::shared_ptr<TCP_Connection> sender_;
    std::shared_ptr<TCP_Connection> partner_;
};

#include "TCP_Connection.h"



#include "Pair_Chat_Session.h"

Pair_Chat_Session::Pair_Chat_Session() :
    sender_(nullptr),
    partner_(nullptr) {
}

Pair_Chat_Session::~Pair_Chat_Session() = default;


bool Pair_Chat_Session::add_owner_to_chat(std::shared_ptr<Client> client) {
    std::lock_guard<std::mutex> lock(pair_session_mtx_);
    sender_ = client->get_connection();
    if (!pair_chat_session_.first) {
        pair_chat_session_.first = client;
        return true;
    }

    return false;
}

bool Pair_Chat_Session::remove_owner_to_chat() {
    std::lock_guard<std::mutex> lock(pair_session_mtx_);
    sender_ = nullptr;

    if (pair_chat_session_.first) {
        pair_chat_session_.first = nullptr;
        return true;
    }

    return false;
}

bool Pair_Chat_Session::add_client_pair_chat(std::shared_ptr<Client> partner) {
    std::lock_guard<std::mutex> lock(pair_session_mtx_);
    partner_ = partner->get_connection();

    if (pair_chat_session_.second) {
        return false;
    }

    if (partner == pair_chat_session_.first) {
        return false;
    }

    pair_chat_session_.second = partner;
    return true;
}

bool Pair_Chat_Session::remove_partner_pair_chat() {
    std::lock_guard<std::mutex> lock(pair_session_mtx_);
    partner_ = nullptr;

    if (pair_chat_session_.second) {
        pair_chat_session_.second = nullptr;
        return true;
    }

    return false;
}

void Pair_Chat_Session::catch_handler(std::shared_ptr<TCP_Connection> connection) {
    try {
        throw;
    }
    catch (const boost::system::system_error& e) {
        console.log("Network error during sign up: ", e.what());
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("Network error occurred during registration. Please try again later.");
            connection->running_.store(false, std::memory_order_release);
        }
    }
    catch (const std::runtime_error& e) {
        console.log("Sign up process failed! ", e.what());
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("An unexpected error occurred during registration.");
            connection->running_.store(false, std::memory_order_release);
        }
    }
    catch (...) {
        console.log("Unknown error during sign up process");
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("An unexpected error occurred during registration.");
            connection->running_.store(false, std::memory_order_release);
        }
    }
}

#pragma once
#include "Client.h"
#include "LOGGER.h"
#include "Master_Server.h"
#include "TCP_Connection.h"
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <mutex>
#include <algorithm>
#include <variant>
#include <type_traits>

class Online_Clients
{
public:
    Online_Clients();
    ~Online_Clients();
    std::variant<bool, std::shared_ptr<Client>> online_user_id_getter(const std::string& id) const;
    std::variant<bool, std::shared_ptr<Client>> online_user_email_getter(const std::string& email) const;
    bool online_user_id_inserter(const std::string& id, std::shared_ptr<Client> user);
    bool online_user_email_inserter(const std::string& email, std::shared_ptr<Client> user);
    bool if_user_id_exist(const std::string& id) const;
    bool if_user_email_exist(const std::string& email) const;
    bool remove_user_id_online(const std::string& id);
    bool remove_user_email_online(const std::string& email);
    std::shared_ptr<Client> search_user_by_key(const std::string& key);
    bool is_email(const std::string& key);

private:
    std::unordered_map<std::string, std::shared_ptr<Client>> onlineUsersId_;
    std::unordered_map<std::string, std::shared_ptr<Client>> onlineUsersEmail_;
    mutable std::mutex onlineUsersId_mtx_;
    mutable std::mutex onlineUsersEmail_mtx_;
};

extern Online_Clients onlineUsers;

#endif