#include "header/Request.hpp"
#include "../share/protocol.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace twServer {
    twServer::Request::Request(std::istringstream request) {
        
        std::string line;

        //getting all parameters from buffer
        while(getline(request, line, '\n')){
            m_params.push_back(line);
        }

        // METHOD
        m_method = m_params.at(0);

        //parsing parameters sent for easier and cleaner access to information later on

        if(m_method == CMD_SEND) {

            //SEND receiver subject msg
            if(m_params.size() < 4){
                return;
            }

            //RECEIVER - control length
            m_receiver = m_params.at(1).substr(0, 8);

            //SUBJECT - control length
            m_subject = m_params.at(2).substr(0, 80);

            //MESSAGE BODY - read all from third param to .
            for(auto it = m_params.begin() + 3; *it != "."; it++) {
                m_body.append(*it + '\n');
            }

        } else if(m_method == CMD_LIST) {
            
            //LIST - does not need username as it is automatically set when logged in
            if(m_params.size() < 1){
                return; 
            }

        } else if(m_method == CMD_READ) {
            
            //READ msgNr
            if(m_params.size() < 2){
                return; 
            }

            m_msgnum = m_params.at(1);

        } else if(m_method == CMD_DEL) {
            
            //DEL msgNr
            if(m_params.size() < 2){
                return; 
            }

            m_msgnum = m_params.at(1);
            
        } else if(m_method == CMD_LOGIN){

            //LOGIN username pw
            if(m_params.size() < 3){
                return;
            }

            //control length of username
            m_username = m_params.at(1).substr(0, 8);

            m_password = m_params.at(2);
        }

        
    }

    std::string Request::toString() const {
        return m_method + '\n' + m_sender + '\n' + m_receiver + '\n' + m_subject + '\n' + m_body;
    }
}