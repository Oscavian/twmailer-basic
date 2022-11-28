#include <iostream>
#include "header/Server.hpp"
#include <signal.h>

twServer::Server server;

void signalHandler(int sig) {
    std::cout << "SIGINT caught! Aborting...\n";
    server.requestAbort();

    exit(sig); //todo: handle better
}


int main(int argc, char** argv){

    (void) signal(SIGINT, signalHandler);

    int port;
    std::string mailDir;
    std::vector<std::string> args(argv, argv + argc);

    try {
        //port and directory necessary
        if (args.size() != 3) {
            throw std::invalid_argument("Invalid arguments");
        }

        port = std::stoi(args[1]);
        mailDir = argv[2];

    } catch (...) {
        std::cerr << "Error: Invalid program usage.\n";
        std::cerr << "Usage: " << args[0] << " <port> <mailSpoolDirectory>\n";
        return EXIT_FAILURE;
    }    

    server.setMailDir(mailDir);
    server.setPort(port);

    try {
        //sets off server functionality
        server.start();

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return 0;

}

