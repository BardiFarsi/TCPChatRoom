#include "Online_Clients.h"

Online_Clients::Online_Clients()
{
	userVariables_.push_back("User Name: ");
	userVariables_.push_back("Client ID: ");
	//clientId_ = generate_id(); 
}

Online_Clients::~Online_Clients() = default;
/*


std::string Online_Client::generate_id(){
	std::string id;
	try {
		if (!clientHasId.load(std::memory_order_relaxed)) {
			std::call_once(idGen_stop_flag_, [&]() {
				id = server_.client_id_generator(*this);
				console.log("The client ID: ", clientId_, " generated for the user: ", userName_);
				clientHasId.store(true, std::memory_order_relaxed);
				return id;
				});
		}
	}
	catch (const std::exception& e) {
		console.log("Warning the client already has an ID");
		console.log("The Client ID is: ", clientId_);
		throw std::runtime_error("Error!. The client already has an ID!");
	}
}
*/

std::string Online_Clients::getter_user_name() const {
	return userName_;
}
std::string Online_Clients::getter_client_id() const {
	return clientId_;
}