#pragma once
#include "Client.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

class All_Clients {
public:
    All_Clients();
    ~All_Clients();
    bool add_new_user(std::unique_ptr<Client> newClient);
    bool if_client_exist(const std::unique_ptr<Client>& newClient);
    void insert_registered_client(const std::string& id, const std::unique_ptr<Client>& newClient);
    std::unordered_map<std::string, std::unique_ptr<Client>> valid_Clients;
    Client* online_client_getter(std::string id);
private:
    std::vector<std::unique_ptr<Client>> all_clients_;
};

extern All_Clients ClientList;  