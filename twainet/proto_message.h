#ifndef PROTO_MESSAGE_H
#define PROTO_MESSAGE_H

#include "data_message.h"
#include "protobuf-c.h"
#include <malloc.h>

class IPCHandler;

template<typename TMessage, typename THandler = IPCHandler>
class ProtoMessage : public DataMessage
{
    static const ProtobufCMessageDescriptor& descriptor;
public:
    ProtoMessage()
    : m_handler(0), unpacked(false)
    {
        message = (TMessage*)malloc(descriptor.sizeof_message);
        descriptor.message_init((ProtobufCMessage*)message);
    }

    ProtoMessage(THandler* handler)
    : m_handler(handler), unpacked(true), message(0)
    {
    }

	~ProtoMessage()
    {
        FreeMessage();
    }

	virtual void onMessage()
	{
        if(m_handler && message)
        {
            m_handler->onMessage(*message);
            FreeMessage();
        }
	}

	virtual bool serialize(char* data, int len)
	{
        if(!unpacked)
        {
            return false;
        }

        if(message)
        {
            FreeMessage();
        }

        message = (TMessage*)protobuf_c_message_unpack(&descriptor, 0, len, (const uint8_t*)data);
		return true;
	}

	virtual bool deserialize(char* data, int& len) const
	{
		int size = protobuf_c_message_get_packed_size((ProtobufCMessage*)message);
		if (size > len)
		{
			len = size;
			return false;
		}

		len = size;
		protobuf_c_message_pack((ProtobufCMessage*)message, (uint8_t*)data);
        return true;
	}

	virtual const char* GetName() const
	{
		return GetMessageName();
	}

	TMessage* GetMessage()
    {
        return message;
    }

    operator TMessage&()
    {
        return *message;
    }

    static const char* GetMessageName()
    {
        return descriptor.name;
    }

    void FreeMessage()
    {
        if(!message)
        {
            return;
        }

        if(unpacked)
        {
            protobuf_c_message_free_unpacked((ProtobufCMessage*)message, 0);
        }
        else
        {
            free(message);
        }

        message = 0;
    }

private:
	THandler* m_handler;
	TMessage* message;
    bool unpacked;
};

#endif/*PROTO_MESSAGE_H*/
