#include "header/ClientHandler.hpp"
#include "../share/protocol.h"

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
            std::cout << "Message received: " << m_receiveBuffer << "\n";

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


