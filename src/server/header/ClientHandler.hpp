#ifndef TWMAILERBASIC_CLIENTHANDLER_H
#define TWMAILERBASIC_CLIENTHANDLER_H

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include "Request.hpp"
#include "Ldap.hpp"

#include "../../share/ClientBase.h"

#define BUF 4096

namespace twServer {

    class ClientHandler : ClientBase {
        public:
            ClientHandler(std::string ipAddr, int *socket, std::string mailDir, int clientId);
            virtual ~ClientHandler();

            //basic methods for interacting with client
            void start();
            void run();
            bool sendBuffer(const char* buffer);
            int receiveBuffer();

            //method for naming new messages
            std::string getNextID(std::string user, std::string path);

            //methods for reacting to client request
            void saveMessage(Request content, std::string path);
            void listMessages(std::string path);
            void readMessage(std::string path, std::string msgNum);
            void deleteMessage(std::string path, std::string msgNum);
            bool loginUser(std::string username, std::string password);
            void abort();

            static std::mutex m_fileMutex;

        private:
            int *m_socket;
            bool m_abortRequested;
            std::string m_ipAddr;
            char m_receiveBuffer[BUF];
            std::string m_mailDir;
            int m_clientId;
        

    };

}

#endif