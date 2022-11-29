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
        void start() override;

        void run() override;

        bool sendBuffer(const char *buffer) override;

        int receiveBuffer() override;

        //method for naming new messages
        static std::string getNextID(const std::string &user, const std::string &path);

        //methods for reacting to client request
        void saveMessage(const Request &content, const std::string &path);

        void listMessages(const std::string &path);

        void readMessage(const std::string &path, const std::string &msgNum);

        void deleteMessage(const std::string &path, const std::string &msgNum);

        void abort() override;

        static std::mutex m_fileMutex;

    private:
        int *m_socket;
        std::string m_ipAddr;
        char m_receiveBuffer[BUF]{};
        std::string m_mailDir;
        int m_clientId;


    };

}

#endif