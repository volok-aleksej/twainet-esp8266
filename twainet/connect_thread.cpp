#include "connect_thread.h"
#include "connector_messages.h"
#include "common_func.h"
#include "connector.h"
#include "include/any_socket.h"
#include "include/socket_factory.h"
#include "include/connector_factory.h"

#include <Arduino.h>

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

    Serial.print("try connect to ");
    Serial.print(m_address.m_ip.c_str());
	Serial.print(":");
    Serial.println(m_address.m_port);
    
    if(m_socket->Connect(m_address.m_ip, m_address.m_port))
	{
        Serial.println("connect success");
		Connector* connector = m_address.m_connectorFactory->CreateConnector(m_socket);
        Serial.printf("Connector %d\n", connector);
		connector->SetId(m_address.m_id);
		connector->SetRemoteAddr(m_address.m_ip, m_address.m_port);
        Serial.println("signal connect success");
		m_socket = 0;
		ConnectorMessage msg(connector);
		onSignal(msg);
	}
	else
	{
        Serial.println("connect error");
		ConnectErrorMessage errMsg(m_address.m_moduleName, "", GetError());
		onSignal(errMsg);
		delete m_socket;
		m_socket = 0;
	}
	
    Serial.println("connect thread finish");
}

void ConnectThread::Stop()
{
	if (m_socket)
	{
		m_socket->Close();
	}
}
