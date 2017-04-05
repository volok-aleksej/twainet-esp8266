#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include "include/singleton.h"
#include <WString.h>

#define LOG_WARNING(Format, ...) Logger::GetInstance().Log(Logger::LOG_WARNING, Format, ## __VA_ARGS__)
#define LOG_INFO(Format, ...) Logger::GetInstance().Log(Logger::LOG_INFO, Format, ## __VA_ARGS__)
#define LOG_ERROR(Format, ...) Logger::GetInstance().Log(Logger::LOG_ERROR, Format, ## __VA_ARGS__)

class Logger : public Singleton<Logger>
{
protected:
	friend class Singleton<Logger>;
	Logger();
public:
	enum TypeLog
	{
		LOG_WARNING,
		LOG_INFO,
		LOG_ERROR
	};

	~Logger();

	void Log(TypeLog type, const char* prototype, ...);
protected:
	String GetStringType(TypeLog type);
};

#endif/*LOGGER_H*/
