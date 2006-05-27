#ifndef SERVER_H
#define SERVER_H

class Interface;
class Server {
protected:
    Interface* interface;
    virtual bool listen() = 0;
public:
    Server(Interface*i) :interface(i){}
    virtual ~Server() {}
    bool start();
    void stop();
};

#endif
