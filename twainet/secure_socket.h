#ifndef SECURE_SOCKET_H
#define SECURE_SOCKET_H

#include "ssl/ssl_crypto.h"
#include "ssl/ssl_crypto_misc.h"
#include "tcp_socket.h"

class AnySocket;

class SecureSocket
{
public:
	SecureSocket();
	virtual ~SecureSocket();

	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
protected:
	bool PerformSslVerify();

	virtual bool SendData(char* data, int len) = 0;
	virtual bool RecvData(char* data, int len) = 0;

	bool GetData(char* data, int len);
protected:
	bool m_bInit;
	unsigned char m_keyOwn[32];
	unsigned char m_keyOther[32];
	unsigned char* m_recvdata;
    unsigned int m_recvSize;
public:
    static const char* startTls;
    static const char* expecTls;
};

class SecureTCPSocket : public SecureSocket, public TCPSocket
{
public:
	SecureTCPSocket();
	explicit SecureTCPSocket(int socket);

	virtual bool Connect(const String& host, int port);
	virtual bool Send(char* data, int len);
	virtual bool Recv(char* data, int len);
protected:
	virtual bool SendData(char* data, int len);
	virtual bool RecvData(char* data, int len);
};

#endif/*SECURE_SOCKET_H*/