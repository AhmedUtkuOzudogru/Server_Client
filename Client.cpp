#include "Client.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <atomic>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

namespace ClientNamespace {
    atomic<bool> shouldExit(false);
    atomic<bool> serverShutdown(false);

    void receiveMessages(SOCKET clientSocket) {
        char recvBuffer[512];
        while (!shouldExit) {
            int byteCount = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            if (byteCount > 0) {
                recvBuffer[byteCount] = '\0';
                cout << recvBuffer << endl;

                // Check if the server sent a shutdown message
                if (string(recvBuffer).find("Disconnecting from client...") != string::npos) {
                    cout << "Server is shutting down. Disconnecting..." << endl;
                    serverShutdown = true;
                    shouldExit = true;
                    break;
                }
            } else if (byteCount == 0) {
                cout << "Server disconnected." << endl;
                serverShutdown = true;
                break;
            } else {
                cout << "recv() failed: " << WSAGetLastError() << endl;
                break;
            }
        }
    }


    void sendMessages(SOCKET clientSocket, std::string username) {
        string sendBuffer;
        cout << "Enter a message to send to the server (or 'SHUTDOWN' to disconnect):\n ";
        while (!shouldExit) {
            getline(cin, sendBuffer);
            if (sendBuffer == "SHUTDOWN") {
                cout << "Disconnecting from server..." << endl;
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

int Client::clientFunction(std::string username) {
    cout << "========== CLIENT ==========\n";
    cout << "-> Setting up DLL...\n";
    SOCKET clientSocket;
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0) {
        cout << "WSAStartup failed with error: " << wsaerr << endl;
        return 0;
    } else {
        cout << "The Winsock dll is ready to use!\n";
        cout << "The status: " << wsaData.szSystemStatus << endl;
    }

    cout << "-> Creating a socket...  \n";
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    } else {
        cout << "socket() is running!\n";
    }

    cout << "-> Connecting to server...\n";
    string serverIP;
    cout << "Enter the server's IP address: ";
    getline(cin, serverIP);

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP.c_str(), &clientService.sin_addr.s_addr);
    clientService.sin_port = htons(55555);
    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        cout << "Failed to connect.\n";
        WSACleanup();
        return 0;
    } else {
        cout << "Connected to server.\n";
    }

    // Send username to server
    string usernameMsg = username + " has joined the chat.";
    send(clientSocket, usernameMsg.c_str(), usernameMsg.length(), 0);

    cout << "-> Ready to sending and receiving threads\n";
    thread receiveThread(ClientNamespace::receiveMessages, clientSocket);
    thread sendThread(ClientNamespace::sendMessages, std::ref(clientSocket), username);

    sendThread.join();
    ClientNamespace::shouldExit = true;
    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);
    receiveThread.join();

    cout << "=== Cleanup ===\n";
    WSACleanup();
    cout << "Disconnected from server." << endl;

    // Check if the disconnection was due to server shutdown
    if (ClientNamespace::serverShutdown) {
        cout << "Returning to main menu..." << endl;
        return 1;  // Return a specific value to indicate server shutdown
    }
    return 0;
}