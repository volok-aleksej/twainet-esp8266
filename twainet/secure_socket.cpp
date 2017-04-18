#include "secure_socket.h"
#include "aes.h"
#include "aes.h"

#define RSA_DATA_BITS 2048
#define RSA_DATA_BYTES RSA_DATA_BITS/8
#define SSL_HEADER_SIZE	8

const char* SecureSocket::startTls = "STARTTLS";
const char* SecureSocket::expecTls = "EXPECTLS";

static unsigned char rsaData[RSA_DATA_BYTES];

err_t RSA_new(uint8_t* data, int len, RSA_CTX **ctx)
{
    int offset = 0, cert_size = 0;
    asn1_skip_obj(data, &cert_size, ASN1_SEQUENCE);
    if (asn1_next_obj(data, &offset, ASN1_SEQUENCE) < 0)
    {
        return ERR_VAL;
    }
    
    uint8_t *modules, *pub_exp;
    int mod_len = asn1_get_int(data, &offset, &modules);
    if(mod_len < 0)
    {
        return ERR_VAL;
    }
    
    int pub_len = asn1_get_int(data, &offset, &pub_exp);
    if(pub_len < 0)
    {
        return ERR_VAL;
    }
    
    RSA_pub_key_new(ctx, modules, mod_len, pub_exp, pub_len);
    
    return ERR_OK;
}

SecureSocket::SecureSocket()
	: m_bInit(false), m_recvdata(0), m_recvSize(0)
{
}

SecureSocket::~SecureSocket()
{
    if(m_recvdata)
    {
        delete m_recvdata;
    }
}
bool SecureSocket::PerformSslVerify()
{
    RSA_CTX *ctx = 0;
    unsigned char* data = 0;
    int len = 0;
    //send STARTTLS to and receive it from other side
    unsigned char sslHeader[SSL_HEADER_SIZE] = {0};
    if (!Send((char*)expecTls, SSL_HEADER_SIZE) ||
        !Recv((char*)sslHeader, SSL_HEADER_SIZE) ||
        memcmp(sslHeader, startTls, SSL_HEADER_SIZE) != 0)
    {
        goto PerformSslVerify_end;
    }
    
     //receive RSA public key
    if(!Recv((char*)&len, sizeof(int)))
    {
        goto PerformSslVerify_end;
    }
    data = new unsigned char[len];
    if (!data || !Recv((char*)data, len))
    {
         goto PerformSslVerify_end;
    }

    if(RSA_new((uint8_t*)data, len, &ctx) != ERR_OK)
    {
        goto PerformSslVerify_end;
    }
    
    //Send session aes key
    get_random(sizeof(m_key), m_key);
    len = RSA_encrypt(ctx, m_key, sizeof(m_key), data, false);
    if(!Send((char*)data, len))
    {
        goto PerformSslVerify_end;
    }
    
    m_bInit = true;
    goto PerformSslVerify_finish;

PerformSslVerify_end:
    m_bInit = false;

PerformSslVerify_finish:	
	if(data)
    {
        delete data;
    }
    
    if(ctx) 
    {
        RSA_free(ctx);
    }
	return m_bInit;
}

bool SecureSocket::Recv(char* data, int len)
{
    bool bRet;
    int recvlen = 0;
    unsigned char* recvdata = 0;
    unsigned char* decriptedData = 0;
 	while(!GetData(data, len))
 	{
 		if(!RecvData((char*)&recvlen, sizeof(int)))
 		{
            goto Recv_end;
 		}
 
 		int realDataLen = GetEncriptedDataLen(recvlen);
 		recvdata = new unsigned char[realDataLen];
 		if(!recvdata || !RecvData((char*)recvdata, realDataLen))
 		{
            goto Recv_end;
 		}
 		
 		decriptedData = new unsigned char[recvlen];
 		int decriptedLen = AESDecrypt(m_key, sizeof(m_key), recvdata, realDataLen, (byte*)decriptedData, recvlen);
 		if(decriptedLen <= 0)
 		{
            goto Recv_end;
 		}
         
 		int newsize = (int)m_recvSize + decriptedLen;
        unsigned char* newdata = new unsigned char[newsize];
        if(!newdata)
        {
            goto Recv_end;
        }
        os_memcpy(newdata, m_recvdata, m_recvSize);
		os_memcpy(newdata + m_recvSize, decriptedData, decriptedLen);
        if(m_recvdata)
        {
            delete m_recvdata;
        }
        m_recvdata = newdata;
        m_recvSize = newsize;
 		
        delete recvdata;
        delete decriptedData;
        recvdata = 0;
        decriptedData = 0;
 	}
 	
 	bRet = true;
    goto Recv_finish;
    
Recv_end:
    bRet = false;
    
Recv_finish:
    if(recvdata)
    {
        delete recvdata;
    }
    if(decriptedData)
    {
        delete decriptedData;
    }

	return bRet;
}

bool SecureSocket::GetData(char* data, int len)
{
	if(!m_recvdata || (int)m_recvSize < len)
	{
		return false;
	}
	
	os_memcpy(data, m_recvdata, len);
	unsigned char *newdata = new unsigned char[m_recvSize - len];
    if(!newdata)
    {
        return false;
    }
	os_memcpy((char*)newdata, m_recvdata + len, m_recvSize - len);
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
	
    bool bRet;
    unsigned char* senddata = 0;
	unsigned char* encriptedData = 0;
    int encriptedLen = GetEncriptedDataLen(len);
	encriptedData = new unsigned char[encriptedLen];
	int sendLen = AESEncrypt(m_key, sizeof(m_key), (byte*)data, len, encriptedData, encriptedLen);
	if(sendLen <= 0)
	{
        goto Send_end;
	}

	senddata = new unsigned char[sendLen + sizeof(int)];
    if(!senddata)
    {
        goto Send_end;
    }
	os_memcpy(senddata + sizeof(int), encriptedData, sendLen);
	os_memcpy(senddata, &len, sizeof(int));
	sendLen += sizeof(int);
	bRet = SendData((char*)senddata, sendLen);

Send_end:
    bRet = false;
    
Send_finish:
    if(senddata)
    {
        delete senddata;
    }
    if(encriptedData)
    {
        delete encriptedData;
    }

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
