#include "logger.h"
#include <Arduino.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

static IConsole* console = 0;

void SetConsole(IConsole* console_)
{
    console = console_;
}

String GetStringType(TypeLog type)
{
	switch(type)
	{
	case LOG_WARNING:
		return "\033[1;33m";
	case LOG_ERROR:
		return "\033[1;31m";
	case LOG_INFO:
		return "\033[1;32m";
	}

	return "";
}

static char printdata[1024] = {0};

extern "C" void Log(TypeLog type, const char* prototype, ...)
{
	String strtype = GetStringType(type);
	strcpy(printdata, strtype.c_str());
	va_list argptr;
	va_start(argptr, prototype);
	vsnprintf(printdata + strtype.length(), 1024 - strtype.length(), prototype, argptr);
	va_end(argptr);
    strcpy(printdata + strlen(printdata), "\033[0m");
	if(console)
            console->Write(printdata);
        else 
            Serial.println(printdata);
}
