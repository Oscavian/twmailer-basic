#ifndef TWMAILERBASIC_SERVER_H
#define TWMAILERBASIC_SERVER_H

#include "ClientHandler.hpp"
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <thread>

#define CHECKNTHROW(X) ({int __val = (X); \
    if(__val == -1) { \
        throw std::runtime_error(std::string(strerror(errno))); \
    }})

namespace twServer {

    class Server{
        public:
            //basic methods for setting up server
            Server() = default;
            Server(int port, std::string mailDir);
            void start();
            void requestAbort();

            void setPort(int port);
            void setMailDir(const std::string& dir);

        private:
            void observe();
            void abort();

            std::vector<ClientHandler*> m_clients;
            std::vector<std::thread*> m_connections;
            int m_port;
            std::string m_mailDir;
            int m_serverSocket;
            bool m_abortRequested;

            

    };
}

#endif