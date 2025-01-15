#include "Pair_Chat_Session.h"


Pair_Chat_Session::Pair_Chat_Session(std::shared_ptr<Client> owner) 
{
    pair_chat_session_[0] = owner;
}

Pair_Chat_Session::~Pair_Chat_Session() = default;


bool Pair_Chat_Session::add_partner_pair_chat(std::shared_ptr<Client> partner) {
    std::lock_guard<std::mutex> lock(pair_session_mtx_);
   
    if (pair_chat_session_[1] || partner == pair_chat_session_[0] ) {
        return false;
    }

    pair_chat_session_[1] = partner;
    return true;
}

bool Pair_Chat_Session::remove_partner_pair_chat() {
    std::lock_guard<std::mutex> lock(pair_session_mtx_);

    if (pair_chat_session_[1]) {
        pair_chat_session_[1] = nullptr;
        return true;
    }

    return false;
}
