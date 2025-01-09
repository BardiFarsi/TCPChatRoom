
#include "Online_Clients.h"

Online_Clients::Online_Clients() {}

Online_Clients::~Online_Clients() = default;

std::variant<bool, std::shared_ptr<Client>> Online_Clients::online_user_id_getter(const std::string& id) const {
	std::lock_guard<std::mutex> lock(onlineUsersId_mtx_);
	auto it = std::find_if(onlineUsersId_.begin(), onlineUsersId_.end(), [&](const auto& pair){
		return pair.first == id; 
	});
	if (it != onlineUsersId_.end()) return it->second;
	return false;
	
}

std::variant<bool, std::shared_ptr<Client>> Online_Clients::online_user_email_getter(const std::string& email) const {
	std::lock_guard<std::mutex> lock(onlineUsersEmail_mtx_);
	auto it = std::find_if(onlineUsersEmail_.begin(), onlineUsersEmail_.end(), [&](const auto& pair) {
		return pair.first == email;
		});
	if (it != onlineUsersEmail_.end()) return it->second;
	return false;

}

bool Online_Clients::online_user_id_inserter(const std::string& id, std::shared_ptr<Client> user) {
	std::lock_guard<std::mutex> lock(onlineUsersId_mtx_);
	if (onlineUsersId_.find(id) == onlineUsersId_.end()) {
		onlineUsersId_[id] = user; 
		return true; 
	}
	return false; 
}

bool Online_Clients::online_user_email_inserter(const std::string& email, std::shared_ptr<Client> user) {
	std::lock_guard<std::mutex> lock(onlineUsersEmail_mtx_);
	if (onlineUsersEmail_.find(email) == onlineUsersEmail_.end()) {
		onlineUsersEmail_[email] = user;
		return true;
	}
	return false;
}

bool Online_Clients::if_user_id_exist(const std::string& id) const {
	std::lock_guard<std::mutex> lock(onlineUsersId_mtx_);
	return onlineUsersId_.find(id) != onlineUsersId_.end();
}

bool Online_Clients::if_user_email_exist(const std::string& email) const {
	std::lock_guard<std::mutex> lock(onlineUsersEmail_mtx_);
	return onlineUsersEmail_.find(email) != onlineUsersEmail_.end();
}

bool Online_Clients::remove_user_id_online(const std::string& id) {
	std::lock_guard<std::mutex> lock(onlineUsersId_mtx_);
	if (auto it = onlineUsersId_.find(id); it != onlineUsersId_.end()) {
		onlineUsersId_.erase(it);
		return true;
	}
	return false; 
}

bool Online_Clients::remove_user_email_online(const std::string& email) {
	std::lock_guard<std::mutex> lock(onlineUsersEmail_mtx_);
	if (auto it = onlineUsersEmail_.find(email); it != onlineUsersEmail_.end()) {
		onlineUsersEmail_.erase(it);
		return true;
	}
	return false;
}

std::shared_ptr<Client> Online_Clients::search_user_by_key(const std::string& key) {
	bool keyFormat = is_email(key);
	auto result = (keyFormat) ? online_user_email_getter(key) 
							  : online_user_id_getter(key);

	return std::visit([](auto&& arg) -> std::shared_ptr<Client> {
		return std::is_same_v<std::decay_t<decltype(arg)>, bool> ? nullptr : arg;
		}, result);
	
}

bool Online_Clients::is_email(const std::string& key) {
	return key.find('@') != std::string::npos &&
		   key.find('.') != std::string::npos &&
		   key.find('@') < key.find('.');
}