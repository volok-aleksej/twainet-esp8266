#include <time.h>

typedef long long sint64_t;

#include "socket_factories.h"
#include "secure_socket.h"

#ifndef INVALID_SOCKET
#	define INVALID_SOCKET -1
#endif/*INVALID_SOCKET*/

/*******************************************************************************************************/
/*                                          TCPSocketFactory                                           */
/*******************************************************************************************************/
TCPSocketFactory::TCPSocketFactory(){}

AnySocket* TCPSocketFactory::CreateSocket()
{
	return new TCPSocket();
}

SocketFactory* TCPSocketFactory::Clone()
{
	return new TCPSocketFactory();
}

/*******************************************************************************************************/
/*                                      TCPSecureSocketFactory                                         */
/*******************************************************************************************************/
TCPSecureSocketFactory::TCPSecureSocketFactory()
{}

AnySocket* TCPSecureSocketFactory::CreateSocket()
{
	return new SecureTCPSocket();
}

SocketFactory* TCPSecureSocketFactory::Clone()
{
	return new TCPSecureSocketFactory();
}
