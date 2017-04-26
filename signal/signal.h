#ifndef SIGNAL_H
#define SIGNAL_H

#include "list.h"
#include "data_message.h"
#include "receiver_func.h"
#include "thread.h"

class SignalOwner;

class Signal
{
public:
	Signal(SignalOwner* owner);

	void onSignal(const NamedMessage& msg);
	void removeOwner();
protected:
	friend class SignalManager;
	bool CheckSignal();
	virtual ~Signal(void);
protected:
	friend class SignalReceiver;
	void subscribe(IReceiverFunc* obj);
	void unsubscribe(SignalReceiver* receiver);
private:
	bool onSignalFunc(const NamedMessage& msg, const IReceiverFunc* obj);
	bool unsubsribeFunc(const SignalReceiver* receiver, const IReceiverFunc* signal);
	bool unsubsribeReceiver(const IReceiverFunc* signal);
	twnstd::list<IReceiverFunc*> m_receiverFunctions;
	SignalOwner* m_owner;
};

#endif	// SIGNAL_H
