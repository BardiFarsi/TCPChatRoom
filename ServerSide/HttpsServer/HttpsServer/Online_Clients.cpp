
#include "Online_Clients.h"

Online_Clients::Online_Clients() {}

Online_Clients::~Online_Clients() = default;

std::variant<bool, std::shared_ptr<Client>> Online_Clients::online_user_getter(const std::string& id) const {
	std::lock_guard<std::mutex> lock(onlineUsers_mtx_);
	auto it = std::find_if(onlineUsers_.begin(), onlineUsers_.end(), [&](const auto& pair){
		return pair.first == id; 
	});
	if (it != onlineUsers_.end()) return it->second;
	return false;
	
}

bool Online_Clients::online_user_inserter(const std::string& id, std::shared_ptr<Client> user) {
	std::lock_guard<std::mutex> lock(onlineUsers_mtx_);
	if (onlineUsers_.find(id) == onlineUsers_.end()) {
		onlineUsers_[id] = user; 
		return true; 
	}
	return false; 
}

bool Online_Clients::if_client_exist(const std::string& id) const {
	std::lock_guard<std::mutex> lock(onlineUsers_mtx_);
	return onlineUsers_.find(id) != onlineUsers_.end();
}

bool Online_Clients::remove_online_user(const std::string& id) {
	std::lock_guard<std::mutex> lock(onlineUsers_mtx_);
	if (auto it = onlineUsers_.find(id); it != onlineUsers_.end()) {
		onlineUsers_.erase(it);
		return true;
	}
	return false; 
}
