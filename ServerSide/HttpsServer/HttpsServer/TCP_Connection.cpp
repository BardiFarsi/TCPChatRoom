#include "TCP_Connection.h"

TCP_Connection::TCP_Connection(io_context& io_context, TCP_Server& server) :
	socket_(io_context),
	strand_(asio::make_strand(io_context)),
	server_(server)
{
	otherUser_ = nullptr; 
	running_ = true; 
}

TCP_Connection::~TCP_Connection() {
	stop();
}

std::shared_ptr<TCP_Connection> TCP_Connection::create(io_context& io_context, TCP_Server& server) {
	return std::make_shared<TCP_Connection>(io_context, server);
}

tcp::socket& TCP_Connection::socket() {
	return socket_;
}

void TCP_Connection::start(const std::string& message) {
	{
		std::lock_guard<std::mutex> lock(write_mtx_);
		message_.clear();
		message_ = response(); 
		message_ += message;
		writeData_.clear();
		writeData_.resize(message_.size());
		std::copy(message_.begin(), message_.end(), writeData_.begin());
	}
	error_code ec;
	if (!ec) {
	asio::write(socket_, asio::buffer(message_), ec);
	handle_communication();
	}
}

void TCP_Connection::handle_communication() {
	console.log("One user joined the chat!");
	read_thread_ = std::thread([this]() { do_read(); });
	write_thread_ = std::thread([this]() { do_write(""); });
}

void TCP_Connection::do_read() {
	while (running_) {
		readData_.clear();
		readData_.resize(BUFF_SIZE);
		error_code ec;
		size_t length = socket_.read_some(asio::buffer(readData_), ec);
		if (!ec) {
			std::lock_guard<std::mutex> lock(read_mtx_);
			Buffer_Sanitizer sanitizer;
			std::string response = sanitizer(readData_);
			if (response == "Exit++") {
				console.log("The client exit the chat");
				running_ = false;
				break;
			}
			server_.broadcast_message(readData_, shared_from_this());
			console.log(response);
			readData_.clear();
		}
		else if (ec == asio::error::eof || ec == asio::error::connection_reset) {
			console.log("Client disconnected!");
			running_ = false;
			break;
		}
		else {
			console.log("Read error: ", ec.message());
			running_ = false;
			break;
		}
	}
}

void TCP_Connection::do_write(const std::string& message) {
	while (running_) {
		{
			std::lock_guard<std::mutex> lock(write_mtx_);
			message_.clear();
			console.log("Type your message: ");
			std::getline(cin, message_);
			if (message_ == "Exit++") {
				console.log("Server is going to close the connection!");
				message_.clear();
				message_ = "User is leaving the chat! ";
				writeData_.clear();
				writeData_.resize(message_.size());
				std::copy(message_.begin(), message_.end(), writeData_.begin());
				server_.broadcast_message(writeData_, shared_from_this());
				running_ = false;
				break;
			}
			writeData_.clear();
			writeData_.resize(message_.size());
			std::copy(message_.begin(), message_.end(), writeData_.begin());
		}
		error_code ec;
		if (!ec) {
			server_.broadcast_message(writeData_, nullptr);
		}
		else {
			console.log("Write error: ", ec.message());
			running_ = false;
			break;
		}
	}
}

void TCP_Connection::stop() {
	running_ = false;
	try {
		error_code ec;
		socket_.shutdown(tcp::socket::shutdown_both, ec);
		socket_.close(ec);
	}
	catch (const std::exception& e) {
		console.log("Error during socket cleanup: ", e.what());
	}

	if (read_thread_.joinable()) read_thread_.join();
	if (write_thread_.joinable()) write_thread_.join();
	server_.remove_connection(shared_from_this());
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