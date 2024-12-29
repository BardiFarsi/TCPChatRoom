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
					stop();
				}
			}
		)
	);
}

void Client::start() {
	read_thread_ = std::thread([this]() { do_read(); });
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	write_thread_ =	std::thread([this]() { do_write(); });
}

void Client::do_read() {
	try {
		error_code ec;
		if (!ec) {
			while (running_) {
				try {
					if (!ec) {
						readData_.clear();
						readData_.resize(BUFF_SIZE);
						size_t length = socket_.read_some(asio::buffer(readData_), ec);
					}
					else if (ec == asio::error::eof) {
						console.log("Connection closed by server!");
						running_ = false;
						break;
					}
					else {
						console.log("Read error: ", ec.message());
						running_ = false;
						break;
					}

					if (!ec) {
						std::lock_guard<std::mutex> lock(read_mtx_);
						Span_Factory make_span;
						Buffer_Sanitizer sanitizer;
						std::span<std::byte> readSpan = make_span(readData_);
						std::string response = sanitizer(readSpan);

						if (response == "Exit++") {
							console.log("The other user exit the chatroom");
						}
						console.log(response);
						readData_.clear();
					}
					else if (ec == asio::error::eof) {
						console.log("Connection closed by server!");
						running_ = false;
						break;
					}
					else {
						console.log("Read error: ", ec.message());
						running_ = false;
						break;
					}
				}
				catch (const std::bad_alloc& e) {
					console.log("Memory allocation failed: ", e.what());
					running_ = false;
					break;
				}
				catch (const std::system_error& e) {
					console.log("System error: ", e.what());
					running_ = false;
					break;
				}
			}
		}
		else {
			console.log("Read error: ", ec.message());
			running_ = false;
		}
	}
	catch (const std::exception& e) {
		console.log("Unexpected error in read loop: ", e.what());
		running_ = false;
	}
}

void Client::do_write() {
	try {
		error_code ec;
		if (!ec) {
			while (running_) {
				try {
					if (!ec) {
						std::lock_guard<std::mutex> lock(write_mtx_);
						message_.clear();

						if (userInput_.empty() && username_.empty()) {
							console.log("What is your name?");
							std::getline(std::cin, username_);
							username_ += ": ";
						}

						if (!username_.empty()) {
							console.log("Type your message: ");
							std::getline(cin, userInput_);
						}

						if (userInput_ == "Exit++") {
							console.log("You are about to exit the chatroom!");
							writeData_.clear();
							writeData_.resize(userInput_.size());
							std::copy(userInput_.begin(), userInput_.end(), writeData_.begin());
							if (!ec) {
								asio::write(socket_, asio::buffer(writeData_), ec);
							}
							else if (ec == asio::error::eof) {
								console.log("Connection closed by server!");
								running_ = false;
								break;
							}
							else {
								console.log("Read error: ", ec.message());
								stop();
								break;
							}
							running_ = false;
							break;
						}

						message_ += username_;
						message_ += userInput_;
						writeData_.clear();
						writeData_.resize(message_.size());
						std::copy(message_.begin(), message_.end(), writeData_.begin());

						if (!ec) {
							asio::write(socket_, asio::buffer(writeData_), ec);
						}
						else if (ec == asio::error::eof) {
							console.log("Connection closed by server!");
							running_ = false;
							break;
						}
						else {
							console.log("Write error: ", ec.message());
							running_ = false;
							break;
						}
					}
					else if (ec == asio::error::eof) {
						console.log("Connection closed by server!");
						running_ = false;
						break;
					}
					else {
						console.log("Read error: ", ec.message());
						running_ = false;
						break;
					}
				}
				catch (const std::bad_alloc& e) {
					console.log("Memory allocation failed: ", e.what());
					running_ = false;
					break;
				}
				catch (const std::system_error& e) {
					console.log("System error: ", e.what());
					running_ = false;
					break;
				}
			}
		}
		else if (ec == asio::error::eof) {
			console.log("Connection closed by server!");
			running_ = false;

		}
		else {
			console.log("Write error: ", ec.message());
			running_ = false;
		}
	}
	catch (const std::exception& e) {
		console.log("Unexpected error in write loop: ", e.what());
		running_ = false;
	}
}

void Client::stop() {
	running_ = false;

	try {
		error_code ec;
		socket_.shutdown(tcp::socket::shutdown_both, ec);
		socket_.close(ec);
	}
	catch (const std::exception& e) {
		console.log("Error during socket cleanup: ", e.what());
	}

	if (read_thread_.joinable()) {
		read_thread_.join();
	}
	if (write_thread_.joinable()) {
		write_thread_.join();
	}
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