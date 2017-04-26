#ifndef SOCKET_FACTORY_H
#define SOCKET_FACTORY_H

class AnySocket;

class SocketFactory
{
public:
	virtual ~SocketFactory(){}
	virtual AnySocket* CreateSocket() = 0;
	virtual SocketFactory* Clone() = 0;
};

#endif/*SOCKET_FACTORY_H*/