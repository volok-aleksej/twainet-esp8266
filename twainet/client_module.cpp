#include <stdio.h>
#include <time.h>
#include "client_module.h"
#include "connect_thread.h"
#include "socket_factories.h"
#include "include/ipc_connector_factory.h"
#include "thread_manager.h"
#include <Arduino.h>

const String ClientModule::m_serverIPCName = "Server";
const String ClientModule::m_clientIPCName = "Client";

ClientModule::ClientModule(const IPCObjectName& ipcName, ConnectorFactory* factory)
: IPCModule(ipcName, factory)
, m_isStopConnect(true), m_signalHandler(this)
{
}

ClientModule::~ClientModule()
{
	m_isStopConnect = true;
}

void ClientModule::Connect(const String& ip, int port)
{
	if(m_isExit)
	{
		return;
	}
	
	m_isStopConnect = false;
	m_ip = ip;
	m_port = port;

	SocketFactory* factory = new TCPSecureSocketFactory();
//    SocketFactory* factory = new TCPSocketFactory();

	ConnectAddress address;
	address.m_localIP = "";
	address.m_localPort = 0;
	address.m_moduleName = address.m_id = m_serverIPCName;
	address.m_connectorFactory = new IPCConnectorFactory<ClientServerConnector>(m_clientIPCName);
	address.m_socketFactory = factory;
	address.m_ip = ip;
	address.m_port = port;
	ConnectThread* thread = new ConnectThread(address);
	thread->addSubscriber(&m_signalHandler, SIGNAL_FUNC(&m_signalHandler, ClientSignalHandler, ConnectorMessage, onAddClientServerConnector));
	thread->addSubscriber(&m_signalHandler, SIGNAL_FUNC(&m_signalHandler, ClientSignalHandler, ConnectErrorMessage, onErrorConnect));
	thread->StartThread();
}

void ClientModule::OnConnectFailed(const String& moduleName)
{
    IPCObjectName ipcModuleName = IPCObjectName::GetIPCName(moduleName);
    Serial.printf("connected failed %s\n", ipcModuleName.GetModuleName().c_str());
    if (ipcModuleName.GetModuleName() == m_serverIPCName &&
        !m_isStopConnect && !m_isExit)
    {
        m_ownSessionId = "";
        ThreadManager& threadManager = ThreadManager::GetInstance();
        threadManager.DelayThread(threadManager.GetCurrentThreadId(), 1000);
        Connect(m_ip, m_port);
        return;
    }
    IPCModule::OnConnectFailed(moduleName);
}

void ClientModule::OnFireConnector(const String& moduleName)
{
	IPCObjectName ipcModuleName = IPCObjectName::GetIPCName(moduleName);
	if ((ipcModuleName.GetModuleName() == m_serverIPCName && ipcModuleName.GetHostName() == m_ownSessionId)&&
		!m_isStopConnect && !m_isExit)
	{
		m_ownSessionId = "";
		return;
	}
	IPCModule::OnFireConnector(moduleName);
}

bool ClientModule::CheckFireConnector(const String& moduleName)
{
	IPCObjectName ipcModuleName = IPCObjectName::GetIPCName(moduleName);
	return ipcModuleName.GetModuleName() == m_serverIPCName || ipcModuleName.GetModuleName() == m_clientIPCName;
}

void ClientModule::OnServerConnected()
{
	Serial.print("Client connected to server. sessionId: ");
    Serial.println(m_ownSessionId.c_str());
}

void ClientModule::OnAuthFailed()
{
	Serial.print("Client authorization failed. m_moduleName ");
    Serial.println(m_moduleName.GetModuleNameString().c_str());
}

void ClientModule::FillIPCObjectList(twnstd::list<IPCObject>& ipcList)
{
	twnstd::list<IPCObject>::iterator it;
	for(it = ipcList.begin(); it != ipcList.end(); ++it)
	{
		if (it->m_ipcName.GetModuleName() == m_clientIPCName &&
		    it->m_ipcName.GetModuleName() == m_serverIPCName)
		{
			it = ipcList.erase(it);
		}
	}
	
	IPCModule::FillIPCObjectList(ipcList);
}
	
void ClientModule::Disconnect()
{
    Serial.print("Try client disconnect. sessionId ");
    Serial.println(m_ownSessionId.c_str());
	IPCObjectName ipcName(m_serverIPCName, m_ownSessionId);
	m_manager.StopConnection(ipcName.GetModuleNameString());
	m_isStopConnect = true;
}

void ClientModule::SetUserName(const String& userName)
{
	m_userPassword.m_userName = userName;
}

void ClientModule::SetPassword(const String& password)
{
	m_userPassword.m_password = password;
}

const String& ClientModule::GetSessionId()
{
	return m_ownSessionId;
}
