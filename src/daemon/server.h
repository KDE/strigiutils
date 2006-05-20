#ifndef SERVER_H
#define SERVER_H

class Interface;
class Server {
protected:
    Interface* interface;
    virtual void listen() = 0;
public:
    Server(Interface*i) :interface(i){}
    virtual ~Server() {}
    void start();
    void stop();
};

#endif
