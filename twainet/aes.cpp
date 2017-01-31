#include "aes.h"
#include "ssl/ssl_crypto.h"
#include <string.h>

#define AES_BLOCK_SIZE 16

int GetEncriptedDataLen(int datalen)
{
	int rest = 0;
	if(datalen%AES_BLOCK_SIZE != 0)
	{
		rest = (datalen/AES_BLOCK_SIZE + 1)*AES_BLOCK_SIZE - datalen;
	}

	return datalen + rest;
}

int AESEncrypt(byte* key, int keylength,
			   byte* data, int datalen,
			   byte* encryptedData, int encryptedDataLen)
{
	
	int realDataLen = GetEncriptedDataLen(datalen);

	if(encryptedDataLen < realDataLen || !encryptedData)
	{
		return -1;
	}
	else if(realDataLen > MAX_BUFFER_LEN || !data || realDataLen == 0)
	{
		return -2;
	}
	else if(keylength != 32 || keylength != 16 || !key)
	{
		return -3;
	}
	
	AES_MODE mode = (keylength*8 == 256) ? AES_MODE_256 : AES_MODE_128;

	unsigned char tempdata[MAX_BUFFER_LEN] = {0};
	memset(tempdata, 0, MAX_BUFFER_LEN);
	memcpy(tempdata, data, datalen);

	unsigned char iv[16] = "123456789abcdef";
	AES_CTX aesKey;
	AES_set_key(&aesKey, key, iv, mode);
	AES_cbc_encrypt(&aesKey, (unsigned char*)tempdata, (unsigned char*)encryptedData, realDataLen);

	return realDataLen;
}

int AESDecrypt(byte* key, int keylength,
			   byte* data, int datalen,
			   byte* decryptedData, int decryptedDataLen)
{
	if(decryptedDataLen > datalen || !decryptedData)
	{
		return -1;
	}
	else if(datalen > MAX_BUFFER_LEN || !data || datalen%AES_BLOCK_SIZE != 0)
	{
		return -2;
	}
    else if(keylength != 32 || keylength != 16 || !key)
    {
        return -3;
    }
	else if (datalen == 0)
	{
		return 0;
	}

    AES_MODE mode = (keylength*8 == 256) ? AES_MODE_256 : AES_MODE_128;
	unsigned char iv[16] = "123456789abcdef";
	AES_CTX aesKey;
    AES_set_key(&aesKey, key, iv, mode);
	char decryptData[MAX_BUFFER_LEN] = {0};
	AES_cbc_encrypt(&aesKey, (unsigned char*)data, (unsigned char*)decryptData, datalen);
	memcpy(decryptedData, decryptData, decryptedDataLen);

	return decryptedDataLen;
}