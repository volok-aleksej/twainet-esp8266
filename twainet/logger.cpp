#include "logger.h"
#include <Arduino.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::Log(TypeLog type, const char* prototype, ...)
{
	char printdata[1024] = {0};
	va_list argptr;
	va_start(argptr, prototype);
	vsnprintf(printdata, 1024, prototype, argptr);
	va_end(argptr);
	String strtype = GetStringType(type);
    Serial.print(strtype);
    Serial.println(printdata);
}

String Logger::GetStringType(TypeLog type)
{
	switch(type)
	{
	case LOG_WARNING:
		return " LOG_WARNING:\t";
	case LOG_ERROR:
		return " LOG_ERROR:\t\t";
	case LOG_INFO:
		return " LOG_INFO:\t\t";
	}

	return "";
}
