#include "header/ClientHandler.hpp"
#include "../share/protocol.h"
#include <sstream>

namespace twServer {
    ClientHandler::ClientHandler(std::string ipAddr, int *socket){
        m_ipAddr = ipAddr;
        m_socket = socket;
    }   

    ClientHandler::~ClientHandler() {
        delete m_socket;
    }

    void ClientHandler::start() {
        run();
    }

    void ClientHandler::run() {
        int size;
        this->sendMessage("Welcome!\n");

        do {
            //receive data

            size = recv(*m_socket, m_receiveBuffer, BUF - 1, 0);

            if (size == -1) {
                if (m_abortRequested) {
                    std::cerr << "recv error after aborted\n";
                } else {
                    std::cerr << "recv error";
                }
                break;
            }

            if (size == 0) {
                std::cout << "Client closed remote socket\n";
                break;
            }

            //remove newline from client message
            if(m_receiveBuffer[size - 2] == '\r' && m_receiveBuffer[size -1] == '\n'){
                size -= 2;
            } else if(m_receiveBuffer[size - 1] == '\n'){
                size -= 1;
            }

            //null terminate message & print to stdout
            m_receiveBuffer[size] = '\0';
            std::cout << "Input received: " << m_receiveBuffer << "\n";
            std::string message = m_receiveBuffer;

            //here parsing client input
              if(message.find(CMD_SEND)!=std::string::npos){
                    std::string command = CMD_SEND;
                    int start_pos = message.find(command);
                    //+1 to remove whitespace
                    int end_pos = command.size() + 1;
                    message.erase(start_pos, end_pos);
                    
                    std::cout << "Message received: " << message << "\n\n";
                    //save msg in doc

                }else if(message.find(CMD_LIST)!=std::string::npos){
                    std::string command = CMD_LIST;
                    int start_pos = message.find(command);
                    //+1 to remove whitespace
                    int end_pos = command.size() + 1;
                    std::string user = message.erase(start_pos, end_pos);

                    std::cout << "list msgs of " << user << "\n\n";
                    //get list of all msg of user

                }else if(message.find(CMD_READ)!=std::string::npos){
                    std::string command = CMD_READ;
                    int start_pos = message.find(command);
                    //+1 to remove whitespace
                    int end_pos = command.size() + 1;
                    message.erase(start_pos, end_pos);
                    std::string msg;
                    std::string user;
                    std::istringstream ss(message);

                    int i = 0;
                    while(getline(ss, msg, ' ')) {
                        if(i==0){
                            user = msg;
                        }
                        i++;
                    }
                    std::cout << "read " << msg << " of " << user << "\n\n";
                    //read specific msg of user

                }else if(message.find(CMD_DEL)!=std::string::npos){
                    std::string command = CMD_DEL;
                    int start_pos = message.find(command);
                    //+1 to remove whitespace
                    int end_pos = command.size() + 1;
                    message.erase(start_pos, end_pos);
                    std::string msg;
                    std::string user;
                    std::istringstream ss(message);

                    int i = 0;
                    while(getline(ss, msg, ' ')) {
                        if(i==0){
                            user = msg;
                        }
                        i++;
                    }
                    std::cout << "delete " << msg << " of " << user << "\n\n";
                    //delete specific msg of user
                }

            //send confirmation msg
            if(!sendMessage(SERV_OK)) {
                throw std::runtime_error("Sending answer failed.");
            }

        } while(strcmp(m_receiveBuffer, CMD_QUIT) != 0 && !m_abortRequested);

        abort();

    }

    bool ClientHandler::sendMessage(const char* buffer) {
        return send(*m_socket, buffer, strlen(buffer), 0) != -1;
    }

    void ClientHandler::abort() {
        std::cout << "Bye Bye Client!\n";

        if(*m_socket != -1) {
            if(shutdown(*m_socket, SHUT_RDWR) == -1) {
                throw std::runtime_error("Nothing to shutdown...");
            }

            if(close(*m_socket) == -1) {
                throw std::runtime_error("Nothing to close...");
            }
            *m_socket = -1;
        }
    }

    void ClientHandler::receiveMessage(){
        
    }

    
}


