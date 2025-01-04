#pragma once
#include "Master_Server.h"
#include "Client.h"
#include "Online_Clients.h"
#include "Registered_Client.h"
#include "All_Clients.h"
#include "TCP_Connection.h"
#include "Message.h"
#include <string>
#include <mutex>
#include <memory>
#include <ranges>
#include <random>
#include <format>
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <variant>
#include <type_traits>
#include <boost/asio.hpp>

using error_code = boost::system::error_code;

class Master_Server;

class User_Manager
{
public:
	User_Manager(Master_Server& masterServer);
	~User_Manager();
	void master_entrance_handeler(std::shared_ptr<TCP_Connection> connection);
	void handle_user_response(const std::shared_ptr<TCP_Connection> conncetion);

private:
	enum class PromptState {
		INITIAL,
		INVALID_INPUT,
		Connection_Failed,
		EXIT
	};
	std::variant<bool, std::string> user_query_prompt_(std::string prompt, std::shared_ptr<TCP_Connection> connection);
	void connection_stop_handler(std::shared_ptr<TCP_Connection> connection);
	bool user_log_in(std::shared_ptr<TCP_Connection> connection);
	bool user_sign_up(std::shared_ptr<TCP_Connection> connection);
	bool catch_handler(std::shared_ptr<TCP_Connection> connection);
	static constexpr size_t TIMESTAMP_LENGTH = 10;
	static constexpr size_t RANDOM_LENGTH = 6;
	static constexpr size_t PREFIX_LENGTH = 3;
	std::string create_registration_announcement(const std::string& userId);
	std::string client_id_generator();
	std::string create_new_id();
	PromptState current_state_ = PromptState::INITIAL;
	Buffer_Sanitizer sanitizer_; 
	Master_Server& masterServer_;
	const std::string readError_{ "Read Error!" };
	const std::string readExit_{ "Exit++" };
	std::string startingPrompt_ = "To Sign Up as new client type '1'. \n"
		"To Log In into your account type '2'. \n"
		"To exit the app please type 'Exit++' \n";
	std::string userNameLogInPrompt_{ 
		"To Log In please type your Name below or Exit++ to close the connection: " };
	std::string userIdLogInPrompt_{ "Please type your User ID below or Exit++ to close the connection: " };
	std::string invalidArgument_{"Invalid Argument. Please enter valid arguments! \n"};
	std::string userNamePrompt_{"What is your name? "};
	std::string userIdMessageCreated_{"Your User ID is: "};
	std::string successLogIn_{"Client Successfully Logged In"};
	std::string successLogInClientSide_{ "Loged In Successfully" };
	std::string userName_;
	std::string userId_;
	std::string userResponse_;
	std::string handlerPrompt_; 
	std::mutex userResponse_mtx_;
	std::random_device rd;
	std::mt19937_64 gen;
};
