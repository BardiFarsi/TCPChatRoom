#include "User_Manager.h"

User_Manager::User_Manager(Master_Server& masterServer) 
    : masterServer_(masterServer) {}

User_Manager::~User_Manager() = default;

void User_Manager::master_entrance_handeler(std::shared_ptr<TCP_Connection> connection) {
    if (!connection) {
        console.log("Invalid connection pointer");
        PromptState::CONNECTION_FAILED;
        return;
    }
    
    try {
        error_code ec;
        std::string userResponse;
        std::string handlerPrompt;
        while (current_state_ != PromptState::EXIT && !ec) {
            handlerPrompt.clear();
            userResponse.clear();

            if (current_state_ == PromptState::INVALID_INPUT) {
                handlerPrompt = g_invalidArgument;
            }

            handlerPrompt += g_startingPrompt;

            if (connection->running_.load(std::memory_order_consume)) {
                connection->do_prompt_user(handlerPrompt);
            }

            if (connection->running_.load(std::memory_order_consume)) {
                
                userResponse = connection->read_from_user();
            }

            if (userResponse != g_readError) {
                handle_user_response(userResponse, connection);
                break;
            }
            else {
                console.log("Error in read handling");
            }
        }
    }
    catch (...) {
        console.log("GateWay Error! ");
        catch_handler(connection);
        connection->stop_process();
    }
}

void User_Manager::handle_user_response(const std::string& userResponse, const std::shared_ptr<TCP_Connection> conncetion) {
    error_code ec;
   
    if (!ec) {
        if (userResponse == "1") {
            user_sign_up(conncetion);
            current_state_ = PromptState::OPTION_SELECTED;
        }
        else if (userResponse == "2") {
            user_log_in(conncetion);
            current_state_ = PromptState::OPTION_SELECTED;
        }
        else if (userResponse == "Exit++") {
            current_state_ = PromptState::EXIT;
            conncetion->stop_process();
        }
        else {
            current_state_ = PromptState::INVALID_INPUT;
            master_entrance_handeler(conncetion);
        }
    }
    current_state_ = PromptState::CONNECTION_FAILED;
}

bool User_Manager::user_sign_up(std::shared_ptr<TCP_Connection> connection) {
    if (!connection) {
        console.log("Invalid connection pointer in sign up process");
        return false;
    }

    try {
        error_code ec;
        std::string localEmail;
        std::string localId;
        std::string localName;
        bool success{ true };

        auto userEmailValidation = user_query_prompt_(g_emailRegistPrompt, connection);
        std::visit(
            [&](const auto& result) {
                using T = std::decay_t<decltype(result)>;
                if constexpr (std::is_same_v<std::string, T>) {
                    localEmail.clear();
                    localEmail = result;
                    success &= !ClientList.if_client_exist(localEmail);
                    success &= !OnlineUsers.if_user_email_exist(localEmail);
                    if (!success) {
                        connection->do_prompt_user(g_emailAddressExist);
                        master_entrance_handeler(std::move(connection));
                    }
                }
                else {
                    connection_stop_handler(connection);
                    success &= result;
                }
            },
            userEmailValidation
        );
        if (!success) return success;

        auto userNameValidation = user_query_prompt_(g_userNamePrompt, connection);
        std::visit(
            [&](const auto& result) {
                using T = std::decay_t<decltype(result)>;
                if constexpr (std::is_same_v<std::string, T>) {
                    localName = result;
                }
                else {
                    connection_stop_handler(connection);
                    success = result;
                }
            },
            userNameValidation
        );
        if (!success) return success;

        localId = client_id_generator();
        std::string userIdMessage = g_userIdMessageCreated + localId;

        std::shared_ptr<Client> newClient = std::make_shared<Registered_Client>(connection, localId, localEmail);

        if (!ClientList.add_new_user(newClient)) {
            if (!ec && connection->running_.load(std::memory_order_acquire)) {
                connection->do_prompt_user("Client already exists or registration failed. (×_×)");
            }
            console.log("Registration failed: Could not add new user to client list");
            return false;
        }

        if (!ec) {
            newClient->write_client_id(userIdMessage);
        }

        bool loginSuccess = OnlineUsers.new_user_parallel_insertion(localId, localEmail, newClient);
        bool verificationSuccess = OnlineUsers.insert_user_verification(localId, localEmail);

        if (!loginSuccess || !verificationSuccess) {
            if (!loginSuccess) {
                console.log("Registration failed: Could not log in user");
            }
            if (!verificationSuccess) {
                console.log("Registration failed: Could not set user verification");
            }

            if (!ec && connection->running_.load(std::memory_order_acquire)) {
                connection->do_prompt_user(
                    "Oops! An error occurred during registration. Please contact us for further details. (×_×)");
                connection->running_.store(false, std::memory_order_acquire);
            }
            return false;
        }

        console.log("Client Successfully Registered");
        if (!ec && connection->running_.load(std::memory_order_acquire)) {
            newClient->get_connection()->do_prompt_user(create_registration_announcement(localId));
        }

        prompt_which_main_service(newClient);
        return true;
    }
    catch (...) {
        return catch_handler(connection);
    }
}

