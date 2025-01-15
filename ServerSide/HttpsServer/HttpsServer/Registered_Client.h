#pragma once
#include "Client.h"
#include "TCP_Connection.h"
#include "Prompt.h"
#include "Pair_Chat_Session.h"
#include "Buffer_Sanitizer.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

using error_code = boost::system::error_code;

class Pair_Chat_Session;

class Registered_Client : public Client
{
public:
	std::atomic<bool> clientHasId;
	Registered_Client(std::shared_ptr<TCP_Connection> connection, std::string clientId, std::string email);
	~Registered_Client();
	std::string get_client_id() const override;
	std::string get_client_name() const override;
	std::string get_client_email() const override;
	std::shared_ptr<TCP_Connection> get_connection() const override;
    bool set_connection(std::shared_ptr<TCP_Connection> connection) override;
	void write_client_id(const std::string& id) override;
	void write_client_name(const std::string& name) override;
	bool add_friend(const std::string& email) override;
	bool remove_friend(const std::string& email);
	bool is_friend_exist(const std::string& email);

protected:
	std::unordered_map<std::string, std::shared_ptr<Client>> friendList_;
	std::shared_ptr<Pair_Chat_Session> pairChat_;
	std::mutex friendList_mtx_;
	std::string clientId_;
	std::string name_;
};

