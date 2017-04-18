#include "message_handler.h"
#include <osapi.h>
#include <malloc.h>

MessageHandler::MessageHandler()
{
}

MessageHandler::~MessageHandler(void)
{
}

void MessageHandler::addMessage(DataMessage* msg)
{
	m_messages.push_back(msg);
}

bool MessageHandler::onData(char* data, int len)
{
	int typeLen = 0;
	os_memcpy(&typeLen, data, sizeof(int));
	int headerLen = sizeof(int) + typeLen;
	if (typeLen
		&& len >= headerLen)
	{
		char* type = (char*)calloc(sizeof(unsigned char), typeLen + 1);
        if(!type)
        {
            return false;
        }
		os_memcpy(type, data + sizeof(unsigned int), typeLen);
		bool ret = onData(type, data + headerLen, len - headerLen);
        free(type);
        return ret;
	}
	return false;
}

bool MessageHandler::toMessage(const DataMessage& msg)
{
	char* data = 0;
	int len = 0;
	deserialize(msg, data, len);
	data = (char*)malloc(len);
    if(!data)
    {
        return false;
    }
	bool res = deserialize(msg, data, len) && SendData(data, len);
	free(data);
	return res;
}

bool MessageHandler::deserialize(const DataMessage& msg, char* data, int& len)
{
	const char* type = msg.GetName();
	int typeLen = (int)strlen(type);
	int headerLen = sizeof(int) + typeLen;
	int msgBodyLen = (len > headerLen ? (len - headerLen) : 0);
    
	bool res = const_cast<DataMessage&>(msg).deserialize(data + headerLen, msgBodyLen);
	len = headerLen + msgBodyLen;

	if(res && data)
	{
		os_memcpy(data, &typeLen, sizeof(unsigned int));
		os_memcpy(data + sizeof(unsigned int), type, typeLen);
	}
	
	return res;
}

bool MessageHandler::onData(char* type, char* data, int len)
{
    if(len < 0) {
        return false;
    }
    for (unsigned int i = 0; i < m_messages.length(); ++i)
    {
        if(strcmp(type, m_messages[i]->GetName()) == 0)
        {
            m_messages[i]->serialize(data, len);
            m_messages[i]->onMessage();
            return true;
        }
    }
	return false;
}