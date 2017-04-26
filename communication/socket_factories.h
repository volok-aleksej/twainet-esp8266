#ifndef SOCKET_FACTORIES_H
#define SOCKET_FACTORIES_H

#include "socket_factory.h"
#include <string>

class TCPSocketFactory : public SocketFactory
{
public:
	TCPSocketFactory();
	virtual AnySocket* CreateSocket();
	virtual SocketFactory* Clone();
};
class TCPSecureSocketFactory : public SocketFactory
{
public:
	TCPSecureSocketFactory();
	virtual AnySocket* CreateSocket();
	virtual SocketFactory* Clone();
};

#endif/*SOCKET_FACTORIES_H*/