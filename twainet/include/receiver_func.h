#ifndef RECEIVER_FUNC_H
#define RECEIVER_FUNC_H

#include <Arduino.h>

class SignalReceiver;

class IReceiverFunc
{
public:
	virtual ~IReceiverFunc(){}
	virtual bool isSignal(const NamedMessage& msg) = 0;
	virtual void onSignal(const NamedMessage& msg) = 0;
	virtual SignalReceiver* GetReciever() const  = 0;
};

template<class TReciever>
class ReceiverFunc : public IReceiverFunc
{
	friend class Signal;
public:
	typedef void (TReciever::*SignalFunction)(const NamedMessage& msg);

	ReceiverFunc(TReciever* reciever, const char* typeSignal, SignalFunction func)
		: m_receiver(reciever), m_func(func), m_typeSignal(typeSignal)
	{
	}

protected:	
	bool isSignal(const NamedMessage& msg)
	{
		return strcmp(m_typeSignal, msg.GetName()) == 0;
	}

	void onSignal(const NamedMessage& msg)
	{
		if(strcmp(m_typeSignal, msg.GetName()) != 0)
		{
			return;
		}

		(m_receiver->*m_func)(msg);
	}

	SignalReceiver* GetReciever() const
	{
		return m_receiver;
	}

private:
	SignalFunction m_func;
	TReciever* m_receiver;
	const char* m_typeSignal;
};

#endif/*RECEIVER_FUNC_H*/