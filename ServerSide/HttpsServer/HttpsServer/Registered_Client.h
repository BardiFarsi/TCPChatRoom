#pragma once
#include "Client.h"
#include "TCP_Connection.h"
#include "Prompt.h"
#include <string>

class Registered_Client : public Client
{
public:
	std::atomic<bool> clientHasId;
	Registered_Client(std::shared_ptr<TCP_Connection> connection, std::string name, std::string email);
	~Registered_Client();
	std::string get_client_id() const override;
	std::string get_client_name() const override;
	std::string get_client_email() const override;
	void write_client_id(const std::string& id) override;
	void write_client_name(const std::string& name) override;

protected:
	std::string clientId_;
	std::string name_;
};

