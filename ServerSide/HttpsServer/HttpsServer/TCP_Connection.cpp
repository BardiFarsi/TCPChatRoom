#include "TCP_Connection.h"
TCP_Connection::TCP_Connection(io_context& io_context) : 
	socket_(io_context),
	strand_(io_context.get_executor()) {}

std::shared_ptr<TCP_Connection> TCP_Connection::create(io_context& io_context) {
	return std::make_shared<TCP_Connection>(io_context);
}

tcp::socket& TCP_Connection::socket() {
	return socket_; 
}


void TCP_Connection::start() {
	message_.clear();
	message_ = response();
	asio::async_write(socket_, asio::buffer(message_),
		asio::bind_executor(strand_, 
			std::bind(&TCP_Connection::handle_write, shared_from_this(), 
				asio::placeholders::error, asio::placeholders::bytes_transferred)));
}

void TCP_Connection::handle_write(const error_code& ec, size_t bytes_transferred) {
	if (ec) {
		console.log("Error in handle_write: ", ec.what());
	}
	console.log(bytes_transferred, " Bytes transferred to client.");
	if (socket_.is_open()) {
		socket_.shutdown(tcp::socket::shutdown_both);
		socket_.close();
	}
	return;
}

std::string TCP_Connection::set_time() {
	try {
		auto now = std::chrono::system_clock::now();
		std::time_t time = std::chrono::system_clock::to_time_t(now);
		std::tm localTime;
		{
			std::lock_guard<std::mutex> lock(date_mtx_);
			std::tm* LTPtr = std::localtime(&time);
			if (!LTPtr) {
				return "Error! std::localtime returned nullptr.";
			}
			else {
				localTime = *LTPtr;
			}
			std::stringstream ss;
			ss << std::put_time(&localTime, "%a %b %d %H:%M:%S %Y");
			return ss.str();
		}
	}
	catch (const std::exception& e) { // for std::bad_alloc & etc
		std::string err = "Caught error in time conversion: ";
		err += e.what();
		return err;
	}
}

std::string TCP_Connection::response() {
	std::lock_guard<std::mutex> lock(response_mtx_);
	std::string response = set_time();
	response += "\n Received your request!";
	return response;
}