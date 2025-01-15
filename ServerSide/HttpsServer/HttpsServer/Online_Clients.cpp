
#include "Online_Clients.h"

Online_Clients OnlineUsers;

Online_Clients::Online_Clients() {}

Online_Clients::~Online_Clients() = default;

std::variant<bool, std::shared_ptr<Client>> Online_Clients::online_user_by_id_getter(const std::string& id) const {
	std::lock_guard<std::mutex> lock(onlineUsersId_mtx_);
    auto it = onlineUsersId_.find(id); 
    if (it != onlineUsersId_.end()) return it->second;
    return false;	
}

std::variant<bool, std::shared_ptr<Client>> Online_Clients::online_user_by_email_getter(const std::string& email) const {
	std::lock_guard<std::mutex> lock(onlineUsersEmail_mtx_);
    auto it = onlineUsersEmail_.find(email);  
    if (it != onlineUsersEmail_.end()) return it->second;
    return false;
}

bool Online_Clients::online_user_parallel_insertion(
    const std::string& id, 
    const std::string& email, 
    std::shared_ptr<TCP_Connection> connection) 
{
    bool success = online_user_id_inserter(id, connection);
    success &= online_user_email_inserter(email, connection);
    return success; 
}

bool Online_Clients::online_user_parallel_keys_verification(const std::string& id, const std::string& email) {
    bool success = if_user_id_exist(id);
    success &= if_user_email_exist(email);
    return success;
}

bool Online_Clients::log_in_user(const std::string& id, const std::string& email, std::shared_ptr<TCP_Connection> connection) {
    return online_user_parallel_insertion(id, email, connection);
}

bool Online_Clients::online_user_id_inserter(const std::string& id, std::shared_ptr<TCP_Connection> connection) {
    if (!connection) {
        return false;
    }

    std::lock_guard<std::mutex> lock(onlineUsersId_mtx_);
   
    if (auto it = onlineUsersId_.find(id); it != onlineUsersId_.end()) {
        return it->second->set_connection(connection);
    }

    return false;
}

bool Online_Clients::online_user_email_inserter(const std::string& email, std::shared_ptr<TCP_Connection> connection) {
    if (!connection) {
        return false;
    }

    std::lock_guard<std::mutex> lock(onlineUsersEmail_mtx_);
   
    if (auto it = onlineUsersEmail_.find(email); it != onlineUsersEmail_.end()) {
        return it->second->set_connection(connection);
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
    auto result = (keyFormat) ? online_user_by_email_getter(key)
        : online_user_by_id_getter(key);

    return std::visit([](auto&& arg) -> std::shared_ptr<Client> {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            return std::shared_ptr<Client>{};  
        }
        else {
            return arg;
        }
        }, result);
	
}

bool Online_Clients::is_email(const std::string& key) {
	return key.find('@') != std::string::npos &&
		   key.find('.') != std::string::npos &&
		   key.find('@') < key.find('.');
}

bool Online_Clients::insert_user_verification(const std::string& id, const std::string& email) {
    if (id.empty() || email.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(usersPassVerifi_mtx_);
    auto it = usersPasswordVerification_.find(id);

    if (it == usersPasswordVerification_.end()) {
        usersPasswordVerification_[id] = email;
        return true;
    }
    return false; 
}

std::variant<bool, std::string> Online_Clients::get_email_by_id(const std::string& id) const {
    if (id.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(usersPassVerifi_mtx_);
    auto it = usersPasswordVerification_.find(id);

    if (it != usersPasswordVerification_.end()) {
        return it->second;
    }
    return false;
}

bool Online_Clients::verify_user_credentials(const std::string& id, const std::string& email) const {
    if (id.empty() || email.empty()) {
        return false;
    }

    std::lock_guard<std::mutex> lock(usersPassVerifi_mtx_);
    auto it = usersPasswordVerification_.find(id);

    if (it != usersPasswordVerification_.end()) {
        return (it->second == email);
    }
    return false;
}

bool Online_Clients::new_user_parallel_insertion(const std::string& id, const std::string& email, std::shared_ptr<Client> user) {
    if (online_user_parallel_keys_verification(id, email)) {
        return false;
    }

    return new_user_id_insertion(id, user) && new_user_email_insertion(email, user);
}

bool Online_Clients::new_user_id_insertion(const std::string& id, std::shared_ptr<Client> user) {
    if (id.empty() || !user) {
        return false;
    }
    std::lock_guard<std::mutex> lock(onlineUsersId_mtx_);
    if (auto it = onlineUsersId_.find(id); it == onlineUsersId_.end()) {
        onlineUsersId_[id] = user;
        return true;
    }
    return false;
}

bool Online_Clients::new_user_email_insertion(const std::string& email, std::shared_ptr<Client> user) {
    if (email.empty() || !user) {
        return false;
    }
    std::lock_guard<std::mutex> lock(onlineUsersEmail_mtx_);
    if (auto it = onlineUsersEmail_.find(email); it == onlineUsersEmail_.end()) {
        onlineUsersEmail_[email] = user;
        return true;
    }
    return false;
}

