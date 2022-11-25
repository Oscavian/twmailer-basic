#include "header/Client.hpp"
#include "../share/protocol.h"
#include <stdexcept>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <termios.h>

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

    Client::~Client() {}

    void Client::start() {

        // Connect to server socket
        if (connect(m_socket, (struct sockaddr *) &m_address, sizeof(m_address)) == -1) {
            std::string msg = "Connect error - no server available at " + std::string(inet_ntoa(m_address.sin_addr)) + ":" + std::to_string(ntohs(m_address.sin_port));
            throw std::runtime_error(msg);
        }


        receiveBuffer();

        run();
    }

    void Client::run() {

        bool isQuit = false;
        std::string message;
        std::string input;
        std::string command;

        do {
            
            std::cout << "CMD >> ";
            std::getline(std::cin, command);

            if(command == CMD_LOGIN){
                message = command + "\n" + handleLogin();

            }else if(command == CMD_SEND){
                message = command + "\n" + handleSend();
            
            } else if(command == CMD_READ) {
                message = command + "\n" + handleRead();

            } else if(command == CMD_LIST) {
                message = command;

            } else if(command == CMD_DEL) {
                message = command + "\n" + handleDel();

            } else if(command == CMD_QUIT) {
                message = command + "\n";
                isQuit = true; 
            } else if(command == "?" || command == "HELP") {
                std::cout << "Available commands:\n"
                          << "LOGIN - login with username and password\n"
                          << "HELP - display this list\n"
                          << "QUIT - end connection to the server\n\n"
                          << "For logged in users:\n"
                          << "SEND - send a mail to a recipients mailbox\n"
                          << "READ - read a mail from a users mailbox\n"
                          << "LIST - list a users mailbox items\n"
                          << "DEL - delete a mail\n";
                          
                continue;
            } else {
                std::cout << "Invalid command. Type '?' for help.\n";
                continue;
            }
            

            if (!message.empty() && message != "\n") {
                if (!sendBuffer(message.c_str())) {
                    throw std::runtime_error("Send failed, abort...");
                }   
                std::cout << "Message sent! Message:\n-----\n" << message << "\n-----\n";
                message.clear();

                receiveBuffer();
            } else {
                std::cout << "Message is empty!\n";
            }

        } while (!isQuit);

        abort();

    }

    std::string Client::handleLogin(){
        std::string username;
        std::string password;

        while(username.empty()){
            std::cout << "USERNAME: ";
            std::getline(std::cin, username);
        }
        password = getpass();

        return username + "\n" + password;
        
    }

    std::string Client::handleSend() {
        std::string receiver;
        std::string subject;
        std::string message;

        while(receiver.empty()){
            std::cout << "RECEIVER >> ";
            std::getline(std::cin, receiver);
        }

        std::cout << "SUBJECT >> ";
        std::getline(std::cin, subject);

        std::cout << "MESSAGE (end with dot + newline)\n>> ";
        
        std::string line;
        
        do {
            std::getline(std::cin, line);
            message.append(line + "\n");
            std::cout << ">> ";
        } while (line.length() == 0 || !((line.length() == 1) && line.at(0) == '.'));

        return receiver + "\n" + subject + "\n" + message;

    }

    std::string Client::handleRead() {
        std::string msgnr;

        std::cout << "MSGNR >> ";
        std::getline(std::cin, msgnr);
        
        return msgnr + "\n";
    }

    std::string Client::handleDel() {
        std::string msgnr;

        std::cout << "MSGNR >> ";
        std::getline(std::cin, msgnr);

        return msgnr + "\n";
    }

    int Client::getch(){
        int ch;
        struct termios t_old, t_new;

        // tcgetattr() gets the parameters associated with the object referred by fd and stores them in the termios structure referenced by termios_p
        tcgetattr(STDIN_FILENO, &t_old);
        
        // copy old to new to have a base for setting c_lflags
        t_new = t_old;
        t_new.c_lflag &= ~(ICANON | ECHO);
        
        // sets the attributes
        tcsetattr(STDIN_FILENO, TCSANOW, &t_new);

        ch = getchar();

        // reset stored attributes
        tcsetattr(STDIN_FILENO, TCSANOW, &t_old);

        return ch;
    }

    std::string Client::getpass(){
        int show_asterisk = 1;

        const char BACKSPACE = 127;
        const char RETURN = 10;

        unsigned char ch = 0;
        std::string password;

        std::cout << "PASSWORD >> ";

        while ((ch = getch()) != RETURN){
            if (ch == BACKSPACE){
                if (password.length() != 0){
                    if (show_asterisk){
                        printf("\b \b"); // backslash: \b
                    }
                    password.resize(password.length() - 1);
                }
            }else {
                password += ch;
                if (show_asterisk){
                    printf("*");
                }
            }
        }
        printf("\n");
        return password;
    }

    bool Client::sendBuffer(const char *buffer) {
        return send(m_socket, buffer, strlen(buffer), 0) != -1;
    }

    int Client::receiveBuffer() {
        int size = recv(m_socket, m_recvBuffer, BUF - 1, 0);

        if (size == -1) {
            throw std::runtime_error("recv error");
        } else if (size == 0) {
            throw std::runtime_error("Server closed remote socket!");
        } else {
            m_recvBuffer[size] = '\0';

            std::cout << "<< " << m_recvBuffer << "\n";
        }
        return size;
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
