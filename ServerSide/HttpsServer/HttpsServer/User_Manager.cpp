#include "User_Manager.h"

User_Manager::User_Manager(TCP_Server& server) : server_(server) {

}

User_Manager::~User_Manager() = default; 

void User_Manager::master_entrance_handeler(TCP_Connection& connection) {
    while (current_state_ != PromptState::EXIT) {
        std::lock_guard<std::mutex> lock(userResponse_mtx_);

        if (current_state_ == PromptState::INVALID_INPUT) {
            handlerPrompt_ = invalidArgument_;
        }
        
        handlerPrompt_ += startingPrompt_;

        connection.do_write(handlerPrompt_);
        userResponse_ = connection.read_from_user();

        if (userResponse_ == "1") {
            user_sign_up(connection);
            current_state_ = PromptState::EXIT;
        }
        else if (userResponse_ == "2") {
            user_log_in(connection);
            current_state_ = PromptState::EXIT;
        }
        else if (userResponse_ == "Exit++") {
            current_state_ = PromptState::EXIT;
            connection.stop_process();
        }
        else {
            current_state_ = PromptState::INVALID_INPUT;
        }

        handlerPrompt_.clear();
        userResponse_.clear();
    }
}

bool User_Manager::user_sign_up(TCP_Connection& connection) {
    try {
        std::lock_guard<std::mutex> lock(userResponse_mtx_);
        connection.do_write(userNamePrompt_);
        userResponse_.clear();
        userResponse_ = connection.read_from_user();
        userName_ = sanitizer_(userResponse_);
        std::unique_ptr<Client> newClient = std::make_unique<Registered_Client>(connection, userName_);
        if (ClientList.add_new_user(std::move(newClient))) {
            userId_ = server_.client_id_generator(newClient);
        }
        newClient->write_client_id(userId_);
        ClientList.insert_registered_client(userId_, newClient);
        user_log_in(userId_);
        return true;
    }
    catch (const std::runtime_error& e) {
        console.log("Sign up process failed! ", e.what());
    }
}

bool User_Manager::user_log_in(TCP_Connection& connection) {

	console.log("Loging in");
	return true; 
}
