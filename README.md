# Chat Room
This Project is at early stage and incomplete.

**Note**
This project is open to the public solely for educational and non-commercial purposes. Any use of the code or derivations of this project for commercial purposes is strictly prohibited by the author.

# WebSocket Chatroom Project

This project is focused on creating a scalable chatroom system using modern C++ features and web technologies. The core architecture will include a **TCP WebSocket server**, **HTTPS handshake** for user authentication, and a custom logging system utilizing advanced C++20 metaprogramming techniques.

## Project Overview

The project is in its **early stages**, with initial steps already implemented. The goal is to create a real-time chat application that supports multiple users. A central server will handle WebSocket connections, providing an interactive chat experience for users.

## Key Features  

### 1. TCP Server  
- Acts as the central hub for incoming traffic, allowing clients to choose their desired service.  
- Supported services:  
  - **Broadcast Messaging**: Send messages to all connected clients.  
  - **Private Chat**: Chat one-on-one with a specific user.  
  - **Group Chat**: Create and manage private group chats with selected friends.  

### 2. Microservice for User Authentication  
- A separate microservice handles user authentication and login.  
- Uses **HTTPS protocol** to ensure secure communication.  
- Stores user data (e.g., email and password) securely in an **SQL database**, enabling users to log in with their credentials for future sessions.  

### 3. Client-Side Registration  
- Users must register and create an account on the server before accessing chat services.  
- Registration includes providing a valid email and password for authentication.
- Chosing a desired service
- Start to chat

### 4. Branch-Based Development  
- The project follows a branch-based versioning approach on GitHub.  
- Each branch represents a different implementation or feature set.  
- Explore different branches to experiment with various stages of development and added functionalities.  

---

## Current Status  

The project is in its early stages of development, and not all functionalities have been fully implemented. Work is ongoing to enhance both the client-side and server-side features.  

---


## How to Contribute  

1. Fork the repository.  
2. Create a feature branch: `git checkout -b feature-name`.  
3. Commit your changes: `git commit -m "Description of your feature"`.  
4. Push to the branch: `git push origin feature-name`.  
5. Open a pull request on the main repository.  

---

## Disclaimer  

This project is a work in progress. Some features may not function as intended, and breaking changes may occur as development continues.  

---

### Contact  

For questions or contributions, contact the author: **Masoud Farsi**  

## Project Structure

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

## Future Plans

- **Server Relay Architecture**: The next step will involve implementing a relay server to handle communication between clients and the central server.
- **User Authentication**: Implementing HTTPS for the handshake process to authenticate users before establishing the WebSocket connection.
- **Extended Chatroom Features**: Adding more features to the chatroom such as user authentication, private messages, and message history.

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

## License
This project is licensed under the Creative Commons Attribution-NonCommercial 4.0 International License (CC BY-NC 4.0). See the LICENSE file for more details.
Copyright © 2024 Masoud Farsi. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to use, copy, modify, and distribute the Software for non-commercial purposes only, subject to the following conditions:

1. The Software may not be used for commercial purposes, including but not limited to commercial software development, services, or distribution.
2. Any modifications or derivative works of the Software must retain this notice and the original copyright statement.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Note: This project is still in its early stages and is actively being developed. Keep an eye out for future updates and feature implementations.


---

This **README** is structured to explain the project's goals, current state, and future plans in a professional manner. Feel free to modify or add any additional details that fit your specific use case. Let me know if you need any changes or additions!

