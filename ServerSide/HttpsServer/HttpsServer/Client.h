#pragma once
#include <string>
#include <memory>

class TCP_Connection;
class Pair_Chat_Session;

class Client : public std::enable_shared_from_this<Client>
{
public:
	Client(std::shared_ptr<TCP_Connection> connection, std::string email);
	virtual ~Client();
	virtual void write_client_id(const std::string& id) = 0;
	virtual void write_client_name(const std::string& name) = 0; 
	virtual std::shared_ptr<TCP_Connection> get_connection() const = 0;
	virtual bool set_connection(std::shared_ptr<TCP_Connection> connection) = 0;
	virtual std::string get_client_id() const = 0;
	virtual std::string get_client_name() const = 0;
	virtual std::string get_client_email() const = 0; 
    virtual bool add_friend(const std::string& email) = 0;
	std::shared_ptr<TCP_Connection> connection_; // connection should be protected
protected:
	std::string email_;
};