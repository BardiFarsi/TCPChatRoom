#pragma once
#include <string>

class TCP_Server;
class TCP_Connection;

class Client {
public:
	Client(TCP_Server& server, TCP_Connection& connection);
	virtual ~Client() = 0;
	virtual std::string generate_id() = 0; 
protected:
	TCP_Server& server_;
	TCP_Connection& connection_;
};