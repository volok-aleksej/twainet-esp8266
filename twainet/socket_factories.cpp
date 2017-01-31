#include "socket_factories.h"
#include "secure_socket.h"
// #include "proxy_socket.h"
// #include "udp_socket.h"

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

AnySocket* TCPSocketFactory::CreateSocket(int socket)
{
	return new TCPSocket(socket);
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

AnySocket* TCPSecureSocketFactory::CreateSocket(int socket)
{
	return new SecureTCPSocket(socket);
}

SocketFactory* TCPSecureSocketFactory::Clone()
{
	return new TCPSecureSocketFactory();
}
