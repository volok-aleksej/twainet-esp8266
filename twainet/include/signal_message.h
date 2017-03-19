#ifndef SIGNAL_MESSAGE_H
#define SIGNAL_MESSAGE_H

#include "data_message.h"

template<class TMessage>
class SignalMessage : public NamedMessage, public TMessage
{
    static const char* messageName;
public:
	SignalMessage(const TMessage& msg)
	{
        *(TMessage*)this = msg;
	}

    virtual const char* GetName()const
    {
        return const_cast<char*>(GetMessageName());
    }
    
	static const char* GetMessageName()
	{
		return messageName;
	}
};

#endif/*SIGNAL_MESSAGE_H*/