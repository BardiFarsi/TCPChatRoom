
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
