#include "All_Clients.h"

All_Clients ClientList;

All_Clients::All_Clients() = default;

All_Clients::~All_Clients() = default;

bool All_Clients::add_new_user(std::shared_ptr<Client> newClient) {
    last_error_ = ClientError::None;
    if (!newClient) {
        return false;
        last_error_ = ClientError::NullClient;
    }

    if (!if_client_exist(newClient)) {
        std::lock_guard<std::mutex> lock(client_mtx_);
        all_clients_.push_back(newClient);
        return true;
    }
    last_error_ = ClientError::ClientAlreadyExists;
    return false;
}

bool All_Clients::if_client_exist(const std::shared_ptr<Client>& newClient) const {
    last_error_ = ClientError::None;
    std::lock_guard<std::mutex> lock(client_mtx_);
    for (const auto& client : all_clients_) {
        if (newClient.get() == client.get()) {
            return true;
        }
    }
    last_error_ = ClientError::ClientAlreadyExists;
    return false;
}

bool All_Clients::if_client_valid(const std::shared_ptr<Client>& newClient) const {
    std::lock_guard<std::mutex> lock(valid_mtx_);
    return std::any_of(valid_Clients_.begin(), valid_Clients_.end(),
        [&newClient](const auto& pair) {  
            return newClient.get() == pair.second.get();
        });
}

bool All_Clients::insert_registered_client(const std::string& id, const std::shared_ptr<Client> newClient) {
    last_error_ = ClientError::None;
    if (id.empty()) {
        last_error_ = ClientError::EmptyId;
        return false;
    }
    if (!newClient) {
        last_error_ = ClientError::NullClient;
        return false;
    }

    if (!verify_consistency(id, newClient)) {
        std::lock_guard<std::mutex> lock(valid_mtx_);
        valid_Clients_.emplace(id, newClient);
        return true;
    }
    last_error_ = ClientError::InvalidState;
    return false;
}

std::optional<std::shared_ptr<Client>> All_Clients::valid_client_getter(const std::string& id) const {
    last_error_ = ClientError::None;
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

bool All_Clients::verify_consistency(const std::string& id, const std::shared_ptr<Client> newClient) const {
    if (id.empty() || !newClient) {
        return false;
    }

    std::scoped_lock lock(valid_mtx_, client_mtx_);
    return (
        if_client_exist(newClient) && 
        if_client_valid(newClient) &&
        is_id_taken(id)
        );
}

bool All_Clients::remove_valid_client(const std::string& id) {
    last_error_ = ClientError::None;
    std::lock_guard<std::mutex> lock_valid(valid_mtx_);
    auto it = valid_Clients_.find(id);
    if (it != valid_Clients_.end()) {
        valid_Clients_.erase(it);
        return true;
    }
    last_error_ = ClientError::RemovalFailed;
    return false;
}

bool All_Clients::delete_client(const std::string& id, const std::shared_ptr<Client> newClient) {
    last_error_ = ClientError::None;
    if (id.empty()) {
        last_error_ = ClientError::EmptyId;
        return false;
    }
    if (!newClient) {
        last_error_ = ClientError::NullClient;
        return false;
    }

    bool removed = is_being_removed(newClient) && remove_valid_client(id);
    if (removed) {
        std::lock_guard<std::mutex> lock(client_mtx_);
        auto it = std::find(all_clients_.begin(), all_clients_.end(), newClient);
        if (it != all_clients_.end()) {
            all_clients_.erase(it);
            return true;
        }
    }

    last_error_ = ClientError::RemovalFailed;
    return false;
}

bool All_Clients::is_being_removed(const std::shared_ptr<Client>& client) const {
    std::scoped_lock lock(valid_mtx_, client_mtx_);
    return (!if_client_exist(client) && !if_client_valid(client));
}

size_t All_Clients::get_total_clients() const {
    std::lock_guard<std::mutex> lock(client_mtx_);
    return all_clients_.size();
}

size_t All_Clients::get_registered_clients() const {
    std::lock_guard<std::mutex> lock(valid_mtx_);
    return valid_Clients_.size();
}
