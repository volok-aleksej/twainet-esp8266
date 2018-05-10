#ifndef CLIENT_MODULE_H
#define CLIENT_MODULE_H

#include "client_signal_handler.h"
#include "ipc_module.h"

class ConnectThread;

class ClientModule : public IPCModule
{
	class UserPassword
	{
	public:
		UserPassword(String userName = "", String password = "")
			: m_userName(userName), m_password(password){}
		
		void operator = (const UserPassword& userPass)
		{
			m_userName = userPass.m_userName;
			m_password = userPass.m_password;
		}

		bool operator == (const UserPassword& userPass)
		{
			return m_userName == userPass.m_userName;
		}
		
		bool operator != (const UserPassword& userPass)
		{
			return m_userName != userPass.m_userName;
		}

		bool operator < (const UserPassword& userPass) const
		{
			return m_userName < userPass.m_userName;
		}

		String m_userName;
		String m_password;
	};
public:
	static const String m_serverIPCName;
	static const String m_clientIPCName;

public:
	ClientModule(const IPCObjectName& ipcName, ConnectorFactory* factory);
	virtual ~ClientModule();
public:
	void Connect(const String& ip, int port);
	void Disconnect();
	void SetUserName(const String& userName);
	void SetPassword(const String& password);

    const String& GetSessionId();
protected:
	friend class ClientSignalHandler;
    virtual void OnConnectFailed(const String& moduleName);
	virtual void OnFireConnector(const String& moduleName);
	virtual bool CheckFireConnector(const String& moduleName);
	virtual void OnServerConnected();
	virtual void OnAuthFailed();
	virtual void FillIPCObjectList(twnstd::list<IPCObject>& ipcList);
	
protected:
	//for client
	String m_ownSessionId;
	String m_ip;
	int m_port;

private:
	//for client
	bool m_isStopConnect;
	UserPassword m_userPassword;
	ClientSignalHandler m_signalHandler;
    ConnectThread* m_connectThread;
};


#endif/*CLIENT_MODULE_H*/