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
        sendMessage("Welcome to my server! Please enter your Commands...\n");

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
                    sendMessage(SERV_ERR);
                    return;
                }

                std::cout << "Message received: \n" << request.getMessage() << "from user: " << request.getSender() << " to " << request.getReceiver() << "\n\n";

                //save msg in doc
                try {
                    std::string path = m_mailDir + "/" + request.getReceiver();
                    makeDirSaveMessage(request.getReceiver(), path, request.getMessage());
                } catch(std::filesystem::filesystem_error & e){
                    std::cerr << e.what() << std::endl;
                }
                //send confirmation msg
                sendMessage(SERV_OK);

            } else if(request.getMethod() == CMD_LIST){
                std::cout << "List messages of user " << request.getUsername() << "\n\n";
                std::string path = m_mailDir + "/" + request.getUsername();
                listMessages(path);

            } else if(request.getMethod() == CMD_READ){
                std::cout << "Read message #" << request.getMsgnum() << " of user " << request.getUsername() << "\n\n";
                std::string path = m_mailDir + "/" + request.getUsername();
                readMessage(path, request.getMsgnum());

            } else if(request.getMethod() == CMD_DEL){
                std::cout << "Delete message #" << request.getMsgnum() << " of user " << request.getUsername() << "\n\n";
                std::string path = m_mailDir + "/" + request.getUsername();
                deleteMessage(path, request.getMsgnum());
            }else{
                if(request.getMethod() != CMD_QUIT){
                    sendMessage(SERV_ERR);

                } else {
                    sendMessage(SERV_OK);
                }
                
            }

        } while(strcmp(m_receiveBuffer, CMD_QUIT) != 0 && !m_abortRequested);

        abort();

    }

    bool ClientHandler::sendMessage(const char* buffer) {
        if(send(*m_socket, buffer, strlen(buffer), 0) == -1){
            throw std::runtime_error("Sending answer failed.");
        }
        return true;
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
    void ClientHandler::listMessages(std::string path){
        std::string filename;
        std::string files = "Messages: \n";
        long unsigned int size = files.size();

        if(std::filesystem::is_directory(path) && std::filesystem::exists(path)){
            for (const auto & entry : std::filesystem::directory_iterator(path)){
                filename = entry.path();
                filename.erase(0, path.size()+1);
                files += filename + "\n";
            }
        }
        if(files.size() > size){
            const char* m_files = files.c_str();
            sendMessage(m_files);
        }else{
            sendMessage("No Messages");
        }
    }

    void ClientHandler::readMessage(std::string path, std::string msgNum){
        int num = stoi(msgNum);
        std::string temp_filename = "";
        std::string m_filename = "";
        std::string content = "Message: \n";

        if(std::filesystem::is_directory(path) && std::filesystem::exists(path)){
            for (const auto & entry : std::filesystem::directory_iterator(path)){
                temp_filename = entry.path();
                temp_filename.erase(0, path.size()+1);
                if(stoi(temp_filename) == num){
                    std::cout << "Message found\n";
                    m_filename = temp_filename;
                    break;
                }
            }
        }
        //if file has been found
        if(m_filename.size() > 0){
            std::string temp;
            //open file
            std::ifstream readFile(path + "/" + m_filename);
            //read every line until end of file
            while(!readFile.eof()){
                while(getline(readFile, temp)){
                    content += " " + temp;
                }
            }
            readFile.close();
            const char* m_content = content.c_str();
            sendMessage(m_content);
        }else{
            //if file has not been found
            sendMessage("Message does not exist");
        }  
    }

    void ClientHandler::deleteMessage(std::string path, std::string msgNum){
        int num = stoi(msgNum);
        std::string temp_filename = "";
        std::string m_filename = "";

        if(std::filesystem::is_directory(path) && std::filesystem::exists(path)){
            for (const auto & entry : std::filesystem::directory_iterator(path)){
                temp_filename = entry.path();
                temp_filename.erase(0, path.size()+1);
                if(stoi(temp_filename) == num){
                    std::cout << "Message found\n";
                    m_filename = temp_filename;
                    break;
                }
            }
        }
        if(m_filename.size() > 0){
            const char* filepath = (path + "/" + m_filename).c_str();
            if(remove(filepath) != 0){
                std::cerr << "Error deleting message\n";
                sendMessage(SERV_ERR);
                return;
            }
            sendMessage(("Deleted message #" + m_filename).c_str());
        }else{
            //if file has not been found
            sendMessage("Message does not exist");
        }
    }
}


