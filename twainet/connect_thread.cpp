#include "connect_thread.h"
#include "connector_messages.h"
#include "common_func.h"
#include "connector.h"
#include "any_socket.h"
#include "socket_factory.h"
#include "connector_factory.h"

ConnectThread::ConnectThread(const ConnectAddress& address)
: Thread(true), m_address(address), m_socket(0)
{
}

ConnectThread::~ConnectThread()
{
	if(m_socket)
	{
		delete m_socket;
		m_socket = 0;
	}
	delete m_address.m_connectorFactory;
	delete m_address.m_socketFactory;
}

void ConnectThread::ThreadFunc()
{
	m_socket = m_address.m_socketFactory->CreateSocket();
	m_socket->Bind(m_address.m_localIP, m_address.m_localPort);
    
    if(m_socket->Connect(m_address.m_ip, m_address.m_port))
	{
		Connector* connector = m_address.m_connectorFactory->CreateConnector(m_socket);
		connector->SetId(m_address.m_id);
		connector->SetRemoteAddr(m_address.m_ip, m_address.m_port);
		m_socket = 0;
		ConnectorMessage msg(connector);
		onSignal(msg);
	}
	else
	{
        delete m_socket;
        m_socket = 0;
		ConnectErrorMessage errMsg(m_address.m_moduleName, "", GetError());
		onSignal(errMsg);
	}
}

void ConnectThread::Stop()
{
	if (m_socket)
	{
		m_socket->Close();
	}
}
