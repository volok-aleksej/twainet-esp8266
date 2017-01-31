#include "ssl/ssl_crypto.h"
#include "ssl/ssl_crypto_misc.h"
#include "common_func.h"

static const char size = 16;
static char uuidNumber[size];
    
String CreateGUID()
{
    get_random(size, (uint8_t*)uuidNumber);
    int len = 0;
    base64_decode(uuidNumber, size, 0, &len);
    char* data = new char[len];
    base64_decode(uuidNumber, size, (uint8_t*)data, &len);
    return data;
}

String RandString(int size)
{
    String str;
	String GUID = CreateGUID();
	size_t charN = 0;
	char erazeChars[] = {'{', '}', '-'};
    const char* symb = GUID.c_str();
	while(*symb != 0)
	{
        if(*symb == '{' || *symb == '}' || *symb == '-')
        {
            continue;
        }
        str.concat(*symb);
	}

	return str;
}

int GetError()
{
    return 0;
}