bool User_Manager::user_log_in(std::shared_ptr<TCP_Connection> connection) {
    if (!connection) {
        console.log("Invalid connection pointer in sign up process");
        return false;
    }

    try {
        error_code ec;
        std::string localEmail;
        std::string localId;
        bool logging = true;

        while (logging && !ec) {
            auto userNameValidation = user_query_prompt_(g_userEmailLoginPrompt, connection);
            std::visit(
                [&](const auto& result) {
                    using T = std::decay_t<decltype(result)>;
                    if constexpr (std::is_same_v<std::string, T>) {
                        localEmail.clear();
                        localEmail = result;
                        console.log(localEmail);
                    }
                    else {
                        connection_stop_handler(connection);
                        logging = result;
                    }
                },
                userNameValidation
            );

            if (!logging) break;

            auto userIdValidation = user_query_prompt_(g_userIdLoginPrompt, connection);
            std::visit(
                [&](const auto& result) {
                    using T = std::decay_t<decltype(result)>;
                    if constexpr (std::is_same_v<std::string, T>) {
                        localId.clear();
                        localId = result;
                        console.log(localId);
                    }
                    else {
                        connection_stop_handler(connection);
                        logging = result;
                    }
                },
                userIdValidation
            );

            if (!logging) break;

            if (!ec) {
                if (OnlineUsers.verify_user_credentials(localId, localEmail)) {
                    if (OnlineUsers.log_in_user(localId, localEmail, connection)) {
                        auto userResult = OnlineUsers.online_user_by_id_getter(localId);
                        bool loginSuccess = false;  

                        std::visit([&](const auto& result) {
                            using T = std::decay_t<decltype(result)>;
                            if constexpr (std::is_same_v<T, std::shared_ptr<Client>>) {
                                result->get_connection()->do_prompt_user(g_successLogInClientSide);
                                console.log(g_successLogIn);
                                prompt_which_main_service(result);
                                loginSuccess = true;
                            }
                            else {
                                console.log("Failed to get client after login");
                                if (!ec) {
                                    connection->do_prompt_user("Login error occurred");
                                }
                            }
                            }, userResult);

                        if (loginSuccess) {
                            return true;  
                        }
                    }
                    else {
                        if (!ec) {
                            connection->do_prompt_user(g_errorUnableToLoginUser);
                        }
                        throw std::runtime_error("Unknown Error! User Email and ID do not login");
                    }
                }
                else {
                    if (!ec) {
                        connection->do_prompt_user("Invalid ID or Email. Please try again!");
                    }
                    master_entrance_handeler(connection);
                    return false;
                }
            }

            if (!ec && connection->running_.load(std::memory_order_acquire)) {
                connection->do_prompt_user(
                    "Oops! Invalid User Name or User ID. Please try again. (×_×)");
                current_state_ = PromptState::INVALID_INPUT;
            }
        }

        return false;  
    }
    catch (...) {
        return catch_handler(connection);
    }
}

