
#if 0
#include "Online_Clients.h"

Online_Clients::Online_Clients() {}

Online_Clients::~Online_Clients() = default;

std::string Online_Clients::getter_user_name() const {
	return userName_;
}
std::string Online_Clients::getter_client_id() const {
	return clientId_;
}

using clientVariant = std::variant<std::tuple<std::string, std::weak_ptr<Client>>, bool>;

clientVariant Online_Clients::get_online_client(const std::string id) {
	std::lock_guard<std::mutex> lock(onlineUsers_mtx_);
	auto it = onlineUsers_.find(id); 
	if (it != onlineUsers_.end()) {
		return std::make_tuple(
			id,
			std::weak_ptr<Client>(it -> second)
		);
	}
	return false; 
}
#endif