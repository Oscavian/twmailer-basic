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

#define BUF 4096

namespace twClient {
    class Client {
    public:
        Client(std::string ip, int port);

        std::string getCmd();

        std::string getSender();

        std::string getReceiver();

        std::string getSubject();

        void start();
        void run();
        void abort();

        bool sendMessage(const char *buffer, int size);
        void receiveMessage();

    private:
        struct sockaddr_in m_address;
        int m_port;
        int m_socket;
        char m_recvBuffer[BUF];

    };
}

#endif