#include "Server.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>
#include <atomic>
#include <string>

using namespace std;
namespace ServerNamespace{
atomic<bool> shouldExit(false);

void receiveMessages(SOCKET clientSocket) {
    char recvBuffer[512];
    while (!shouldExit) {
        int byteCount = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (byteCount > 0) {
            recvBuffer[byteCount] = '\0';  // Null-terminate the received data
            cout << recvBuffer << endl;
        }
        else if (byteCount == 0) {
            cout << "Client disconnected." << endl;
            break;
        }
        else {
            cout << "recv() failed: " << WSAGetLastError() << endl;
            break;
        }
    }
}void sendMessages(SOCKET clientSocket, std::string username) {
    string sendBuffer;
    cout << "Enter a message to send to the server (or 'SHUTDOWN' to disconnect):\n ";
    while (!shouldExit) {
        getline(cin, sendBuffer);
        if (sendBuffer == "SHUTDOWN") {
            cout << "Disconnecting from server..." << endl;
            shouldExit = true;
            break;
        }
        string messageToSend = username + ": " + sendBuffer;

        int byteCount = send(clientSocket, messageToSend.c_str(), messageToSend.length(), 0);
        if (byteCount > 0) {

        }
        else {
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
    int port = 55555; // Unreserved port
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0) {
        cout << "WSAStartup failed with error: " << wsaerr << endl;
        return 1;
    }
    else {
        cout << "The Winsock dll is ready to use!\n";
        cout << "The status: " << wsaData.szSystemStatus << endl;
    }

    cout << "-> Creating a socket...  \n";
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else {
        cout << "socket() is running!\n";
    }

    cout << "->  Binding the socket...\n";
    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        cout << "bind() failed: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    else {
        cout << "bind() is completed...\n";
    }

    cout << "-> Listenin  on the socket...\n";
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        cout << "Error listening on socket: " << WSAGetLastError() << endl;
    }
    else {
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
    thread receiveThread(ServerNamespace::receiveMessages, clientSocket);
    thread sendThread(ServerNamespace::sendMessages, std::ref(clientSocket), username);

    // Wait for both threads to finish
    sendThread.join();

    // At this point, the user has typed SHUTDOWN
    // We need to make sure the receive thread exits too
    shutdown(clientSocket, SD_BOTH);  // This will unblock the receive call
    receiveThread.join();

    cout << "Starting cleanup...\n";
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    cout << "Server shut down." << endl;
    return 0;
}