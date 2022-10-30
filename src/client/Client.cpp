#include "header/Client.hpp"
#include "../share/protocol.h"
#include <stdexcept>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

namespace twClient {
    Client::Client(const std::string &ip, int port) {

        m_port = port;
        m_socket = -1;

        std::cout << "Setting up client...\n";

        //init socket
        if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            throw std::runtime_error("Error creating socket");
        }


        //init address
        memset(&m_address, 0, sizeof(m_address));

        m_address.sin_family = AF_INET;
        m_address.sin_port = htons(port);
        inet_aton(ip.c_str(), &m_address.sin_addr);

        std::cout << "Client setup successful!\n";
    }

    Client::~Client() {
        /*
        try {
            abort();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
        */
    }

    void Client::start() {

        // Connect to server socket
        if (connect(m_socket, (struct sockaddr *) &m_address, sizeof(m_address)) == -1) {
            throw std::runtime_error("Connect error - no server available");
        }


        receiveMessage();

        run();
    }

    void Client::run() {

        bool isQuit = false;
        std::string message;

        do {
            std::cout << ">> ";
            std::getline(std::cin, message);

            if (!message.empty() && message != "\n") {
                isQuit = (message == CMD_QUIT);
                std::cout << "Message: " << message << std::endl;

                if (!sendMessage(message.c_str())) {
                    throw std::runtime_error("Send failed, abort...");
                }

                std::cout << "Message sent!\n";

                receiveMessage();
            }

        } while (!isQuit);

        abort();

    }

    bool Client::sendMessage(const char *buffer) {
        return send(m_socket, buffer, strlen(buffer), 0) != -1;
    }

    void Client::receiveMessage() {
        int size = recv(m_socket, m_recvBuffer, BUF - 1, 0);

        if (size == -1) {
            throw std::runtime_error("recv error");
        } else if (size == 0) {
            throw std::runtime_error("Server closed remote socket!");
        } else {
            m_recvBuffer[size] = '\0';

            std::cout << "<< " << m_recvBuffer << "\n";

            /*
            if (strcmp("OK", m_recvBuffer) != 0) {
                throw std::runtime_error("Server error occured, abort...");
            }
            */
        }
    }


    void Client::abort() {
        std::cout << "Bye Bye!\n";

        if (m_socket != -1) {
            if (shutdown(m_socket, SHUT_RDWR) == -1) {
                throw std::runtime_error("Nothing to shutdown...");
            }

            if (close(m_socket) == -1) {
                throw std::runtime_error("Nothing to close...");
            }
            m_socket = -1;
        }
    }


}
