#include "Client.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>
#include <atomic>
#include <string>

using namespace std;
namespace ClientNamespace
{



atomic<bool> shouldExit(false);

void receiveMessages(SOCKET clientSocket) {
    char recvBuffer[512];
    while (!shouldExit) {
        int byteCount = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        if (byteCount > 0) {
            recvBuffer[byteCount] = '\0';  // Null-terminate the received data
            cout <<  recvBuffer << endl;

        }
        else if (byteCount == 0) {
            cout << "Server disconnected." << endl;
            break;
        }
        else {
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
            shutdown(clientSocket, SD_BOTH);  // This will unblock the receive call
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
int Client::clientFunction(std::string username)
{
    cout << "========== CLIENT ==========\n";
  cout << "-> Setting up DLL...\n";
    SOCKET clientSocket;
    int port = 55555; // Unreserved port
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0)
    {
        cout << "WSAStartup failed with error: " << wsaerr << endl;
        return 0;
    }
    else
    {
        cout << "The Winsock dll is ready to use!\n";
        cout << "The status: " << wsaData.szSystemStatus << endl;
    }

    cout << "-> Creating a socket...  \n";
    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        cout << "Error at socket(): " << WSAGetLastError() << endl;
        WSACleanup();
        return 0;
    }
    else
    {
        cout << "socket() is running!\n";
    }

    cout << "-> Connecting to server...\n";
    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);
    clientService.sin_port = htons(port);
    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
    {
        cout << "Failed to connect.\n";
        WSACleanup();
        return 0;
    }
    else
    {
        cout << "Connected to server.\n";
    }

    cout << "-> Ready to  sending and receiving threads\n";
    thread receiveThread(ClientNamespace::receiveMessages, clientSocket);
    thread sendThread(ClientNamespace::sendMessages, std::ref(clientSocket), username);

    // Wait for both threads to finish
    sendThread.join();

    // At this point, the user has typed SHUTDOWN
    // We need to make sure the receive thread exits too
    receiveThread.join();

    cout << "=== Step 5 - Cleanup ===\n";
    closesocket(clientSocket);
    WSACleanup();
    cout << "Disconnected from server." << endl;
    return 0;

}