#pragma once
#include "Client.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <algorithm> 
#include <optional> 
#include <mutex>

class All_Clients {
public:
    enum class ClientError {
        None,
        EmptyId,
        NullClient,
        ClientNotFound,
        ClientAlreadyExists,
        ClienIsNotValid,
        IdAlreadyTaken,
        InvalidState,
        RemovalFailed
    };
    All_Clients();
    ~All_Clients();
    bool add_new_user(std::shared_ptr<Client> newClient);
    bool if_client_exist(const std::shared_ptr<Client>& newClient) const;
    bool if_client_valid(const std::shared_ptr<Client>& newClient) const;
    bool is_id_taken(const std::string& id) const;
    bool verify_consistency(const std::string& id, const std::shared_ptr<Client> newClient) const;
    bool is_being_removed(const std::shared_ptr<Client>& client) const;
    bool insert_registered_client(const std::string& id, const std::shared_ptr<Client> newClient);
    bool remove_valid_client(const std::string& id);
    bool delete_client(const std::string& id, const std::shared_ptr<Client> newClient);
    std::optional<std::shared_ptr<Client>> valid_client_getter(const std::string& id) const;
    size_t get_total_clients() const;
    size_t get_registered_clients() const;
    ClientError get_last_error() const { return last_error_; };
private:
    mutable std::mutex valid_mtx_;
    mutable std::mutex client_mtx_;
    std::unordered_map<std::string, std::shared_ptr<Client>> valid_Clients_;
    std::vector<std::shared_ptr<Client>> all_clients_;
    mutable ClientError last_error_{ ClientError::None };
};

extern All_Clients ClientList;  