#pragma once
#include "Client.h"
#include "Online_Client.h"
#include "TCP_Connection.h"
#include <string>
#include <mutex>
#include <thread>
#include <atomic>


class User_Manager
{
public:
	~User_Manager();
	void signing_prompt(TCP_Connection& connection, 
		std::unique_lock<std::mutex> existing_lock = std::unique_lock<std::mutex>());
	bool user_log_in(TCP_Connection& connection);
	bool user_sign_up(TCP_Connection& connection);
	std::string message;
private:
	TCP_Connection& connection_; 
	void master_entrance_handeler(const std::string& userResponse);
	void invalid_argument_prompt();
	std::string startingPrompt_ = "To Sign Up as new client type '1'. \n"
		"To Log In into your account type '2'. \n"
		"To exit the app please type 'Exit++' \n";
	std::string userResponse_;
	std::string invalidArgument_{"Invalid Argument. Please enter valid arguments! \n"};
	std::string handlerPrompt_; 
	std::mutex signingPrompt_mtx_;
};

