
#if 0
#pragma once
#include "Client.h"
#include "LOGGER.h"
#include "TCP_Server.h"
#include "TCP_Connection.h"
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <mutex>
#include <variant>
#include <tuple>

class Online_Clients
{
public:
	using clientVariant = std::variant<std::tuple<std::string, std::weak_ptr<Client>>, bool>;
	Online_Clients();
	~Online_Clients();
	std::string getter_user_name() const;
	std::string getter_client_id() const;
	bool add_online_clients(std::string id); 
	clientVariant get_online_client(const std::string id);
private: 
	std::unordered_map<std::string, std::shared_ptr<Client>> onlineUsers_;
	std::string userName_;
	std::string clientId_; 
	std::mutex onlineUsers_mtx_;
};

#endif