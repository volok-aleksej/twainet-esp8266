#include "secure_socket.h"
#include "aes.h"
#define RSA_DATA_BITS 2048
#define RSA_DATA_BYTES RSA_DATA_BITS/8
#define SSL_HEADER_SIZE	8

const char* SecureSocket::startTls = "STARTTLS";
const char* SecureSocket::expecTls = "EXPECTLS";

static unsigned char rsaData[RSA_DATA_BYTES];

SecureSocket::SecureSocket()
	: m_bInit(false)
{
}

SecureSocket::~SecureSocket()
{
}

bool SecureSocket::PerformSslVerify()
{
    X509_CTX *ctx = 0;
    unsigned char* data = 0;
    int len = 0;
    bool bRet = false;
    //send STARTTLS to and receive it from other side
    unsigned char sslHeader[SSL_HEADER_SIZE] = {0};
    if (!Send((char*)expecTls, SSL_HEADER_SIZE) ||
        !Recv((char*)sslHeader, SSL_HEADER_SIZE) ||
        memcmp(sslHeader, startTls, SSL_HEADER_SIZE) != 0)
    {
        goto end;
    }
    
     //receive RSA public key
    if(!Recv((char*)&len, sizeof(int)))
    {
        goto end;
    }
    data = new unsigned char[len];
    if (!Recv((char*)data, len))
    {
         goto end;
    }

    if(x509_new((uint8_t*)data, &len, &ctx) != X509_OK)
    {
        goto end;
    }
    
    //Send session aes key
    get_random(sizeof(m_keyOwn), m_keyOwn);
    len = RSA_encrypt(ctx->rsa_ctx, m_keyOwn, sizeof(m_keyOwn), data, false);
    if(!Send((char*)data, len))
    {
        goto end;
    }
    delete data;

    if (!Recv((char*)data, len) ||
        RSA_encrypt(ctx->rsa_ctx, data, len, m_keyOther, false) <= 0)
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
	
	if(data)
    {
        delete data;
    }
    
    if(ctx) 
    {
        x509_free(ctx);
    }
	return bRet;
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
/*                              SecureTCPSocket                                  */
/*********************************************************************************/
SecureTCPSocket::SecureTCPSocket()
{
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
