#include "TCP_Connection.h"

TCP_Connection::TCP_Connection(io_context& io_context, TCP_Server& server) :
	socket_(io_context),
	strand_(asio::make_strand(io_context)),
	server_(server),
	otherUser_(nullptr)
{
	running_.store(true, std::memory_order_release);
}

TCP_Connection::~TCP_Connection() {
    stop_process();
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
    try {
        error_code ec;
        if (!ec) {
            while (running_.load(std::memory_order_acquire)) {
                try {
                    readData_.clear();
                    readData_.resize(BUFF_SIZE);
                    size_t length;
                    try {
                        length = socket_.read_some(asio::buffer(readData_), ec);
                    }
                    catch (const boost::system::system_error& e) {
                        console.log("Socket read error: ", e.what());
                        running_.store(false, std::memory_order_release);
                        break;
                    }

                    if (!ec) {
                        std::lock_guard<std::mutex> lock(read_mtx_);
                        try {
                            Buffer_Sanitizer sanitizer;
                            std::string response = sanitizer(readData_);

                            if (response == "Exit++") {
                                console.log("The client exit the chat");
                                response.clear();
                                response = "A user is leaving the chatroom!";
                                server_.broadcast_message(response, shared_from_this());
                                running_.store(false, std::memory_order_release);
                                break;
                            }

                            try {
                                server_.broadcast_message(readData_, shared_from_this());
                            }
                            catch (const boost::system::system_error& e) {
                                console.log("Broadcast error: ", e.what());
                                running_.store(false, std::memory_order_release);
                                break;
                            }

                            console.log(response);
                            readData_.clear();
                        }
                        catch (const std::runtime_error& e) {
                            console.log("Sanitizer error: ", e.what());
                            running_.store(false, std::memory_order_release);
                            break;
                        }
                    }
                    else if (ec == asio::error::eof || ec == asio::error::connection_reset) {
                        console.log("Client disconnected!");
                        running_.store(false, std::memory_order_release);
                        break;
                    }
                    else {
                        console.log("Read error: ", ec.message());
                        running_.store(false, std::memory_order_release);
                        break;
                    }
                }
                catch (const std::system_error& e) {
                    console.log("System error in read loop: ", e.what());
                    running_.store(false, std::memory_order_release);
                    break;
                }
            }
        }
        else if (ec == asio::error::eof) {
            console.log("Connection closed by server!");
            running_.store(false, std::memory_order_release);
        }
        else {
            console.log("Read error: ", ec.message());
            running_.store(false, std::memory_order_release);
        }
    }
    catch (const std::exception& e) {
        console.log("Unexpected error in do_read: ", e.what());
        running_.store(false, std::memory_order_release);
    }
    catch (...) {
        console.log("Unknown error in do_read");
        running_.store(false, std::memory_order_release);
    }
}


void TCP_Connection::do_write(const std::string& message) {
    try {
        error_code ec;
        if (!ec) {
            while (running_.load(std::memory_order_acquire)) {
                try {
                    if (!ec) {
                        std::lock_guard<std::mutex> lock(write_mtx_);
                        try {
                            message_.clear();
                            console.log("Type your message: ");
                            std::getline(cin, message_);

                            if (message_ == "Exit++") {
                                console.log("Server is going to close the connection!");
                                message_.clear();
                                message_ = "Server is closing the chat! ";

                                try {
                                    writeData_.clear();
                                    writeData_.resize(message_.size());
                                    std::copy(message_.begin(), message_.end(), writeData_.begin());
                                    server_.broadcast_message(writeData_, shared_from_this());
                                }
                                catch (const std::bad_alloc& e) {
                                    console.log("Memory allocation failed: ", e.what());
                                    running_.store(false, std::memory_order_release);
                                    break;
                                }

                                running_.store(false, std::memory_order_release);
                                break;
                            }

                            try {
                                writeData_.clear();
                                writeData_.resize(message_.size());
                                std::copy(message_.begin(), message_.end(), writeData_.begin());
                            }
                            catch (const std::bad_alloc& e) {
                                console.log("Memory allocation failed: ", e.what());
                                running_.store(false, std::memory_order_release);
                                break;
                            }
                            catch (const std::length_error& e) {
                                console.log("Buffer size error: ", e.what());
                                throw;
                            }
                        }
                        catch (const std::ios_base::failure& e) {
                            console.log("I/O operation failed: ", e.what());
                            running_.store(false, std::memory_order_release);
                            break;
                        }
                    }
                    else if (ec == asio::error::eof) {
                        console.log("Connection closed by server!");
                        running_.store(false, std::memory_order_release);
                        break;
                    }
                    else {
                        console.log("Write error: ", ec.message());
                        running_.store(false, std::memory_order_release);
                        break;
                    }

                    if (!ec) {
                        try {
                            server_.broadcast_message(writeData_, nullptr);
                        }
                        catch (const boost::system::system_error& e) {
                            console.log("System error: ", e.what());
                            running_.store(false, std::memory_order_release);
                            break;
                        }
                    }
                    else if (ec == asio::error::eof) {
                        console.log("Connection closed by server!");
                        running_.store(false, std::memory_order_release);
                        break;
                    }
                    else {
                        console.log("Write error: ", ec.message());
                        running_.store(false, std::memory_order_release);
                        break;
                    }
                }
                catch (const std::system_error& e) {
                    console.log("System error: ", e.what());
                    running_.store(false, std::memory_order_release);
                    break;
                }
            }
        }
        else if (ec == asio::error::eof) {
            console.log("Connection closed by server!");
            running_.store(false, std::memory_order_release);
        }
        else {
            console.log("Write error: ", ec.message());
            running_.store(false, std::memory_order_release);
        }
    }
    catch (const std::exception& e) {
        console.log("Unexpected error in do_write: ", e.what());
        running_.store(false, std::memory_order_release);
    }
    catch (...) {
        console.log("Unknown error in do_write");
        running_.store(false, std::memory_order_release);
    }
}

void TCP_Connection::stop_process() {
    std::call_once(stop_flag_, [this]() {
        stop();
        });
}

void TCP_Connection::stop() {
	std::lock_guard<std::mutex> lock(stop_mtx_);
	running_.store(false, std::memory_order_release);

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