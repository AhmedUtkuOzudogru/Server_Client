#include "Client.h"
#include "Server.h"
#include <iostream>
#include <string>

class Main {
    std::string username;

public:
    void run() {
        std::cout << "Enter your username: ";
        std::getline(std::cin, username);

        while (true) {
            displayMenu();
            int choice;
            std::cin >> choice;
            std::cin.ignore();

            switch (choice) {
            case 1:
                startChat(username);
                break;
            case 2:
                hostChat(username);
                break;
            case 3:
                std::cout << "Exiting program. Goodbye!" << std::endl;
                return;
            default:
                std::cout << "Invalid option. Please try again." << std::endl;
                break;
            }

        }
    }

private:
    void displayMenu() {
        std::cout << "\n===== MENU =====" << std::endl;
        std::cout << "1. Chat" << std::endl;
        std::cout << "2. Host Chat" << std::endl; // Added this line
        std::cout << "3. Exit" << std::endl;
        std::cout << "Choose an option: ";
    }

    void startChat(const std::string& username) {
        Client client;
        client.clientFunction(username);
        run();
    }

    void hostChat(const std::string& username) {
        Server server;
        server.serverFunction(username);
    }
};

int main() {
    Main app;
    app.run();
    return 0;
}