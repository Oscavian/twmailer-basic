#ifndef TWMAILERBASIC_CLIENTBASE_H
#define TWMAILERBASIC_CLIENTBASE_H

class ClientBase {
    public:
        virtual void start() = 0;
        virtual void run() = 0;
        virtual void abort() = 0;

        virtual bool sendBuffer(const char *buffer) = 0;
        virtual int receiveBuffer() = 0;
};

#endif