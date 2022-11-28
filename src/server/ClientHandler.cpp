#include "header/ClientHandler.hpp"
#include "../share/protocol.h"
#include "header/Request.hpp"
#include "header/Ldap.hpp"
#include <sstream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <bits/stdc++.h>

namespace twServer {
    ClientHandler::ClientHandler(std::string ipAddr, int *socket, std::string mailDir, int clientId){
        m_ipAddr = ipAddr;
        m_socket = socket;
        m_mailDir = mailDir;
        m_abortRequested = false;
        m_clientId = clientId;
    }   

    ClientHandler::~ClientHandler() {
        delete m_socket;
    }

    void ClientHandler::start() {
        run();
    }

    void ClientHandler::run() {
        std::string user = "";
        int size;
        sendBuffer("Welcome to my server! Please enter your Commands...\n");

        do {
            //receive data
            size = receiveBuffer();

            if(size == 0 || size == -1){
                break;
            }
            
            //request object for parsing all requests for easier readability
            Request request = Request(std::istringstream(std::string(m_receiveBuffer)));

            //setting path
            std::string path = m_mailDir + "/" + user;

            //method necessary
            if(request.getMethod().empty()){
                sendBuffer(SERV_ERR);
                return;
            }

            //here parsing client input
            //Options SEND, LIST, READ and DEL only available afetr logging in
            if(request.getMethod() == CMD_SEND){
                if(user.empty()){
                    sendBuffer(SERV_ERR);
                    std::cerr << "Error: User is not logged in.\n";
                    continue;
                }
                request.setSender(user);

                std::cout << "Message received: \n" << request.getMessage() << "from user: " << request.getSender() << " to " << request.getReceiver() << "\n\n";

                if(request.getReceiver().empty() || request.getSender().empty()) {
                    sendBuffer(SERV_ERR);
                    std::cerr << "Error: Request body incomplete - nothing saved.\n";
                    continue;
                }

                //save msg in doc
                try {
                    path = m_mailDir + "/" + request.getReceiver();
                    saveMessage(request, path);
                } catch(std::filesystem::filesystem_error & e){
                    std::cerr << e.what() << std::endl;
                }
                //send confirmation msg
                sendBuffer(SERV_OK);

            } else if(request.getMethod() == CMD_LIST){
                if(user.empty()){
                    sendBuffer(SERV_ERR);
                    std::cerr << "Error: User is not logged in.\n";
                    continue;
                }
                request.setUsername(user);
                
                if(request.getUsername().empty()){
                    std::cerr << "Error: Username not specified!\n";
                    sendBuffer(SERV_ERR);
                    continue;
                }

                std::cout << "List messages of user " << request.getUsername() << "\n\n";
                //sends list of message-subjects + numbers
                listMessages(path);

            } else if(request.getMethod() == CMD_READ){
                if(user.empty()){
                    sendBuffer(SERV_ERR);
                    std::cerr << "Error: User is not logged in.\n";
                    continue;
                }
                request.setUsername(user);
                if(request.getMsgnum().empty()){
                    sendBuffer(SERV_ERR);
                    std::cerr << "Error: Msgnum not specified!\n";
                    continue;
                }

                std::cout << "Read message #" << request.getMsgnum() << " of user " << request.getUsername() << "\n\n";
                //opens amd reads requested msg
                readMessage(path, request.getMsgnum());

            } else if(request.getMethod() == CMD_DEL) {
                if(user.empty()){
                    sendBuffer(SERV_ERR);
                    std::cerr << "Error: User is not logged in.\n";
                    continue;
                }
                request.setUsername(user);
                if(request.getMsgnum().empty()){
                    sendBuffer(SERV_ERR);
                    std::cerr << "Error: Msgnum not specified!\n";
                    continue;
                }
                std::cout << "Delete message #" << request.getMsgnum() << " of user " << request.getUsername() << "\n\n";
                //removes requested msg
                deleteMessage(path, request.getMsgnum());

            } else if(request.getMethod() == CMD_LOGIN){
                if(request.getUsername().empty()){
                    sendBuffer(SERV_ERR);
                    std::cerr << "Error: No username specified!\n";
                    continue;
                }
                if(request.getPassword().empty()){
                    sendBuffer(SERV_ERR);
                    std::cerr << "Error: No password specified!\n";
                }
                std::cout << "Trying to log in as user " << request.getUsername() << "\n\n";

                //sets up ldap connection for logging in
                Ldap ldap = Ldap(request.getUsername(), request.getPassword());
                //checks the typed in credentials
                int userExists = ldap.checkIfUserExists();

                if(userExists == 1){
                    //user does exist
                    sendBuffer(SERV_OK);
                    user = request.getUsername();
                } else{
                    sendBuffer(SERV_ERR);
                }

            } else {
                if(request.getMethod() != CMD_QUIT){
                    sendBuffer(SERV_ERR);

                } else {
                    sendBuffer(SERV_OK);
                }
                
            }

        } while(strcmp(m_receiveBuffer, CMD_QUIT) != 0 && !m_abortRequested);

        abort();

    }

