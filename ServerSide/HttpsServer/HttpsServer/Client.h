#pragma once
#include <string>

class TCP_Connection;

class Client {
public:
	Client(TCP_Connection& connection);
	virtual ~Client() = 0;
	virtual std::string get_client_id() const = 0;
	virtual std::string get_client_name() const = 0;
	virtual void write_client_id(const std::string id) = 0;
protected:
	TCP_Connection& connection_;
};