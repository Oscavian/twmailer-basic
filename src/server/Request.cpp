#include "header/Request.hpp"
#include "../share/protocol.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

namespace twServer {
    twServer::Request::Request(std::istringstream request) {
        
        std::string line;

        while(getline(request, line, '\n')){
            m_params.push_back(line);
        }

        // METHOD
        m_method = m_params.at(0);


        if(m_method == CMD_SEND) {

            if(m_params.size() < 5){
                return;
            }

            //SENDER
            m_sender = m_params.at(1).substr(0, 8);

            //RECEIVER
            m_receiver = m_params.at(2).substr(0, 8);

            //SUBJECT
            m_subject = m_params.at(3).substr(0, 80);

            //MESSAGE BODY
            for(auto it = m_params.begin() + 4; *it != "."; it++) {
                m_body.append(*it + '\n');
            }

        } else if(m_method == CMD_LIST) {
            
            if(m_params.size() < 2){
                return; 
            }

            m_username = m_params.at(1).substr(0, 8);

        } else if(m_method == CMD_READ) {
            
            if(m_params.size() < 3){
                return; 
            }

            m_username = m_params.at(1).substr(0, 8);

            m_msgnum = m_params.at(2);

        } else if(m_method == CMD_DEL) {
            
            if(m_params.size() < 3){
                return; 
            }

            m_username = m_params.at(1).substr(0, 8);
            m_msgnum = m_params.at(2);
        }

        
    }

    std::string Request::toString() const {
        return m_method + '\n' + m_sender + '\n' + m_receiver + '\n' + m_subject + '\n' + m_body;
    }
}