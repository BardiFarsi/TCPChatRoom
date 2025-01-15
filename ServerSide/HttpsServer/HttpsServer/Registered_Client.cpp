#include "Registered_Client.h"

Registered_Client::Registered_Client(std::shared_ptr<TCP_Connection> connection, std::string clientId, std::string email) :
	Client(connection, email), 
	clientId_(clientId), 
	name_(""),
	pairChat_(std::make_shared<Pair_Chat_Session>(shared_from_this()))
{
	clientHasId.store(false, std::memory_order_relaxed);
}

Registered_Client::~Registered_Client() = default;

std::string Registered_Client::get_client_id() const {
	return clientId_;
}

std::string Registered_Client::get_client_name() const {
	return name_;
}

std::string Registered_Client::get_client_email() const {
	return email_;
}

std::shared_ptr<TCP_Connection> Registered_Client::get_connection() const{
	return connection_;
}

bool Registered_Client::set_connection(std::shared_ptr<TCP_Connection> connection) {
    if (!connection) {
        return false;  
    }
    connection_ = connection;  
    return true;
}

void Registered_Client::write_client_id(const std::string& id) {
	clientId_ = id;
}

void Registered_Client::write_client_name(const std::string& name) {
	name_ = name;
}

bool Registered_Client::add_friend(const std::string& email) {
    if (email.empty()) {
        return false;
    }

    auto userFriend = OnlineUsers.online_user_by_email_getter(email);
    bool success = false;

    std::visit([&](const auto& result) {
        using T = std::decay_t<decltype(result)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Client>>) {
            if (!is_friend_exist(email)) {
                std::lock_guard<std::mutex> lock(friendList_mtx_);
                auto [it, inserted] = friendList_.emplace(email, result);
                success = inserted; 
            }
        }
        }, userFriend);

    return success;  
}

bool Registered_Client::remove_friend(const std::string& email) {
	std::lock_guard<std::mutex> lock(friendList_mtx_);

	if (auto it = friendList_.find(email); it != friendList_.end()) {
		friendList_.erase(it);
		return true;
	}

	return false; 
}

bool Registered_Client::is_friend_exist(const std::string& email) {
	std::lock_guard<std::mutex> lock(friendList_mtx_);

	return friendList_.find(email) != friendList_.end();
}
