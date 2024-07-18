

# Server_Client

This project implements a simple LAN-based server-client data transferring program using Winsock2. The server and client programs allow users to send and receive messages over a local network(for now ).

## Features

- Server and client communication using Winsock2
- Real-time messaging between server and client
- Multithreaded message handling

## Prerequisites

- Windows operating system
- Visual Studio or any C++ compiler supporting Winsock2
- Cmake
- Winsock2 library

## Installation

1. **Clone the repository:**

    ```bash
    git https://github.com/AhmedUtkuOzudogru/Server_Client
    cd Server_Client
    ```

2. **Open the project in your preferred C++ IDE (e.g., Visual Studio).**

3. **Build the project:**
    - Ensure that the Winsock2 library is linked.
    - Compile the `Server.cpp`, `Client.cpp` and  `Main.cpp` files.

## Usage
### Run the Main.cpp
- Enter your username
- Choose between host or join a chat
  
### Messaging

- **Sending Messages:**
  - Type your message and press Enter to send.
  - To disconnect, type `SHUTDOWN`.

- **Receiving Messages:**
  - Messages from the other user will be displayed in the terminal.

## Code Overview

### Server

- **`Server.cpp`**: Contains the server-side code for setting up the server, accepting client connections, and handling message sending and receiving.

    ```cpp
    // Server.cpp
    #include "Server.h"
    // ... (includes and namespace)
    
    void receiveMessages(SOCKET clientSocket, const string& username) {
        // Function to receive messages from the client
    }

    void sendMessages(SOCKET clientSocket, std::string username) {
        // Function to send messages to the client
    }

    int Server::serverFunction(string username) {
        // Main server function to initialize Winsock and handle client connections
    }
    ```

### Client

- **`Client.cpp`**: Contains the client-side code for connecting to the server and handling message sending and receiving.

    ```cpp
    // Client.cpp
    #include "Client.h"
    // ... (includes and namespace)
    
    void receiveMessages(SOCKET clientSocket) {
        // Function to receive messages from the server
    }

    void sendMessages(SOCKET clientSocket, std::string username) {
        // Function to send messages to the server
    }

    int Client::clientFunction(std::string username) {
        // Main client function to initialize Winsock and connect to the server
    }
    ```
