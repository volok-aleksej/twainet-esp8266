#include <osapi.h>
#include <stdint.h>
#include <time.h>
#include "common_func.h"

typedef long long sint64_t;

#include "ssl/ssl_crypto.h"
#include "ssl/ssl_crypto_misc.h"

static const char size = 16;
static char uuidNumber[size];

String CreateGUID()
{
    get_random(size, (uint8_t*)uuidNumber);
    size_t len = 0;
    char data[size*2] = {0};
    for(int i = 0; i < size; i++) {
        os_sprintf(data, "%02X", uuidNumber[i]);
    }
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
