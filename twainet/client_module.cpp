#include <stdio.h>
#include <time.h>
#include "client_module.h"
#include "connect_thread.h"
#include "socket_factories.h"
#include "ipc_connector_factory.h"
#include "thread_manager.h"
#include "logger.h"

const String ClientModule::m_serverIPCName = "Server";
const String ClientModule::m_clientIPCName = "Client";

ClientModule::ClientModule(const IPCObjectName& ipcName, ConnectorFactory* factory)
: IPCModule(ipcName, factory)
, m_isStopConnect(true), m_signalHandler(this)
, m_connectThread(0)
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
	address.m_connectorFactory = new IPCConnectorFactory<ClientConnector>(m_clientIPCName);
	address.m_socketFactory = factory;
	address.m_ip = ip;
	address.m_port = port;
	m_connectThread = new ConnectThread(address);
	m_connectThread->addSubscriber(&m_signalHandler, SIGNAL_FUNC(&m_signalHandler, ClientSignalHandler, ConnectorMessage, onAddClientConnector));
	m_connectThread->addSubscriber(&m_signalHandler, SIGNAL_FUNC(&m_signalHandler, ClientSignalHandler, ConnectErrorMessage, onErrorConnect));
	m_connectThread->StartThread();
}

void ClientModule::OnConnectFailed(const String& moduleName)
{
    IPCObjectName ipcModuleName = IPCObjectName::GetIPCName(moduleName);
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
        LOG_INFO("Client %s disconnected", m_ownSessionId.c_str());
		m_ownSessionId = "";
        Connect(m_ip, m_port);
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
	LOG_INFO("Client %s connected", m_ownSessionId.c_str());
}

void ClientModule::OnAuthFailed()
{
	LOG_INFO("Client authorization failed");
}
	
void ClientModule::Disconnect()
{
	IPCObjectName ipcName(m_serverIPCName, m_ownSessionId);
	m_manager.StopConnection(ipcName.GetModuleNameString());
	m_isStopConnect = true;
    m_connectThread->StopThread();
    m_connectThread = 0;
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
