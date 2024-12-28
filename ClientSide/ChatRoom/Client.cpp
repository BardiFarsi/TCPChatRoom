#include "Client.h"


Client::Client(asio::io_context& io_context) : 
    io_context_(io_context), 
	socket_(io_context),
    strand_(io_context.get_executor()), 
    running_(true) {}

Client::~Client() {
	stop();
}

void Client::connect(const std::string& host, const std::string& port) {
	resolver resolver(io_context_);
	auto endpoints = resolver.resolve(host, port);
	asio::async_connect(socket_, endpoints,
		asio::bind_executor(strand_,
			[this](const error_code& ec, const tcp::endpoint& endpoint) {
				if (!ec) {
					console.log("Connected to ", endpoint.address().to_string());
					start();
				}
				else {
					console.log("Connection error: ", ec.message());
					running_ = false;
				}
			}
		)
	);
}

void Client::start() {
	read_thread_ = std::thread([this]() { do_read(); });
	write_thread_ =	std::thread([this]() { do_write(); });
}

void Client::do_read() {
	while (running_) {
		readData_.clear();
		readData_.resize(BUFF_SIZE);
		error_code ec;
		size_t length = socket_.read_some(asio::buffer(readData_), ec);
		if (!ec) {
			std::lock_guard<std::mutex> lock(read_mtx_);
			Span_Factory make_span;
			Buffer_Sanitizer sanitizer;
			std::span<std::byte> readSpan = make_span(readData_);
			std::string response = sanitizer(readSpan);
			if (response == "Exit++") {
				console.log("The other user exit the chatroom");
				running_ = false;
				break;
			}
			console.log(response);
			readData_.clear();
		}
		else if (ec == asio::error::eof) {
			console.log("Connection closed by server!");
			running_ = false;
		}
		else {
			console.log("Read error: ", ec.message());
			running_ = false;
		}
	}
}

void Client::do_write() {
	while (running_) {
		{
			std::lock_guard<std::mutex> lock(write_mtx_);
			message_.clear();
			console.log("Type your message: ");
			std::getline(cin, message_);
			if (message_ == "Exit++") {
				console.log("You are about to exit the chatroom!");
				writeData_.clear();
				writeData_.resize(message_.size());
				std::copy(message_.begin(), message_.end(), writeData_.begin());
				running_ = false;
				break;
			}
			writeData_.clear();
			writeData_.resize(message_.size());
			std::copy(message_.begin(), message_.end(), writeData_.begin());
		}
		error_code ec; 
		if (!ec) {
			asio::write(socket_, asio::buffer(writeData_), ec);
		}
		else {
			console.log("Write error: ", ec.message());
			running_ = false;
			break;
		}
	}
}

void Client::stop() {
	running_ = false;
	if (read_thread_.joinable()) read_thread_.join();
	if (write_thread_.joinable()) write_thread_.join();
}

std::string Client::set_time() {
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
		running_ = false;
		return err;
	}
}

std::string Client::response() {
	std::lock_guard<std::mutex> lock(response_mtx_);
	std::string response = set_time();
	response += "\n Received your request!";
	return response;
}