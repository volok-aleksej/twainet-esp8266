#ifndef IPC_CONNECTOR_H
#define IPC_CONNECTOR_H

#include <WString.h>
#include "connector.h"
#include "ipc_checker_thread.h"
#include "ipc_object_name.h"
#include "ipc_handler.h"
#include "signal_owner.h"
#include "signal_receiver.h"

class IPCConnector : public Connector, public SignalReceiver, public SignalOwner
{
    static char *baseAccessId;
public:
	IPCConnector(AnySocket* socket, const IPCObjectName& moduleName);
	virtual ~IPCConnector();
    
    virtual void SubscribeModule(::SignalOwner* owner);
        
	const IPCObjectName& GetModuleName() const;
	String GetAccessId();
protected:
 	friend class Signal;
 	friend class IPCHandler;
 	friend class IPCModule;
    void onIPCMessage(const IPCSignalMessage& msg);
 	
protected:
 	virtual void ThreadFunc();
 	virtual void OnStart();
 	virtual void OnStop();
	virtual bool SendData(char* data, int len);

	virtual bool SetModuleName(const IPCObjectName& moduleName);
	virtual void SetAccessId(const String& accessId);
	virtual void OnConnected();
	virtual void OnDisconnected();
	virtual void OnAddIPCObject(const String& moduleName);
	virtual IPCObjectName GetIPCName();
protected:
	IPCHandler m_handler;
	IPCCheckerThread *m_checker;
	bool m_bConnected;
private:
	IPCObjectName m_moduleName;
	String m_accessId;
	String m_rand;
};

#endif/*IPC_CONNECTOR_H*/