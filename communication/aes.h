#include <WString.h>

typedef unsigned char byte;

#define MAX_BUFFER_LEN		512

//datalen must be less MAX_BUFFER_LEN
//return size encripted data
//return -3 - key is incorrect
//return -2 - incorrect input data
//return -1 - incorrect output data
//return 0 - open ssl internal error
int AESEncrypt(byte* key, int keylength,
			   byte* data, int datalen,
			   byte* encryptedData, int encryptedDataLen);

//datalen must be less MAX_BUFFER_LEN
//return size decripted data
//return -3 - key is incorrect
//return -2 - incorrect input data
//return -1 - incorrect output data
//return 0 - open ssl internal error
int AESDecrypt(byte* key, int keylength,
			   byte* data, int datalen,
			   byte* decryptedData, int decryptedDataLen);

int GetEncriptedDataLen(int datalen);