std::string User_Manager::user_prompter(const std::string& message, std::shared_ptr<Client> user) {
    error_code ec;
    if (!ec && user->get_connection()->running_.load(std::memory_order_consume)) {
        user->get_connection()->do_prompt_user(message);
    }

    std::string userResponse;
    std::string handlerPrompt;

    try {
        while (!ec && service_state_ != ServiceState::EXIT) {
            handlerPrompt.clear();
            userResponse.clear();

            if (user->get_connection()->running_.load(std::memory_order_consume)) {
                userResponse = user->get_connection()->read_from_user();
            }

            if (userResponse != g_readError) {
                service_state_ == ServiceState::RESPONSE_SENT;
                return userResponse;
            }

            if (service_state_ == ServiceState::INVALID_INPUT || current_state_ == PromptState::INVALID_INPUT) {
                handlerPrompt = g_invalidArgument;
            }

            handlerPrompt += message;

            if (user->get_connection()->running_.load(std::memory_order_consume)) {
                user->get_connection()->do_prompt_user(handlerPrompt);
            }
        }
        service_state_ == ServiceState::CONNECTION_FAILED;
        console.log(g_readError);
        return g_readError;
    }
    catch (...) {
        console.log("Service GateWay Error! ");
        catch_handler(user->get_connection());
        user->get_connection()->stop_process();
    }
}

void User_Manager::prompt_which_main_service(std::shared_ptr<Client> client) {
    error_code ec;

    std::string userResponse = user_prompter(g_whichServiceMainPropmt, client);
    if (userResponse != g_readError) {
        process_selected_main_service(userResponse, client);
    }
    else {
        console.log("Error in read handling");
        service_state_ == ServiceState::CONNECTION_FAILED;
        throw std::runtime_error(g_errorConnection);
    }
}

void User_Manager::process_selected_main_service(const std::string& response, std::shared_ptr<Client> user) {
    error_code ec;

    if (!ec) {
        if (response == "1") {
            add_partner_to_chat(user);
            current_state_ = PromptState::OPTION_SELECTED;
        }
        else if (response == "2") {
            console.log("join_group_chat(client)");
            current_state_ = PromptState::OPTION_SELECTED;
        }
        else if (response == "3") {
            console.log("join_broadcast_server(client);"); 
            current_state_ = PromptState::OPTION_SELECTED;
        }
        else if (response == "Exit++") {
            console.log("Client is going to exit!"); 
            current_state_ = PromptState::EXIT;
            user->connection_->stop_process();
        }
        else {
            current_state_ = PromptState::INVALID_INPUT;
            prompt_which_main_service(user);
        }
    }
    current_state_ = PromptState::CONNECTION_FAILED;
}

void User_Manager::add_partner_to_chat(std::shared_ptr<Client> user) {
    error_code ec;
    std::string userResponse = user_prompter(g_addPartnerToChatPrompt, user);

    if (userResponse != g_readError) {
        process_chat_request_prompt(userResponse, user);
    }
    else {
        console.log("Error in read handling");
        service_state_ == ServiceState::CONNECTION_FAILED;
        throw std::runtime_error(g_errorConnection);
    }
}

void User_Manager::process_chat_request_prompt(const std::string& response, std::shared_ptr<Client> user) {
    error_code ec;

    if (!ec) {
        if (response == "1") {
            add_new_friend(user);
            current_state_ = PromptState::OPTION_SELECTED;
        }
        else if (response == "2") {
            create_pair_chat(user);
            current_state_ = PromptState::OPTION_SELECTED;
        }
        else if (response == "3") {
            prompt_which_main_service(user);
            current_state_ = PromptState::OPTION_SELECTED;
        }
        else if (response == "Exit++") {
            current_state_ = PromptState::EXIT;
            user->get_connection()->stop_process();
        }
        else {
            current_state_ = PromptState::INVALID_INPUT;
            add_partner_to_chat(user);
        }
    }
    current_state_ = PromptState::CONNECTION_FAILED;
    throw std::runtime_error(g_errorConnection);
}

