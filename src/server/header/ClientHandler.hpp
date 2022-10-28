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

#include "../../share/ClientBase.h"

#define BUF 4096

namespace twServer {

    class ClientHandler : ClientBase {
        public:
            ClientHandler(std::string ipAddr, int *socket);
            ~ClientHandler();

            void start();
            void run();
            bool sendMessage(const char* buffer);
            void receiveMessage();
            void abort();

        private:
            int *m_socket;
            bool m_abortRequested;
            std::string m_ipAddr;
            char m_receiveBuffer[BUF];
        

    };

}

#endif