#include "header/Client.hpp"
#include <stdexcept>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

namespace twClient {
    Client::Client(std::string ip, int port) {
        
        m_port = port;
        m_socket = -1;
        
        std::cout << "Setting up client...\n";

        //init socket
        if((m_socket = socket(AF_INET, SOCK_STREAM, 0) == -1)){
            throw std::runtime_error("Error creating socket");
        }

        //init address
        memset(&m_address, 0, sizeof(m_address));

        m_address.sin_family = AF_INET;
        m_address.sin_port = htons(m_port);
        inet_aton(ip.c_str(), &m_address.sin_addr);

        std::cout << "Client setup successful!\n";
    }

    void Client::start() {
        

        if(connect(m_socket, (struct sockaddr*) &m_address, sizeof(m_address)) == -1) {
            throw std::runtime_error("Connection failed - no server available!");
        }

        std::cout << "Connection to " << inet_ntoa(m_address.sin_addr) << ": " << ntohs(m_address.sin_port) << " established\n";

        //todo: welcome message

    }

    void Client::run() {
        
        bool isQuit = false;
        std::string message;
    
        do {
            std::cout << ">> ";
            std::cin >> message;

            if(message != "" && message != "\n") {
                isQuit = (message == "quit");

                if (!sendMessage(message.c_str(), message.size())){
                    throw std::runtime_error("Send failed, abort...");
                }

                std::cout << "Message sent!\n";

                receiveMessage();
            }
        
        } while (!isQuit);
    }

    bool Client::sendMessage(const char *buffer, int size){
        return (send(m_socket, buffer, size, 0) != -1);
    }

    void Client::receiveMessage() {
        int size = recv(m_socket, m_recvBuffer, BUF - 1, 0);

        if(size == -1) {
            throw std::runtime_error("recv error");
        } else if (size == 0) {
            throw std::runtime_error("Server closed remote socket!");
        } else {
            m_recvBuffer[size] = '\0';

            std::cout << "<< " << m_recvBuffer << "\n";

            if(strcmp("OK", m_recvBuffer) != 0){
                throw std::runtime_error("Server error occured, abort...");
            }
        }
    }


    void Client::abort() {
        std::cout << "Bye Bye!\n";

        if(m_socket != -1) {
            if(shutdown(m_socket, SHUT_RDWR) == -1) {
                throw std::runtime_error("Shutting down socket failed.");
            }

            if(close(m_socket) == -1) {
                throw std::runtime_error("Closing socket failed.");
            }
            m_socket = -1;
        }
    }

    
}
