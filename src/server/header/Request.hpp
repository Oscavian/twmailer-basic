#ifndef TWMAILERBASIC_REQUEST_H
#define TWMAILERBASIC_REQUEST_H

#include <string>
#include <vector>

namespace twServer {


    class Request {

    public:
        Request(std::istringstream request);

        //methods for reading created requets object
        std::string getMethod() const { return m_method; };

        std::string getSender() const { return m_sender; };

        void setSender(std::string sender) { m_sender = sender; };

        std::string getReceiver() const { return m_receiver; };

        std::string getSubject() const { return m_subject; };

        std::string getMessage() const { return m_body; };

        std::string getUsername() const { return m_username; };

        void setUsername(std::string username) { m_username = username; };

        std::string getPassword() const { return m_password; };

        std::string getMsgnum() const { return m_msgnum; };

        std::string toString() const;


    private:

        std::string m_method;

        std::vector<std::string> m_params;

        std::string m_sender;

        std::string m_username;
        std::string m_password;
        std::string m_msgnum;

        std::string m_receiver;
        std::string m_subject;
        std::string m_body;


    };
}

#endif