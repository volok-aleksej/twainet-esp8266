#ifndef CONNECT_THREAD_H
#define CONNECT_THREAD_H

#include "include/address.h"
#include "thread.h"
#include "signal.h"
#include "signal_owner.h"

class AnySocket;

class ConnectThread : public SignalOwner, public Thread
{
public:
	ConnectThread(const ConnectAddress& address);
	~ConnectThread();

	virtual void Stop();
protected:
	virtual void ThreadFunc();

protected:
	ConnectAddress m_address;
	AnySocket* m_socket;
};

#endif/*CONNECT_THREAD_H*/