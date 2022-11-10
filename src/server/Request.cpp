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
                //SENDER
            m_sender = m_params.at(1);

            //cut sendername to 8 chars
            if(m_sender.length() > 8) {
                m_sender.erase(m_sender.begin() + 8, m_sender.end());
            }

            //RECEIVER
            m_receiver = m_params.at(2);

            if(m_receiver.length() > 8) {
                m_receiver.erase(m_receiver.begin() + 8, m_receiver.end());
            }

            //SUBJECT
            m_subject = m_params.at(3);

            //MESSAGE BODY
            for(auto it = m_params.begin() + 4; it != m_params.end(); it++) {
                m_body.append(*it + '\n');
            }

        } else if(m_method == CMD_LIST) {
            m_username = m_params.at(1);

        } else if(m_method == CMD_READ) {
            m_username = m_params.at(1).substr(0, 7);

            m_msgnum = m_params.at(2);

        } else if(m_method == CMD_DEL) {
            m_username = m_params.at(1);
            m_msgnum = m_params.at(2);
        }

        
    }

    std::string Request::toString() const {
        return m_method + '\n' + m_sender + '\n' + m_receiver + '\n' + m_subject + '\n' + m_body;
    }
}