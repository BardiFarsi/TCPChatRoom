#pragma once
#include "TCP_Connection.h"
#include "User_Manager.h"
#include "LOGGER.h"
#include "Span_Factory.h"
#include "Client.h"
#include "Online_Clients.h"
#include "All_Clients.h"
#include "Message.h"
#include "TCP_Server.h"
#include "Prompt.h"
#include <vector>
#include <unordered_map>
#include <string>
#include <array>
#include <span>
#include <iostream>
#include <memory>
#include <ranges>
#include <type_traits>
#include <mutex>
#include <cstdint>
#include <stdexcept>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using io_context = asio::io_context;
using tcp = asio::ip::tcp;
using error_code = boost::system::error_code;

class Pair_Chat_Session : public std::enable_shared_from_this<Pair_Chat_Session>
{
public:
    template<typename T>
        requires Message<T>
    void send_message_to_partner(T& message) {
        static_assert(has_data_method<T>::value,
            "Type must have a data() method");
        static_assert(has_size_method<T>::value,
            "Type must have a size() method");
        std::lock_guard<std::mutex> lock(pair_session_mtx_);
        std::span<const std::byte> bytes = Span_Factory()(message);
        std::string message_str(
            reinterpret_cast<const char*>(bytes.data()),
            bytes.size()
        );
        
        if (pair_chat_session_[1]) {
            auto reciever = pair_chat_session_[1];
            error_code ec;

            if (!ec) {
                reciever->get_connection()->do_prompt_user(message_str);
            }
            else {
                console.log(g_errorConnection, ec.message());
            }
        }
        else {

            if (pair_chat_session_[0]) {
                auto sender = pair_chat_session_[0]; 
                error_code ec;

                if (!ec) {
                    sender->get_connection()->do_prompt_user(g_partnerUnavailable);
                }
                else {
                    console.log(g_errorConnection, ec.message());
                }
            }
            else {
                throw std::runtime_error("Unknown Error in Pair Chat");
            }
        }
    }

    Pair_Chat_Session(std::shared_ptr<Client> owner);
    ~Pair_Chat_Session();

    bool add_partner_pair_chat(std::shared_ptr<Client> partner);
    bool remove_partner_pair_chat();

private:
    std::array<std::shared_ptr<Client>, 2> pair_chat_session_;
    std::mutex pair_session_mtx_;
};