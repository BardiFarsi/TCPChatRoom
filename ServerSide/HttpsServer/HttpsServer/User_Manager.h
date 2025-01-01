#pragma once
#include "Client.h"
#include "Online_Clients.h"
#include "Registered_Client.h"
#include "All_Clients.h"
#include "TCP_Connection.h"
#include "TCP_Server.h"
#include <string>
#include <mutex>
#include <memory>

class TCP_Server;


class User_Manager
{
public:
	User_Manager(TCP_Server& server);
	~User_Manager();
	bool user_log_in(TCP_Connection& connection);
	bool user_sign_up(TCP_Connection& connection);
private:
	enum class PromptState {
		INITIAL,
		INVALID_INPUT,
		EXIT
	};
	PromptState current_state_ = PromptState::INITIAL;
	Buffer_Sanitizer sanitizer_; 
	TCP_Server& server_;
	void master_entrance_handeler(TCP_Connection& connection);
	std::string startingPrompt_ = "To Sign Up as new client type '1'. \n"
		"To Log In into your account type '2'. \n"
		"To exit the app please type 'Exit++' \n";
	std::string invalidArgument_{"Invalid Argument. Please enter valid arguments! \n"};
	std::string userNamePrompt_{"What is your name? "};
	std::string userName_;
	std::string userId_;
	std::string userResponse_;
	std::string handlerPrompt_; 
	std::mutex userResponse_mtx_;
};

