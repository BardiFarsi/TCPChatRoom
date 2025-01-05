
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
	std::variant<bool, std::shared_ptr<Client>> online_user_getter(const std::string& id) const;
	bool online_user_inserter(const std::string& id, std::shared_ptr<Client> user);
	bool if_client_exist(const std::string& id) const;
	bool remove_online_user(const std::string& id);
private: 
	std::unordered_map<std::string, std::shared_ptr<Client>> onlineUsers_;
	std::string clientId_; 
	mutable std::mutex onlineUsers_mtx_;
};
