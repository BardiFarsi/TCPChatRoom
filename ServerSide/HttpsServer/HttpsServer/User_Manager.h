#pragma once
#include "Master_Server.h"
#include "Client.h"
#include "Online_Clients.h"
#include "Registered_Client.h"
#include "All_Clients.h"
#include "TCP_Connection.h"
#include "Message.h"
#include "Prompt.h"
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
	void handle_user_response(const std::string& userResponse, const std::shared_ptr<TCP_Connection> conncetion);

private:
	enum class PromptState {
		INITIAL,
		INVALID_INPUT,
		CONNECTION_FAILED,
		OPTION_SELECTED,
		EXIT
	};
	enum class ServiceState {
		INITIAL,
		INVALID_INPUT,
		CONNECTION_FAILED,
		SERVICE_SELECTED,
		EXIT
	};
	std::variant<bool, std::string> user_query_prompt_(std::string prompt, std::shared_ptr<TCP_Connection> connection);
	void connection_stop_handler(std::shared_ptr<TCP_Connection> connection);
	void prompt_which_main_service(std::shared_ptr<Client> client);
	void process_selected_main_service(const std::string& response, std::shared_ptr<Client> client);
	void add_partner_to_chat(std::shared_ptr<Client> client);
	bool user_log_in(std::shared_ptr<TCP_Connection> connection);
	bool user_sign_up(std::shared_ptr<TCP_Connection> connection);
	bool catch_handler(std::shared_ptr<TCP_Connection> connection);
	static constexpr size_t TIMESTAMP_LENGTH = 10;
	static constexpr size_t RANDOM_LENGTH = 6;
	static constexpr size_t PREFIX_LENGTH = 3;
	std::string create_registration_announcement(const std::string& userId);
	std::string client_id_generator();
	std::string create_new_id();
	Buffer_Sanitizer sanitizer_; 
	Master_Server& masterServer_;
	std::random_device rd;
	std::mt19937_64 gen;
	PromptState current_state_ = PromptState::INITIAL;
	ServiceState service_state_ = ServiceState::INITIAL;
};
