#include "secure_socket.h"
// #include "udt_socket.h"
#include "aes.h"

// #pragma warning(disable:4251)
// #include "udt.h"
// #pragma warning(default:4251)

#define RSA_DATA_SIZE 2048
#define SSL_HEADER_SIZE	8

char SecureSocket::m_ssl_header[] = "STARTTLS";

SecureSocket::SecureSocket()
	: m_bInit(false)
{
}

SecureSocket::~SecureSocket()
{
}

bool SecureSocket::PerformSslVerify()
{
	unsigned long bits = RSA_DATA_SIZE;
	RSA* rsaOwn = RSA_generate_key(bits, RSA_F4, NULL, NULL);
	RSA* rsaOther = RSA_new();
	bool bRet = true;

	//using variable
    int len = 0;
    unsigned char* data = 0;

    //send STARTTLS to and receive it from other side
    unsigned char sslHeader[SSL_HEADER_SIZE] = {0};
    if (!Send(m_ssl_header, SSL_HEADER_SIZE) ||
        !Recv((char*)sslHeader, SSL_HEADER_SIZE) ||
        memcmp(sslHeader, m_ssl_header, SSL_HEADER_SIZE) != 0)
    {
        goto end;
    }

    //send RSA public key
    len = i2d_RSAPublicKey(rsaOwn, (unsigned char**)&data);
    if (!Send((char*)&len, sizeof(int)) ||
        !Send((char*)data, len))
    {
        goto end;
    }
    delete data;

    //receive RSA public key
    if(!Recv((char*)&len, sizeof(int)))
    {
        goto end;
    }
    data = new unsigned char[len];
    if (!Recv((char*)data, len) ||
        !d2i_RSAPublicKey(&rsaOther, (const unsigned char**)&data, len))
    {
        goto end;
    }

    //Send session aes key
    if(AESGenerateKey(m_keyOwn, sizeof(m_keyOwn)) <= 0)
    {
        goto end;
    }
    len = RSA_size(rsaOther);
    data = new unsigned char[len];
    len = RSA_public_encrypt(sizeof(m_keyOwn), m_keyOwn, data, rsaOther, RSA_PKCS1_PADDING);
    if(!Send((char*)data, len))
    {
        goto end;
    }
    delete data;

    len = RSA_size(rsaOwn);
    data = new unsigned char[len];
    if (!Recv((char*)data, len) ||
        RSA_private_decrypt(len, data, m_keyOther, rsaOwn, RSA_PKCS1_PADDING) <= 0)
    {
        goto end;
    }
    delete data;
    goto finish;
end:
		bRet = false;
finish:

	if(bRet)
	{
		m_bInit = true;
	}

	RSA_free(rsaOwn);
	RSA_free(rsaOther);
	return true;
}

bool SecureSocket::Recv(char* data, int len)
{
	int recvlen = 0;
	while(!GetData(data, len))
	{
		if(!RecvData((char*)&recvlen, sizeof(int)))
		{
			return false;
		}

		int realDataLen = GetEncriptedDataLen(recvlen);
		unsigned char* recvdata = new unsigned char[realDataLen];
		if(!RecvData((char*)recvdata, realDataLen))
		{
			return false;
		}
		
		unsigned char* decriptedData = new unsigned char[recvlen];
		int decriptedLen = AESDecrypt(m_keyOwn, sizeof(m_keyOwn), recvdata, realDataLen, (byte*)decriptedData, recvlen);
		if(decriptedLen == -1)
		{
			return false;
		}

		delete recvdata;
		int newsize = (int)m_recvSize + decriptedLen;
        unsigned char* newdata = new unsigned char[newsize];
        memcpy(newdata, m_recvdata, m_recvSize);
		memcpy(newdata + newsize - decriptedLen, decriptedData, decriptedLen);
        if(m_recvdata) {
            delete m_recvdata;
        }
        m_recvdata = newdata;
        m_recvSize = newsize;
		delete decriptedData;
	}
	return true;
}

bool SecureSocket::GetData(char* data, int len)
{
	if((int)m_recvSize < len)
	{
		return false;
	}
	
	memcpy(data, m_recvdata, len);
	unsigned char *newdata = new unsigned char[m_recvSize - len];
	memcpy((char*)newdata, m_recvdata + len, m_recvSize - len);
    m_recvSize -= len;
    if(m_recvdata) {
        delete m_recvdata;
    }
    m_recvdata = newdata;
	return true;
}

