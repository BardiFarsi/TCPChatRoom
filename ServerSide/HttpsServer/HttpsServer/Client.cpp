#include "Client.h"

Client::Client(std::shared_ptr<TCP_Connection> connection, std::string email): 
	connection_(std::move(connection)),
	email_(email) {}  

Client::~Client() = default; 
