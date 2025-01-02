#include "Client.h"

Client::Client(std::shared_ptr<TCP_Connection> connection)
    : connection_(std::move(connection)) {}  

Client::~Client() = default; 
