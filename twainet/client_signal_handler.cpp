#include <stdio.h>
#include <time.h>
#include "client_signal_handler.h"
#include "client_module.h"
#include "thread_manager.h"
// #include "utils/logger.h"

ClientSignalHandler::ClientSignalHandler(ClientModule* module)
: m_module(module)
{
}

ClientSignalHandler::~ClientSignalHandler()
{
	removeReceiver();
}

void ClientSignalHandler::onAddClientServerConnector(const ConnectorMessage& msg)
{
	ClientServerConnector* conn = static_cast<ClientServerConnector*>(msg.m_conn);
	if(conn)
	{
		conn->SetUserName(m_module->m_userPassword.m_userName);
		conn->SetPassword(m_module->m_userPassword.m_password);
		m_module->ipcSubscribe(conn, this, SIGNAL_FUNC(this, ClientSignalHandler, LoginResultMessage, onLoginResult));
		m_module->ipcSubscribe(conn, this, SIGNAL_FUNC(this, ClientSignalHandler, ClientServerConnectedMessage, onConnected));
	}

	m_module->AddConnector(msg.m_conn);
}

void ClientSignalHandler::onErrorConnect(const ConnectErrorMessage& msg)
{
    m_module->OnConnectFailed(msg.m_moduleName);
}

void ClientSignalHandler::onLoginResult(const LoginResultMessage& msg)
{
	if(const_cast<LoginResultMessage&>(msg).GetMessage()->login_result == CLIENT_SERVER__RESULT_CODE__LOGIN_FAILURE)
	{
		m_module->OnAuthFailed();
		m_module->m_isStopConnect = true;
	}
	else if(m_module->m_ownSessionId.length() == 0)
	{
		m_module->m_ownSessionId = const_cast<LoginResultMessage&>(msg).GetMessage()->own_session_id;
	}
	else
	{
		m_module->m_manager.StopConnection(IPCObjectName(m_module->m_serverIPCName).GetModuleNameString());
	}
}

void ClientSignalHandler::onConnected(const ClientServerConnectedMessage& msg)
{
    m_module->OnServerConnected();
}
