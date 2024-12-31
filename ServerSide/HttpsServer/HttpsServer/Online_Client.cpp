#include "Online_Client.h"

Online_Client::Online_Client(std::string userName, TCP_Server& server, TCP_Connection& connection) : 
	Client(server, connection), userName_(userName) 
{
	userVariables_.push_back("User Name: ");
	userVariables_.push_back("Client ID: ");
	clientHasId.store(false, std::memory_order_relaxed);
	clientId_ = generate_id(); 
}

Online_Client::~Online_Client() = default;

std::string Online_Client::generate_id(){
	if (!clientHasId.load(std::memory_order_relaxed)) {
		clientHasId.store(true, std::memory_order_relaxed);
		std::string id = server_.client_id_generator(*this);
		console.log("The client ID: ", clientId_, " generated for the user: ", userName_);
		return id; 
	}
	else {
		console.log("Warning the client already has an ID");
		console.log("The Client ID is: ", clientId_);
		throw std::runtime_error("Error!. The client already has an ID!");
	}
}

std::string Online_Client::getter_user_name() const {
	return userName_;
}
std::string Online_Client::getter_client_id() const {
	return clientId_;
}