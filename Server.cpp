#include "Server.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <atomic>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

namespace ServerNamespace {
    atomic<bool> shouldExit(false);

    void receiveMessages(SOCKET clientSocket, const string& username) {
        char recvBuffer[512];
        // Send a message that the user has entered the chat
        string enterMessage = username + " has entered the chat.";
        send(clientSocket, enterMessage.c_str(), enterMessage.length(), 0);

        while (!shouldExit) {
            int byteCount = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            if (byteCount > 0) {
                recvBuffer[byteCount] = '\0';
                cout << recvBuffer << endl;
            } else if (byteCount == 0) {
                cout << "Client disconnected." << endl;
                break;
            } else {
                cout << "recv() failed: " << WSAGetLastError() << endl;
                break;
            }
        }
    }

    void sendMessages(SOCKET clientSocket, std::string username) {
        string sendBuffer;
        cout << "Enter a message to send to the client (or 'SHUTDOWN' to disconnect):\n ";
        while (!shouldExit) {
            getline(cin, sendBuffer);
            if (sendBuffer == "SHUTDOWN") {
                cout << "Disconnecting from client..." << endl;
                shouldExit = true;
                shutdown(clientSocket, SD_BOTH);
                closesocket(clientSocket);
                break;
            }
            string messageToSend = username + ": " + sendBuffer;
            int byteCount = send(clientSocket, messageToSend.c_str(), messageToSend.length(), 0);
            if (byteCount <= 0) {
                cout << "send() failed: " << WSAGetLastError() << endl;
                break;
            }
        }
    }
}

int Server::serverFunction(string username) {
    cout << "=============== SERVER ================\n";
    cout << "-> Setting up DLL...\n";
    SOCKET serverSocket, clientSocket;
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0) {
        cout << "WSAStartup failed with error: " << wsaerr << endl;
        return 1;
    } else {
        cout << "The Winsock dll is ready to use!\n";
        cout << "The status: " << wsaData.szSystemStatus << endl;
    }

    cout << "-> Creating a socket...  \n";
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    } else {
        cout << "socket() is running!\n";
    }

    cout << "->  Binding the socket...\n";
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(55555);
    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        cout << "bind() failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    } else {
        cout << "bind() is completed...\n";
    }

    // Get and display the server's IP address
    char hostName[256];
    gethostname(hostName, sizeof(hostName));
    struct addrinfo hints, *res;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(hostName, NULL, &hints, &res);
    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    char ipAddress[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(addr->sin_addr), ipAddress, INET_ADDRSTRLEN);
    cout << "Server IP address: " << ipAddress << endl;
    cout << "Port: 55555" << endl;

    cout << "-> Listening on the socket...\n";
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        cout << "Error listening on socket: " << WSAGetLastError() << endl;
    } else {
        cout << "listen() is active, waiting for connections..." << endl;
    }

    cout << "-> Accepting a connection...\n";
    clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        cout << "accept() failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }
    cout << "Client connected!\n";

    cout << "-> Start sending and receiving threads\n";
    thread receiveThread(ServerNamespace::receiveMessages, clientSocket, username);
    thread sendThread(ServerNamespace::sendMessages, std::ref(clientSocket), username);

    sendThread.join();
    ServerNamespace::shouldExit = true;
    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);
    receiveThread.join();

    cout << "Starting cleanup...\n";
    closesocket(serverSocket);
    WSACleanup();
    cout << "Server shut down." << endl;
    return 0;
}