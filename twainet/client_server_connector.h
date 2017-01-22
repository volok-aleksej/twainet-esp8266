#ifndef CLIENT_CONNECTOR_H
#define CLIENT_CONNECTOR_H

#include "ipc_connector.h"
#include "client_server.pb-c.h"
#include "ipc.pb-c.h"

class ClientServerConnector;

typedef ProtoMessage<ClientServer__Login, ClientServerConnector> LoginMessage;
typedef ProtoMessage<ClientServer__LoginResult, ClientServerConnector> LoginResultMessage;

class ClientServerConnector : public IPCConnector
{
public:
	ClientServerConnector(AnySocket* socket, const IPCObjectName& moduleName);
	virtual ~ClientServerConnector();

	virtual void SubscribeConnector(const IPCConnector* connector);
	virtual void SubscribeModule(::SignalOwner* owner);
	virtual void OnConnected();

	void SetUserName(const String& userName);
	void SetPassword(const String& password);
protected:
	virtual void OnStart();
	virtual bool SetModuleName(const IPCObjectName& moduleName);
	virtual IPCObjectName GetIPCName();
protected:
	friend class Signal;
	void onIPCMessage(const IPCProtoMessage& msg);
	void onAddIPCObjectMessage(const AddIPCObjectMessage& msg);
	void onRemoveIPCObjectMessage(const RemoveIPCObjectMessage& msg);

protected:
	template<typename TMessage, typename THandler> friend class ProtoMessage;
	//Client Messages
	void onMessage(const ClientServer__LoginResult& msg);
	//Server Messages
	void onMessage(const ClientServer__Login& msg);
    
private:
	String m_userName;
	String m_password;
	String m_ownSessionId;
    IPCObjectName m_connId;
};

#endif/*CLIENT_CONNECTOR_H*/