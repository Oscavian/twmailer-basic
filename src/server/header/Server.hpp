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

#define CHECKNTHROW(X) ({int __val = (X); \
    if(__val == -1) { \
        throw std::runtime_error(std::string(strerror(errno))); \
    }})

namespace twServer {

    class Server{
        public:
            Server(int port, std::string mailDir);
            void start();
            void observe();
            void abort();

        private:
            //std::vector<ClientHandler*> m_clients;
            int m_port;
            std::string m_mailDir;
            int m_serverSocket;

            

    };
}

#endif