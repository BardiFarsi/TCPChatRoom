# Chat Room

# WebSocket Chatroom Project

This project is focused on creating a scalable chatroom system using modern C++ features and web technologies. The core architecture will include a **TCP WebSocket server**, **HTTPS handshake** for user authentication, and a custom logging system utilizing advanced C++20 metaprogramming techniques.

## Project Overview

The project is in its **early stages**, with initial steps already implemented. The goal is to create a real-time chat application that supports multiple users. A central server will handle WebSocket connections, providing an interactive chat experience for users.

Key features of the project include:

- **TCP WebSocket Chatroom**: Establishing WebSocket connections for real-time messaging between clients and the server.
- **HTTPS Handshake**: Implementing the initial authorization process for WebSocket connections via a secure HTTPS handshake.
- **Listener Script**: Using a `bash` script to configure the server and listen on port 3000 for incoming connections.
- **Custom Logging System**: Implementing a flexible logging system with C++20 metaprogramming features like variadic templates, `constexpr`, and SFINAE (Substitution Failure Is Not An Error).

## Project Structure

### `www/` Directory

The `www/` directory contains the **static web resources** needed to interact with the WebSocket server:

- **`index.html`**: The HTML file that serves as the entry point to the chat application.
- **`listener-port3000.sh`**: A bash script that configures the server to listen on port 3000 for incoming WebSocket connections. This script is essential for initializing the server.

### `main.cpp`

The `main.cpp` file contains the core logic for setting up the **TCP WebSocket server**:

- **TCP WebSocket Initialization**: The code establishes a WebSocket server on port 3000, allowing users to connect and send/receive messages in real-time.
- **Chatroom Functionality**: The basic structure for a chatroom, including support for multiple users, is in the initial stages of implementation.

### Header Files

The header files contain essential classes and utilities for the project:

1. **`Buffer_Sanitizer`**: A utility class designed to extract and sanitize input, ensuring that only readable characters are passed to the console.
2. **`LOGGER`**: A custom logging class implemented using C++20 metaprogramming techniques:
   - **Variadic Templates**: Used to handle an arbitrary number of log arguments.
   - **`constexpr`**: Allows certain log operations to be evaluated at compile-time, improving efficiency.
   - **SFINAE (Substitution Failure Is Not An Error)**: Template specialization and SFINAE techniques are used to automatically detect and format different data types, ensuring that only printable types are logged.

## Current Progress

- **Initial Setup**: The basic framework for the WebSocket server and chatroom functionality is established.
- **TCP WebSocket Server**: The WebSocket server is functional and listening on port 3000.
- **Logging System**: The custom `LOGGER` class has been designed and partially implemented with C++20 features.
- **Bash Script**: The `listener-port3000.sh` script is in place for setting up the server's listening port.

## Future Plans

- **Server Relay Architecture**: The next step will involve implementing a relay server to handle communication between clients and the central server.
- **User Authentication**: Implementing HTTPS for the handshake process to authenticate users before establishing the WebSocket connection.
- **Extended Chatroom Features**: Adding more features to the chatroom such as user authentication, private messages, and message history.
- **Error Handling & Buffer Sanitization**: Refining the `Buffer_Sanitizer` class for better error handling and input validation.

## Technologies Used

- **C++20**: Advanced C++ features such as metaprogramming, variadic templates, and `constexpr` are used to create a flexible logging system and other utilities.
- **WebSockets**: Real-time communication protocol for handling chat messages between the server and clients.
- **HTTPS**: Used for securing the WebSocket handshake and authenticating users.
- **Bash Scripting**: For configuring and running the server.

## Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd <repository-name>
2. Run the server setup script (listener-port3000.sh) to start listening on port 3000:
   ./www/listener-port3000.sh
3. Compile and run the C++ code:
   g++ -std=c++20 main.cpp -o chatroom
./chatroom

## Contributing

Contributions to the project are welcome! If you'd like to contribute, please fork the repository and create a pull request with your proposed changes. Ensure that your code adheres to the project's coding style and that all tests pass before submitting your pull request.

## License
This project is licensed under the GNU General Public License (GPL). See the LICENSE file for details.

Note: This project is still in its early stages and is actively being developed. Keep an eye out for future updates and feature implementations.


---

This **README** is structured to explain the project's goals, current state, and future plans in a professional manner. Feel free to modify or add any additional details that fit your specific use case. Let me know if you need any changes or additions!

