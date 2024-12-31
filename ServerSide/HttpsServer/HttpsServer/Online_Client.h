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


class Online_Client : public Client
{
public:
	Online_Client(std::string userName, TCP_Server& server, TCP_Connection& connection);
	~Online_Client();
	std::string generate_id() override;
	std::string getter_user_name() const;
	std::string getter_client_id() const;
	std::atomic<bool> clientHasId; 
private: 
	std::string userName_;
	std::string clientId_; 
	std::vector<std::string> userVariables_; 
	std::unordered_map<std::string, std::string> userDataDictionary_;

};

