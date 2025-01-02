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
	std::string get_client_id() const;
	std::string get_client_name() const;
	void write_client_id(const std::string& id) override;
	//bool set_connection(const std::shared_ptr<TCP_Connection>& connection) override;
	//std::shared_ptr<TCP_Connection> get_connection() const override;
private:
	std::string clientId_;
	std::string name_; 
};

