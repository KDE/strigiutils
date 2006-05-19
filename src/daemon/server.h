#ifndef SERVER_H
#define SERVER_H

class Server {
protected:
	virtual void listen() = 0;
public:
	virtual ~Server() {}
	void start();
	void stop();
};

#endif
