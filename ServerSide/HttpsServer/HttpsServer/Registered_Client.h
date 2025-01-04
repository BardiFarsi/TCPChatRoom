#pragma once
#include "Client.h"
#include "TCP_Connection.h"
#include <string>

class Registered_Client : public Client
{
public:
	std::atomic<bool> clientHasId;
	Registered_Client(std::shared_ptr<TCP_Connection> connection, std::string name);
	~Registered_Client();
	std::string get_client_id() const override;
	std::string get_client_name() const override;
	void write_client_id(const std::string& id) override;
	void write_client_name(const std::string& name) override;

protected:
	std::string clientId_;
	std::string name_; 
};

