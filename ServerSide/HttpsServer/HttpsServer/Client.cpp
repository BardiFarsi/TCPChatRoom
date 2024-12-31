#include "Client.h"

Client::Client(TCP_Server& server, TCP_Connection& connection)
    : server_(server), connection_(connection) {
}  

Client::~Client() {}