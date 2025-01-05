#include "All_Clients.h"

All_Clients ClientList;

All_Clients::All_Clients() = default;

All_Clients::~All_Clients() = default;

bool All_Clients::add_new_user(std::shared_ptr<Client> newClient) {

    if (!newClient) {
        return false;
        last_error_ = ClientError::NullClient;
    }

    if (!if_client_exist(newClient->get_client_email())) {
        std::lock_guard<std::mutex> lock(client_mtx_);
        all_clients_.push_back(newClient);
        return true;
    }
    last_error_ = ClientError::ClientAlreadyExists;
    return false;
}

bool All_Clients::if_client_exist(const std::string& email) const {
    std::lock_guard<std::mutex> lock(client_mtx_);
    for (const auto& client : all_clients_) {
        if (email == client->get_client_email()) {
            last_error_ = ClientError::ClientAlreadyExists;
            return true;
        }
    }
    return false;
}

bool All_Clients::if_client_valid(const std::string& id, const std::string& email) const {
    std::lock_guard<std::mutex> lock(valid_mtx_);
    if (const auto it = valid_Clients_.find(id); it != valid_Clients_.end()) {
        return email == it->second->get_client_email();
    }
    return false;
}

// Password can be replaced here
bool All_Clients::log_in_client(const std::string& userId, const std::string& email, 
    std::shared_ptr<TCP_Connection> connection) {
    std::lock_guard<std::mutex> lock(valid_mtx_);
    if (auto it = valid_Clients_.find(userId); it != valid_Clients_.end()) {
        if (email == it->second->get_client_email()) {
            it->second->connection_ = std::move(connection);
            return true;
        }
    }
    return false;
}

bool All_Clients::insert_registered_client(const std::string& id, std::string email, const std::shared_ptr<Client> newClient) {
    if (id.empty() && email.empty()) {
        last_error_ = ClientError::EmptyType;
        return false;
    }
    if (!newClient) {
        last_error_ = ClientError::NullClient;
        return false;
    }

    if (!verify_consistency(id, email)) {
        std::lock_guard<std::mutex> lock(valid_mtx_);
        auto [it, inserted] = valid_Clients_.emplace(id, newClient);
        return inserted;
    }
    last_error_ = ClientError::InvalidState;
    return false;
}

std::optional<std::shared_ptr<Client>> All_Clients::valid_client_getter(const std::string& id) const {
    std::lock_guard<std::mutex> lock(valid_mtx_);
    auto it = valid_Clients_.find(id);

    if (it != valid_Clients_.end()) {
        return it->second; 
    }
    last_error_ = ClientError::ClienIsNotValid;
    return std::nullopt;
}

bool All_Clients::is_id_taken(const std::string& id) const {
    return valid_client_getter(id).has_value();;
}

bool All_Clients::verify_consistency(const std::string& id, const std::string& email) const {
    if (id.empty() || !email.empty()) {
        return false;
    }

    return (
        if_client_exist(email) && 
        if_client_valid(id, email) &&
        is_id_taken(id)
        );
}

bool All_Clients::remove_valid_client(const std::string& id) {
    std::lock_guard<std::mutex> lock_valid(valid_mtx_);
    
    if (auto it = valid_Clients_.find(id); it != valid_Clients_.end()) {
        valid_Clients_.erase(it);
        return true;
    }
    last_error_ = ClientError::RemovalFailed;
    return false;
}

bool All_Clients::delete_client(const std::string& email) {

    if (email.empty()) {
        last_error_ = ClientError::EmptyType;
        return false;
    }

    std::lock_guard<std::mutex> lock(client_mtx_);

    if (auto it = std::find_if(all_clients_.begin(), all_clients_.end(),
        [&email](const auto& client) { return email == client->get_client_email(); });
        it != all_clients_.end())
    {
        all_clients_.erase(it);
        return true;
    }

    last_error_ = ClientError::RemovalFailed;
    return false;
}

bool All_Clients::delete_consistency(const std::string& id, const std::string& email) {
    if (delete_client(email) && remove_valid_client(id)) {
        return true;
    }
    return false;
}

size_t All_Clients::get_total_clients() const {
    std::lock_guard<std::mutex> lock(client_mtx_);
    return all_clients_.size();
}

size_t All_Clients::get_registered_clients() const {
    std::lock_guard<std::mutex> lock(valid_mtx_);
    return valid_Clients_.size();
}
