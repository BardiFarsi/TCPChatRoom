#pragma once
#include "Client.h"
#include "LOGGER.h"
#include "TCP_Server.h"
#include "TCP_Connection.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <atomic>
#include <stdexcept>

class Online_Clients
{
public:
	Online_Clients();
	~Online_Clients();
	std::string getter_user_name() const;
	std::string getter_client_id() const;
	bool add_online_clients(std::string id); 
	std::atomic<bool> clientIsOnline; 
private: 
	std::string userName_;
	std::string clientId_; 
	std::vector<std::string> userVariables_; 
	std::unordered_map<std::string, std::unique_ptr<Client>> onlineUsers_;

};

