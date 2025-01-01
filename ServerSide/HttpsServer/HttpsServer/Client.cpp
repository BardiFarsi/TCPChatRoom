#include "Client.h"

Client::Client(TCP_Connection& connection)
    : connection_(connection) {}  

Client::~Client() {}