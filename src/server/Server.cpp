#include "header/Server.hpp"
#include <thread>

namespace twServer {

    Server::Server(int port, std::string mailDir) {
        m_port = port;
        m_mailDir = mailDir;
    }

    void Server::start() {
        struct sockaddr_in address;

        int reuseValue = 1;

        std::cout << "Setting up the server...\n";

        // create a socket
        CHECKNTHROW(m_serverSocket = socket(AF_INET, SOCK_STREAM, 0));


        //set socket options 1
        CHECKNTHROW(setsockopt(m_serverSocket,
                               SOL_SOCKET,
                               SO_REUSEADDR,
                               &reuseValue,
                               sizeof(reuseValue)));

        //set socket options 2
        CHECKNTHROW(setsockopt(m_serverSocket,
                               SOL_SOCKET,
                               SO_REUSEPORT,
                               &reuseValue,
                               sizeof(reuseValue)));

        //init address
        memset(&address, 0, sizeof(address)); //zero out bits
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(m_port);

        //assign address+port to socket
        CHECKNTHROW(bind(m_serverSocket, (struct sockaddr *) &address, sizeof(address)));

        observe();
    }

    void Server::observe() {
        bool abortRequested = false;
        socklen_t addrlen;
        struct sockaddr_in cliaddress;
        int clientSocket;

        CHECKNTHROW(listen(m_serverSocket, 5));

        while (!abortRequested) {

            std::cout << "Waiting for connections...\n";

            //accept connection setup
            addrlen = sizeof(struct sockaddr_in);

            if ((clientSocket = accept(m_serverSocket, (struct sockaddr *) &cliaddress, &addrlen)) == -1) {
                if (abortRequested) {
                    std::cerr << "Accept error after aborted\n";
                } else {
                    std::cerr << "Accept error.\n";
                }
                break;
            }


            //start client
            fprintf(stdout, "Client connected from %s:%d...\n",
                    inet_ntoa(cliaddress.sin_addr),
                    ntohs(cliaddress.sin_port));

            //connection established, start communicating...


            auto client = new ClientHandler(std::string(inet_ntoa(cliaddress.sin_addr)), new int(clientSocket),
                                            m_mailDir, 1);
            m_clients.push_back(client);

            std::thread(&ClientHandler::run, *client).detach();

            //client.run();

            //reset socket
            clientSocket = -1;
        }

        abort();

    }

    void Server::abort() {
        std::cout << "Server Shutting down!\n";

        if (m_serverSocket != -1) {
            if (shutdown(m_serverSocket, SHUT_RDWR) == -1) {
                throw std::runtime_error("Nothing to shutdown...");
            }

            if (close(m_serverSocket) == -1) {
                throw std::runtime_error("Nothing to close...");
            }
            m_serverSocket = -1;
        }

        for (auto t: m_connections) {
            (*t).join();
        }

        for (auto c: m_clients) {
            delete c;
        }


    }

}