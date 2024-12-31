#pragma once

// This is a draft for debugging purposes - it won't compile
#if 0

#include <string>
#include <random>
#include <chrono>
#include <format>
#include <stdexcept>
#include <set>
#include <mutex>

class Online_Client {
private:
    static std::set<std::string> active_ids;  // Track active IDs
    static std::mutex id_mutex;               // Protect concurrent access
    static std::random_device rd;             // Hardware random number generator
    static std::mt19937_64 gen;              // Mersenne Twister engine

    // Length requirements for different parts of the ID
    static constexpr size_t TIMESTAMP_LENGTH = 10;
    static constexpr size_t RANDOM_LENGTH = 6;
    static constexpr size_t PREFIX_LENGTH = 3;

public:
    // Generate a unique client ID
    std::string generate_id() {
        try {
            std::string id;
            bool is_unique = false;

            // Keep trying until we generate a unique ID
            while (!is_unique) {
                id = create_new_id();

                // Thread-safe check and insertion
                std::lock_guard<std::mutex> lock(id_mutex);
                if (active_ids.find(id) == active_ids.end()) {
                    active_ids.insert(id);
                    is_unique = true;
                }
            }

            return id;

        }
        catch (const std::exception& e) {
            throw std::runtime_error(std::format("Failed to generate client ID: {}", e.what()));
        }
    }

    // Release an ID when client disconnects
    static void release_id(const std::string& id) {
        std::lock_guard<std::mutex> lock(id_mutex);
        active_ids.erase(id);
    }

    // Validate an ID format
    static bool is_valid_id(const std::string& id) {
        if (id.length() != PREFIX_LENGTH + TIMESTAMP_LENGTH + RANDOM_LENGTH) {
            return false;
        }

        // Check prefix format
        if (id.substr(0, PREFIX_LENGTH) != "uid") {
            return false;
        }

        // Check if the rest is hexadecimal
        return std::all_of(id.begin() + PREFIX_LENGTH, id.end(),
            [](char c) { return std::isxdigit(c); });
    }

private:
    // Create a new ID combining timestamp and random components
    std::string create_new_id() {
        // Get current timestamp
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()
        ).count();

        // Generate random component
        std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFF);  // 6 hex digits
        uint32_t random_component = dist(gen);

        // Format ID: uid + timestamp + random
        return std::format("uid{:0{}}:{:0{}x}",
            timestamp, TIMESTAMP_LENGTH,
            random_component, RANDOM_LENGTH);
    }
};

// Initialize static members
std::set<std::string> Online_Client::active_ids;
std::mutex Online_Client::id_mutex;
std::random_device Online_Client::rd;
std::mt19937_64 Online_Client::gen(Online_Client::rd());

// Example usage
void example_usage() {
    Online_Client client;

    // Generate some IDs
    auto id1 = client.generate_id();
    auto id2 = client.generate_id();

    std::cout << "Generated ID 1: " << id1 << std::endl;
    std::cout << "Generated ID 2: " << id2 << std::endl;

    // Validate IDs
    std::cout << "ID 1 is valid: " << std::boolalpha
        << Online_Client::is_valid_id(id1) << std::endl;

    // Release an ID when client disconnects
    Online_Client::release_id(id1);
}








#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

class Client {
private:
    std::string name;
    // other client properties

public:
    Client(std::string n) : name(std::move(n)) {}
    // other methods
};

class ClientManager {
private:
    // Store Client objects directly, not references
    std::unordered_map<std::string, Client> online_clients_id;

public:
    // Method 1: Using insert
    bool add_client_insert(const std::string& id, Client client) {
        auto [iter, inserted] = online_clients_id.insert({ id, std::move(client) });
        return inserted; // true if insertion happened, false if key already existed
    }

    // Method 2: Using try_emplace (preferred in modern C++)
    bool add_client_emplace(const std::string& id, Client client) {
        auto [iter, inserted] = online_clients_id.try_emplace(id, std::move(client));
        return inserted;
    }

    // Method 3: Check-then-insert with proper error handling
    bool add_client_safe(const std::string& id, Client client) {
        if (online_clients_id.find(id) != online_clients_id.end()) {
            // ID already exists
            return false;
        }
        online_clients_id.emplace(id, std::move(client));
        return true;
    }

    // Remove a client
    bool remove_client(const std::string& id) {
        return online_clients_id.erase(id) > 0;
    }

    // Check if client exists
    bool client_exists(const std::string& id) const {
        return online_clients_id.contains(id); // C++20
        // For pre-C++20: return online_clients_id.find(id) != online_clients_id.end();
    }

    // Get client count
    size_t client_count() const {
        return online_clients_id.size();
    }

    // Example of how to access a client safely
    std::optional<std::reference_wrapper<Client>> get_client(const std::string& id) {
        auto it = online_clients_id.find(id);
        if (it != online_clients_id.end()) {
            return std::reference_wrapper<Client>(it->second);
        }
        return std::nullopt;
    }
};

// Example usage
int main() {
    ClientManager manager;

    // Adding clients
    Client client1("Alice");
    if (manager.add_client_emplace("id123", std::move(client1))) {
        std::cout << "Client 1 added successfully\n";
    }

    // Try to add a client with same ID
    Client client2("Bob");
    if (!manager.add_client_emplace("id123", std::move(client2))) {
        std::cout << "Could not add client 2 - ID already exists\n";
    }

    // Check and access a client
    if (auto client = manager.get_client("id123")) {
        // Use the client
        // client->get().some_method();
    }

    // Remove a client
    if (manager.remove_client("id123")) {
        std::cout << "Client removed successfully\n";
    }

    return 0;
}

#endif