#pragma once
#include <string>
#include <memory>

class TCP_Connection;

class Client {
public:
	Client(std::shared_ptr<TCP_Connection> connection);
	virtual ~Client();
	// virtual bool set_connection(const std::shared_ptr<TCP_Connection>& connection) = 0;
	// virtual std::shared_ptr<TCP_Connection> get_connection() const = 0;
	virtual void write_client_id(const std::string& id) = 0;
	virtual void write_client_name(const std::string& name) = 0; 
	virtual std::string get_client_id() const = 0;
	virtual std::string get_client_name() const = 0;
	std::shared_ptr<TCP_Connection> connection_;
};