//
// Created by user on 16.07.2024.
//

#ifndef CLIENT_H
#define CLIENT_H
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>


class Client {
public:
    static int clientFunction(std::string username);

};



#endif //CLIENT_H
