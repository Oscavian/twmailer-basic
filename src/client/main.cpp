#include <iostream>
#include <vector>
#include "header/Client.hpp"

int main(int argc, char **argv) {

    std::vector<std::string> args(argv, argv + argc);
    int port;
    std::string ip;

    try {
        if (args.size() != 3) {
            throw std::invalid_argument("Invalid arguments");
        }

        //TODO: add validation
        ip = args[1];
        port = std::stoi(args[2]);


    } catch (...) {
        std::cerr << "Error: Invalid program usage.\n";
        std::cerr << "Usage: " << args[0] << " <ip> <port>\n";
        return EXIT_FAILURE;
    }

    try {
        twClient::Client client(ip, port);
        client.start();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

}