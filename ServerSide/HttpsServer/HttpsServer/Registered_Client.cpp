#include "Registered_Client.h"

Registered_Client::Registered_Client(TCP_Connection& connection, std::string name) :
	Client(connection), name_(name) 
{
	clientHasId.store(false, std::memory_order_relaxed);
}

Registered_Client::~Registered_Client() = default;

std::string Registered_Client::get_client_id() const {
	return clientId_;
}

std::string Registered_Client::get_client_name() const {
	return name_;
}

void Registered_Client::write_client_id(const std::string id) {
	clientId_ = id;
}