#include "Pair_Chat_Session.h"

Pair_Chat_Session::Pair_Chat_Session(std::shared_ptr<Client> client) : pair_chat_session_(client, nullptr)
{
	std::lock_guard<std::mutex> lock(pair_session_mtx_);
}

Pair_Chat_Session::~Pair_Chat_Session() = default;

bool Pair_Chat_Session::add_client_pair_chat(std::shared_ptr<Client> client) {
	std::lock_guard<std::mutex> lock(pair_session_mtx_);
   
    if (pair_chat_session_.second) {
        return false;  
    }

    if (client == pair_chat_session_.first) {
        return false;
    }

    pair_chat_session_.second = client;
    return true;
}

bool Pair_Chat_Session::remove_client_pair_chat() {
    std::lock_guard<std::mutex> lock(pair_session_mtx_);

    if (pair_chat_session_.second) {
        pair_chat_session_.second = nullptr;
        return true;
    }

    return false; 
}
