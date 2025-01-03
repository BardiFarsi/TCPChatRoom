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
       
        if (connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user(userNamePrompt_);
        }
       
        userResponse_.clear();

        if (connection->running_.load(std::memory_order_acquire)) {
            userResponse_ = connection->read_from_user();
        }

        if (userResponse_ != readError_) {
            userName_ = sanitizer_(userResponse_);
        }
        else {
            console.log("Error in read handling");
        }

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
    catch (const boost::system::system_error& e) {
        console.log("Network error during sign up: ", e.what());
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("Network error occurred during registration. Please try again later.");
            connection->running_.store(false, std::memory_order_release);
        }
        return false;
    }
    catch (const std::runtime_error& e) {
        console.log("Sign up process failed! ", e.what());
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("An unexpected error occurred during registration.");
            connection->running_.store(false, std::memory_order_release);
        }
        return false;
    }
    catch (...) {
        console.log("Unknown error during sign up process");
        if (connection && connection->running_.load(std::memory_order_acquire)) {
            connection->do_prompt_user("An unexpected error occurred during registration.");
            connection->running_.store(false, std::memory_order_release);
        }
        return false;
    }
}

std::string User_Manager::create_registration_announcement(const std::string& userId) {
    std::string announcement = "Congratulations! Your account has been successfully created. ^_^ \n";
    announcement += userId;
    announcement += "\n. Please keep your User ID safe to be able to reconnect to server again. ";
    return announcement;
}

bool User_Manager::user_log_in(std::shared_ptr<TCP_Connection> connection) {

	console.log("Logging in..."); // NOT CREATED YET
	return true; 
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