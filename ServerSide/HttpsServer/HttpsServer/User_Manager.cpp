#include "User_Manager.h"

User_Manager::User_Manager(Master_Server& masterServer) 
    : masterServer_(masterServer) {}

User_Manager::~User_Manager() = default;

void User_Manager::master_entrance_handeler(std::shared_ptr<TCP_Connection> connection) {
    if (!connection) {
        console.log("Invalid connection pointer");
        PromptState::Connection_Failed;
        return;
    }

    try {
        error_code ec;
        while (current_state_ != PromptState::EXIT && !ec) {
            std::lock_guard<std::mutex> lock(userResponse_mtx_);

            if (current_state_ == PromptState::INVALID_INPUT) {
                handlerPrompt_ = invalidArgument_;
            }

            handlerPrompt_ += startingPrompt_;

            if (connection->running_.load(std::memory_order_consume)) {
                connection->do_prompt_user(handlerPrompt_);
            }

            if (connection->running_.load(std::memory_order_consume)) {
                userResponse_ = connection->read_from_user();
            }

            if (userResponse_ != readError_) {
                handle_user_response(connection);
            }
            else {
                console.log("Error in read handling");
            }

            handlerPrompt_.clear();
            userResponse_.clear();
        }
    }
    catch (const std::exception& e) {
        console.log("GateWay Error! ", e.what());
        connection->stop_process();
    }
}

void User_Manager::handle_user_response(const std::shared_ptr<TCP_Connection> conncetion) {
    if (userResponse_ == "1") {
        user_sign_up(conncetion);
        current_state_ = PromptState::EXIT;
    }
    else if (userResponse_ == "2") {
        user_log_in(conncetion);
        current_state_ = PromptState::EXIT;
    }
    else if (userResponse_ == "Exit++") {
        current_state_ = PromptState::EXIT;
        conncetion->stop_process();
    }
    else {
        current_state_ = PromptState::INVALID_INPUT;
    }
}

bool User_Manager::user_sign_up(std::shared_ptr<TCP_Connection> connection) {
    if (!connection) {
        console.log("Invalid connection pointer in sign up process");
        return false;
    }

    try {
        error_code ec;
        bool success;

        auto userNameValidation = user_query_prompt_(userNamePrompt_, connection);
        std::visit(
            [&](const auto& result) {
                using T = std::decay_t<decltype(result)>;
                if constexpr (std::is_same_v<std::string, T>) {
                    userName_.clear();
                    userName_ = result;
                }
                else {
                    connection_stop_handler(connection);
                    success =  result;
                }
            }
            , userNameValidation
        );

        if (!success) return success;

        std::shared_ptr<Client> newClient = std::make_shared<Registered_Client>(connection, userName_);

        if (!ClientList.add_new_user(newClient)) {
            if (!ec && connection->running_.load(std::memory_order_acquire)) {
                connection->do_prompt_user("Client already exists or registration failed. (×_×)");
            }
            console.log("Connection failed during Sign Up process");
            return false;
        }

        userId_ = client_id_generator();
        userIdMessageCreated_ += userId_;

        if (!ec) {
            newClient->write_client_id(userIdMessageCreated_);
        }
      
        if (!ClientList.insert_registered_client(userId_, newClient)) {
            console.log("Failed to connect the client!");
            if (!ec && connection->running_.load(std::memory_order_acquire)) {
                connection->do_prompt_user(
                    "Oops! An error occurred during registration. Please contact us for further details. (×_×)");
                connection->running_.store(false, std::memory_order_acquire);
            }
            console.log("Connection failed during Sign Up process");
            return false;
        }
        
        console.log("Client Successfully Registered");

        if (!ec && connection->running_.load(std::memory_order_acquire)) {
            newClient->connection_->do_prompt_user(create_registration_announcement(userId_));
        }
        
        user_log_in(newClient->connection_);
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
        bool loopCondition = true;
        while (loopCondition && !ec) {
           
            auto userNameValidation = user_query_prompt_(userNameLogInPrompt_, connection);
            std::visit(
            [&](const auto& result) {
                    using T = std::decay_t<decltype(result)>;
                    if constexpr (std::is_same_v<std::string, T>) {
                        userName_.clear();
                        userName_ = result;
                        console.log(userName_);
                    }
                    else {
                        connection_stop_handler(connection);
                        loopCondition = result;
                    }
                }
                , userNameValidation
            );
            
            if (!loopCondition) break;

            auto userIdValidation = user_query_prompt_(userIdLogInPrompt_, connection);
            std::visit(
                [&](const auto& result) {
                    using T = std::decay_t<decltype(result)>;
                    if constexpr (std::is_same_v<std::string, T>) {
                        userId_.clear();
                        userId_ = result;
                        console.log(userId_);
                    }
                    else {
                        connection_stop_handler(connection);
                        loopCondition = result;
                    }
                }
                , userIdValidation
            );

            if (!loopCondition) break;

            if (!ec) {
                if (ClientList.log_in_client(userId_, userName_, connection)) {
                    connection->do_prompt_user(successLogInClientSide_);
                    console.log(successLogIn_);
                    // Function for which service you want to use? 
                    break;
                } 
            }
            
            if (!ec && connection->running_.load(std::memory_order_acquire)) {
                connection->do_prompt_user(
                    "Oops! Invalid User Name or User ID. Please try again. (×_×)");
                current_state_ = PromptState::INVALID_INPUT;
            }
        }
    }
    catch (...) {
        return catch_handler(connection);
    }
}

std::variant<bool, std::string> User_Manager::user_query_prompt_(std::string prompt, std::shared_ptr<TCP_Connection> connection) {
    error_code ec;
    userResponse_.clear();
    try {
        if (!ec && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user(prompt);
            userResponse_ = connection->read_from_user();
        }

        if (userResponse_ != readError_ && userResponse_ != readExit_) {
            std::string userName = sanitizer_(userResponse_);
            console.log(userName);
            return userName;
        }

        current_state_ = PromptState::Connection_Failed;
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
        current_state_ = PromptState::Connection_Failed;
        return false;
    }
    catch (const std::runtime_error& e) {
        console.log("Sign up process failed! ", e.what());
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("An unexpected error occurred during registration.");
            connection->running_.store(false, std::memory_order_release);
        }
        current_state_ = PromptState::Connection_Failed;
        return false;
    }
    catch (...) {
        console.log("Unknown error during sign up process");
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("An unexpected error occurred during registration.");
            connection->running_.store(false, std::memory_order_release);
        }
        current_state_ = PromptState::Connection_Failed;
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