void User_Manager::add_new_friend(std::shared_ptr<Client> user) {
    error_code ec;
    std::string userResponse = user_prompter(g_addNewFriendMainPrompt, user);

    if (userResponse != g_readError) {
        add_new_friend_handler(userResponse, user);
    }
    else {
        console.log("Error in read handling");
        service_state_ == ServiceState::CONNECTION_FAILED;
        throw std::runtime_error(g_errorConnection);
    }
    

}

void User_Manager::add_new_friend_handler(const std::string& response, std::shared_ptr<Client> user) {
    error_code ec;
    bool success = user->add_friend(response);

    if (success) {
        if (!ec && user->get_connection()->running_.load(std::memory_order_consume)) {
            user->get_connection()->do_prompt_user("Friend successfully added. (ᵔᴥᵔ)");
        }
        add_partner_to_chat(user);
    }
    else {
        if (!ec && user->get_connection()->running_.load(std::memory_order_consume)) {
            user->get_connection()->do_prompt_user("User not found. Please try again! (◕︵◕)");
        }
        add_partner_to_chat(user);
    }
}

std::variant<bool, std::string> User_Manager::user_query_prompt_(std::string prompt, std::shared_ptr<TCP_Connection> connection) {
    error_code ec;
    try {
        std::string localResponse;
        if (!ec && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user(prompt);
            localResponse = connection->read_from_user();
        }

        if (localResponse != g_readError && localResponse != g_readExit && localResponse != "" && localResponse != "\n") 
        {
            std::string prompt = sanitizer_(localResponse);
            console.log(g_defaultConsoleUserResponse, prompt);
            return prompt;
        }

        current_state_ = PromptState::CONNECTION_FAILED;
        connection->running_.store(false, std::memory_order_release);
        connection->stop_process();
        return false;
    }
    catch (...) {
        return catch_handler(connection);
    }
}

void User_Manager::connection_stop_handler(std::shared_ptr<TCP_Connection> connection) {
    console.log("Error in read handling or client is leaving");
    current_state_ = PromptState::EXIT;
    connection->running_.store(false, std::memory_order_release);
    connection->stop_process();
}

bool User_Manager::catch_handler(std::shared_ptr<TCP_Connection> connection) {
    try {
        throw;  
    }
    catch (const boost::system::system_error& e) {
        console.log("Network error during sign up: ", e.what());
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("Network error occurred during registration. Please try again later.");
            connection->running_.store(false, std::memory_order_release);
        }
        current_state_ = PromptState::CONNECTION_FAILED;
        return false;
    }
    catch (const std::runtime_error& e) {
        console.log("Sign up process failed! ", e.what());
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("An unexpected error occurred during registration.");
            connection->running_.store(false, std::memory_order_release);
        }
        current_state_ = PromptState::CONNECTION_FAILED;
        return false;
    }
    catch (...) {
        console.log("Unknown error during sign up process");
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("An unexpected error occurred during registration.");
            connection->running_.store(false, std::memory_order_release);
        }
        current_state_ = PromptState::CONNECTION_FAILED;
        return false;
    }
}

std::string User_Manager::client_id_generator() {
    std::string id;
    while (true) {
        id.clear();
        id = create_new_id();
        if (!ClientList.is_id_taken(id))
            return id;
    }
}

std::string User_Manager::create_new_id() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();

    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFF);
    uint32_t random_component = dist(gen);

    return std::format("uid{:0{}}:{:0{}x}",
        timestamp, TIMESTAMP_LENGTH,
        random_component, RANDOM_LENGTH);
}

std::string User_Manager::create_registration_announcement(const std::string& userId) {
    std::string announcement = "Congratulations! Your account has been successfully created. ^_^ \n";
    announcement += userId;
    announcement += "\n. Please keep your User ID safe to be able to reconnect to server again. ";
    return announcement;
}
