#include <iostream>
#include "header/Server.hpp"

int main(int argc, char** argv){

    int port;
    std::string mailDir;
    std::vector<std::string> args(argv, argv + argc);

        try {
        if (args.size() != 3) {
            throw std::invalid_argument("Invalid arguments");
        }

        //TODO: add validation
        port = std::stoi(args[1]);
        mailDir = argv[2];

    } catch (...) {
        std::cerr << "Error: Invalid program usage.\n";
        std::cerr << "Usage: " << args[0] << " <port> <mailSpoolDirectory>\n";
        return EXIT_FAILURE;
    }    

    twServer::Server server(port, mailDir);

    try {

        server.start();

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;

}

