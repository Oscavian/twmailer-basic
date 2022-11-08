#include "header/ClientHandler.hpp"
#include "../share/protocol.h"
#include "header/Request.hpp"
#include <sstream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>

namespace twServer {
    ClientHandler::ClientHandler(std::string ipAddr, int *socket, std::string mailDir){
        m_ipAddr = ipAddr;
        m_socket = socket;
        m_mailDir = mailDir;
        m_abortRequested = false;
    }   

    ClientHandler::~ClientHandler() {
        delete m_socket;
    }

    void ClientHandler::start() {
        run();
    }

    void ClientHandler::run() {
        int size;
        this->sendMessage("Welcome!\n");

        do {
            //receive data
            size = receiveMessage();

            if(size == 0 || size == -1){
                break;
            }
            
            Request request = Request(std::istringstream(std::string(m_receiveBuffer)));

            //here parsing client input
            if(request.getMethod() == CMD_SEND){

                if(request.getMethod().empty()){
                    if(!sendMessage(SERV_ERR)) {
                        throw std::runtime_error("Sending answer failed.");
                    }
                    return;
                }

                std::cout << "Message received: \n" << request.getMessage() << " from user: " << request.getSender() << " to " << request.getReceiver() << "\n\n";

                //save msg in doc
                try {
                //here instead of ../ -> m_mailDir
                    std::string path = "../" + request.getReceiver();
                    makeDirSaveMessage(request.getReceiver(), path, request.getMessage());
                } catch(std::filesystem::filesystem_error & e){
                    std::cerr << e.what() << std::endl;
                }
                //send confirmation msg
                if(!sendMessage(SERV_OK)) {
                    throw std::runtime_error("Sending answer failed.");
                }

            } else if(request.getMethod() == CMD_LIST){

                std::cout << "list msgs of " << request.getUsername() << "\n\n";
                //send confirmation msg
                if(!sendMessage(SERV_OK)) {
                    throw std::runtime_error("Sending answer failed.");
                }
                //get list of all msg of user

            } else if(request.getMethod() == CMD_READ){

                std::cout << "read message #" << request.getMsgnum() << " of " << request.getUsername() << "\n\n";
                //send confirmation msg
                if(!sendMessage(SERV_OK)) {
                    throw std::runtime_error("Sending answer failed.");
                }
                //read specific msg of user

            } else if(request.getMethod() == CMD_DEL){
                std::cout << "delete message #" << request.getMsgnum() << " of " << request.getUsername() << "\n\n";
                //send confirmation msg
                if(!sendMessage(SERV_OK)) {
                    throw std::runtime_error("Sending answer failed.");
                }
                //delete specific msg of user
            }else{
                if(request.getMessage() != CMD_QUIT){
                    if(!sendMessage(SERV_ERR)) {
                        throw std::runtime_error("Sending answer failed.");
                    }
                }else{
                    if(!sendMessage(SERV_OK)) {
                        throw std::runtime_error("Sending answer failed.");
                    }
                }
                
            }

        } while(strcmp(m_receiveBuffer, CMD_QUIT) != 0 && !m_abortRequested);

        abort();

    }

    bool ClientHandler::sendMessage(const char* buffer) {
        return send(*m_socket, buffer, strlen(buffer), 0) != -1;
    }

    void ClientHandler::abort() {
        std::cout << "Bye Bye Client!\n";

        if(*m_socket != -1) {
            if(shutdown(*m_socket, SHUT_RDWR) == -1) {
                throw std::runtime_error("Nothing to shutdown...");
            }

            if(close(*m_socket) == -1) {
                throw std::runtime_error("Nothing to close...");
            }
            *m_socket = -1;
        }
    }

    int ClientHandler::receiveMessage() {
        int size;

        size = recv(*m_socket, m_receiveBuffer, BUF - 1, 0);

            if (size == -1) {
                if (m_abortRequested) {
                    std::cerr << "recv error after aborted\n";
                } else {
                    std::cerr << "recv error";
                }
                return -1;
            }

            if (size == 0) {
                std::cerr << "Client closed remote socket\n";
                return 0;
            }

            //remove newline from client message
            if(m_receiveBuffer[size - 2] == '\r' && m_receiveBuffer[size -1] == '\n'){
                size -= 2;
            } else if(m_receiveBuffer[size - 1] == '\n'){
                size -= 1;
            }

            //null terminate message
            m_receiveBuffer[size] = '\0';

            return size;
    }

    std::string ClientHandler::removeCommand(std::string message, std::string command){
        int start_pos = message.find(command);
        //+1 to remove whitespace
        int end_pos = command.size() + 1;
        message.erase(start_pos, end_pos);
        return message;
    }

    void ClientHandler::makeDirSaveMessage(std::string user, std::string path, std::string message){
        std::string ID;
        //if dir does not exist, ID = 1
        //else get next id
        if(std::filesystem::is_directory(path) && std::filesystem::exists(path)){
           ID = getNextID(user, path); 
        }else{
            ID = std::to_string(1);
        }
        //if dir does not exit, creates dir + file
        //else only file
        std::filesystem::path filepath{path};
        filepath /= ID;
        std::filesystem::create_directories(filepath.parent_path());

        //creates file + writes in it
        std::ofstream ofs(filepath);
        ofs << message; 
        ofs.close();
    }

    std::string ClientHandler::getNextID(std::string user, std::string path){
        std::string filename;
        int highestNr = 1;
        std::vector<int> ids;
        
        for (const auto & entry : std::filesystem::directory_iterator(path)){
           filename = entry.path();
           filename.erase(0, path.size()+1);
           ids.push_back(stoi(filename));
        }
        if(ids.size() == 0){
            return std::to_string(1);
        }
        std::sort(ids.begin(), ids.end());

        highestNr = ids.at(ids.size()-1) + 1;
        std::string newID = std::to_string(highestNr);

        return newID;
    }

    void ClientHandler::handleSend(Request request) {
        

    }

    
}