    void ClientHandler::abort() {
        //releasing resources of the specific connection
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


    bool ClientHandler::sendBuffer(const char* buffer) {
        if(send(*m_socket, buffer, strlen(buffer), 0) == -1){
            throw std::runtime_error("Sending answer failed.");
        }
        return true;
    }


    int ClientHandler::receiveBuffer() {
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

    void ClientHandler::saveMessage(Request content, std::string path){
        std::string ID;
        //if dir does not exist, ID = 1
        //else get next id
        if(std::filesystem::is_directory(path) && std::filesystem::exists(path)){
           ID = getNextID(content.getReceiver(), path); 
        }else{
            ID = std::to_string(1);
        }
        //if dir does not exit, creates dir + file
        //else only file
        std::filesystem::path filepath{path};
        filepath /= ID;
        //does not throw error when directory already exists
        std::filesystem::create_directories(filepath.parent_path());

        //creates file + writes in it
        std::ofstream ofs(filepath);
        ofs << "Sender: " << content.getSender() << "\n"
            << "Receiver: " << content.getReceiver() << "\n"
            << "Subject: " << content.getSubject() << "\n"
            << "Message: \n" << content.getMessage() << "\n"
        ; 
        //file is closed
        ofs.close();
    }

    std::string ClientHandler::getNextID(std::string user, std::string path){
        std::string filename;
        int highestNr = 1;
        std::vector<int> ids;
        
        //iterates through directory and collects all filenames as ints in vector ids
        for (const auto & entry : std::filesystem::directory_iterator(path)){
           filename = entry.path();
           filename.erase(0, path.size()+1);
           ids.push_back(stoi(filename));
        }
        if(ids.size() == 0){
            //if there are no messages, return ID 1
            return std::to_string(1);
        }
        //sort ids by size
        std::sort(ids.begin(), ids.end());

        //get new hightest id by looking at last int in ids and increment it
        highestNr = ids.at(ids.size()-1) + 1;
        //return it as string for easier usage
        std::string newID = std::to_string(highestNr);

        return newID;
    }


    void ClientHandler::listMessages(std::string path){
        std::string filename;
        std::string response = "";
        size_t size = response.size();
        int cnt = 0; 
        //open directory and iterate through
        if(std::filesystem::is_directory(path) && std::filesystem::exists(path)){
            for (const auto & entry : std::filesystem::directory_iterator(path)){
                filename = entry.path();
                std::string temp;
                //open file
                std::ifstream readFile(filename);
                //read every line until subject
                while(!readFile.eof()){
                    while(getline(readFile, temp)){
                        //if Subject is found
                       if(temp.find("Subject") != std::string::npos){
                            filename.erase(0, path.size()+1);
                            //add subject + filename to response
                            response += temp + '\n';
                            response += " - MsgNr: " + filename + '\n';
                        }
                    }
                }
                readFile.close();
                cnt++;
            }
        }

        //add counter of responsed to start of string
        response = std::to_string(cnt) + "\n" + response;

        if(response.size() > size){
            //send collected response to client 
            sendBuffer(response.c_str());
        }else{
            //if no msg in path, send 0
            sendBuffer("0");
        }
    }

    void ClientHandler::readMessage(std::string path, std::string msgNum){
        int num = stoi(msgNum);
        std::string temp_filename = "";
        std::string m_filename = "";
        std::string content = "Message: \n";

        //open directory and iterate through
        if(std::filesystem::is_directory(path) && std::filesystem::exists(path)){
            for (const auto & entry : std::filesystem::directory_iterator(path)){
                temp_filename = entry.path();
                //get filename without path information for comparison with given filename
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
                    content += '\n' + temp;
                }
            }
            readFile.close();
            
            content = SERV_OK + '\n' + content;
            //send content of founf msg + OK to client
            sendBuffer(content.c_str());
        }else{
            //if file has not been found
            sendBuffer(SERV_ERR);
        }  
    }

    void ClientHandler::deleteMessage(std::string path, std::string msgNum){
        int num = stoi(msgNum);
        std::string temp_filename = "";
        std::string m_filename = "";

        //similar mechanic to read
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
            //get complete path of file and remove it
            const char* filepath = (path + "/" + m_filename).c_str();
            if(remove(filepath) != 0){
                std::cerr << "Error deleting message\n";
                sendBuffer(SERV_ERR);
                return;
            }
            sendBuffer(SERV_OK);
        }else{
            //if file has not been found
            sendBuffer(SERV_ERR);
        }
    }
}


