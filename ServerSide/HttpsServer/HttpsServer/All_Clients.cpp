#include "All_Clients.h"

All_Clients ClientList;

All_Clients::All_Clients() = default;

All_Clients::~All_Clients() = default;

bool All_Clients::add_new_user(std::unique_ptr<Client> newClient) {
    if (!if_client_exist(newClient)) {
        all_clients_.push_back(std::move(newClient));
        return true;
    }
    return false;
}

bool All_Clients::if_client_exist(const std::unique_ptr<Client>& newClient) {
    for (const auto& client : all_clients_) {
        if (newClient.get() == client.get()) {
            return true;
        }
    }
    return false;
}

void All_Clients::insert_registered_client(const std::string& id, const std::unique_ptr<Client>& newClient) {
    valid_Clients.emplace(id, std::move(newClient));
}

Client* All_Clients::online_client_getter(std::string id) {
    return valid_Clients[id].get(); 
}