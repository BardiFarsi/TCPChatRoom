#include "User_Manager.h"

User_Manager::~User_Manager() = default; 

void User_Manager::signing_prompt(TCP_Connection& connection, 
	std::unique_lock<std::mutex> existing_lock) { 
	
	std::unique_lock<std::mutex> lock = existing_lock.owns_lock() ?
		std::move(existing_lock) : std::unique_lock<std::mutex>(signingPrompt_mtx_);

	handlerPrompt_ += startingPrompt_;
	connection.do_write(handlerPrompt_);
	userResponse_ = connection.read_from_user();
	master_entrance_handeler(userResponse_);
	userResponse_.clear();
	handlerPrompt_.clear();
}

bool User_Manager::user_sign_up(TCP_Connection& connection) {
	console.log("Signing up");
	return true;
}

bool User_Manager::user_log_in(TCP_Connection& connection) {

	console.log("Loging in");
	return true; 
}

void User_Manager::master_entrance_handeler(const std::string& userResponse) {
	if (userResponse == "1") {
		user_sign_up(connection_);
	}
	else if (userResponse == "2") {
		user_log_in(connection_);
	}
	else if (userResponse == "Exit++") {
		connection_.stop_process(); 
	}
	else {
		invalid_argument_prompt();
	}
}

void User_Manager::invalid_argument_prompt() {
	std::unique_lock<std::mutex> lock(signingPrompt_mtx_);
	handlerPrompt_.clear();
	handlerPrompt_ += invalidArgument_;
	signing_prompt(connection_, std::move(lock));
}