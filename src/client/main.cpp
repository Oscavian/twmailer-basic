#include <iostream>
#include <vector>
#include "header/Client.hpp"

int main(int argc, char **argv) {

    std::vector<std::string> args(argv, argv + argc);
    int port;
    std::string ip;

    try {
        //ip and port required
        if (args.size() != 3) {
            throw std::invalid_argument("Invalid arguments");
        }

        ip = args[1];
        port = std::stoi(args[2]);


    } catch (...) {
        std::cerr << "Error: Invalid program usage.\n";
        std::cerr << "Usage: " << args[0] << " <ip> <port>\n";
        return EXIT_FAILURE;
    }

    try {
        //sets up the client with all necessary parameters
        twClient::Client client(ip, port);
        client.start();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

}