bool SecureSocket::Send(char* data, int len)
{
	if(len > MAX_BUFFER_LEN)
	{
		int pos = 0, newlen = MAX_BUFFER_LEN;
		while(pos < len)
		{
			if(!Send(data + pos, newlen))
			{
				return false;
			}
			pos += newlen;
			(len - pos > MAX_BUFFER_LEN) ? (newlen = MAX_BUFFER_LEN) : (newlen = len - pos);
		}
		return true;
	}
	
	unsigned char* encriptedData = new unsigned char[MAX_BUFFER_LEN];
	int sendLen = AESEncrypt(m_keyOther, sizeof(m_keyOther), (byte*)data, len, encriptedData, MAX_BUFFER_LEN);
	if(sendLen <= 0)
	{
		return false;
	}

	unsigned char* senddata = new unsigned char[sendLen + sizeof(int)];
	memcpy(senddata + sizeof(int), encriptedData, sendLen);
	memcpy(senddata, &len, sizeof(int));
	sendLen += sizeof(int);
	delete encriptedData;
	bool bRet = SendData((char*)senddata, sendLen);
	delete senddata;
	return bRet;
}

/*********************************************************************************/
/*                              SecureUDTSocket                                  */
/*********************************************************************************/
// SecureUDTSocket::SecureUDTSocket(IPVersion ipv)
// 	: UDTSocket(ipv)
// {
// }
// 
// SecureUDTSocket::SecureUDTSocket(int socket, IPVersion ipv, bool isUdp)
// 	: UDTSocket(socket, ipv, isUdp)
// {
// 	if(!isUdp)
// 	{
// 		PerformSslVerify();
// 	}
// }
// 
// SecureUDTSocket::SecureUDTSocket(int udpSocket, int socket)
// 	: UDTSocket(udpSocket, socket)
// {
// 	PerformSslVerify();
// }
// 
// bool SecureUDTSocket::Send(char* data, int len)
// {
// 	if(m_bInit)
// 	{
// 		return SecureSocket::Send(data, len);
// 	}
// 	else
// 	{
// 		return SendData(data, len);
// 	}
// }
// 
// bool SecureUDTSocket::Recv(char* data, int len)
// {
// 	if(m_bInit)
// 	{
// 		return SecureSocket::Recv(data, len);
// 	}
// 	else
// 	{
// 		return RecvData(data, len);
// 	}
// }
// 
// bool SecureUDTSocket::Connect(const std::string& host, int port)
// {
// 	if(!UDTSocket::Connect(host, port))
// 	{
// 		return false;
// 	}
// 
// 	return PerformSslVerify();
// }
// 
// bool SecureUDTSocket::SendData(char* data, int len)
// {
// 	return UDTSocket::Send(data, len);
// }
// 
// bool SecureUDTSocket::RecvData(char* data, int len)
// {
// 	return UDTSocket::Recv(data, len);
// }


/*********************************************************************************/
/*                              SecureTCPSocket                                  */
/*********************************************************************************/
SecureTCPSocket::SecureTCPSocket()
{
}

SecureTCPSocket::SecureTCPSocket(int socket)
: TCPSocket(socket)
{
	PerformSslVerify();
}

bool SecureTCPSocket::Connect(const String& host, int port)
{
	if(!TCPSocket::Connect(host, port))
	{
		return false;
	}
	
	return PerformSslVerify();
}

bool SecureTCPSocket::Send(char* data, int len)
{
	if(m_bInit)
	{
		return SecureSocket::Send(data, len);
	}
	else
	{
		return SendData(data, len);
	}
}

bool SecureTCPSocket::Recv(char* data, int len)
{
	if(m_bInit)
	{
		return SecureSocket::Recv(data, len);
	}
	else
	{
		return RecvData(data, len);
	}
}

bool SecureTCPSocket::SendData(char* data, int len)
{
	return TCPSocket::Send(data, len);
}

bool SecureTCPSocket::RecvData(char* data, int len)
{
	return TCPSocket::Recv(data, len);
}
