#ifndef TWMAILERBASIC_CLIENT_H
#define TWMAILERBASIC_CLIENT_H

#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../../share/ClientBase.h"

#define BUF 4096

namespace twClient {
    class Client : ClientBase {
    public:
        Client(const std::string& ip, int port);
        ~Client();

        void start();
        void run();
        void abort();

        bool sendBuffer(const char *buffer);
        int receiveBuffer();

        std::string handleSend();
        std::string handleRead();
        std::string handleDel();
        std::string handleList();

    private:
        struct sockaddr_in m_address{};
        int m_port;
        int m_socket;
        char m_recvBuffer[BUF]{};

    };
}

#endif