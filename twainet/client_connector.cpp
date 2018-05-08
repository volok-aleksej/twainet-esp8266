#include "client_connector.h"
#include "connector_messages.h"
#include "client_module.h"
#include "common_func.h"

template<> const ProtobufCMessageDescriptor& LoginMessage::descriptor = client_server__login__descriptor;
template<> const ProtobufCMessageDescriptor& LoginResultMessage::descriptor = client_server__login_result__descriptor;

ClientConnector::ClientConnector(AnySocket* socket, const IPCObjectName& moduleName)
: IPCConnector(socket, moduleName)
{
	addMessage(new LoginMessage(this));
	addMessage(new LoginResultMessage(this));
}

ClientConnector::~ClientConnector()
{
	removeReceiver();
}

void ClientConnector::SubscribeModule(::SignalOwner* owner)
{
	IPCConnector::SubscribeModule(owner);
}

void ClientConnector::SetUserName(const String& userName)
{
	m_userName = userName;
}

void ClientConnector::SetPassword(const String& password)
{
	m_password = password;
}

void ClientConnector::OnStart()
{
	m_checker = new IPCCheckerThread(this);
	LoginMessage loginMsg;
	loginMsg.GetMessage()->name = (char*)m_userName.c_str();
	loginMsg.GetMessage()->password = (char*)m_password.c_str();
    loginMsg.GetMessage()->has_login_result = false;
	toMessage(loginMsg);
}

bool ClientConnector::SetModuleName(const IPCObjectName& moduleName)
{
	return false;
}

void ClientConnector::OnConnected()
{
 	m_bConnected = true;
	ClientServerConnectedMessage msg(GetId());
	onSignal(msg);
}

IPCObjectName ClientConnector::GetIPCName()
{
	IPCObjectName name = IPCObjectName::GetIPCName(GetId());
	name.SetHostName(m_ownSessionId);
	return name;
}

void ClientConnector::onMessage(const ClientServer__LoginResult& msg)
{
	LoginResultMessage lrMsg;
    lrMsg.GetMessage()->login_result = msg.login_result;
    lrMsg.GetMessage()->own_session_id = msg.own_session_id;
	onSignal(lrMsg);
	if(msg.login_result == CLIENT_SERVER__RESULT_CODE__LOGIN_FAILURE)
	{
		StopThread();
		return;
	}

	m_ownSessionId = msg.own_session_id;
	SetAccessId(m_userName);

	IPCObjectName name(GetId(), m_ownSessionId);
	SetId(name.GetModuleNameString());
	IPCConnector::SetModuleName(IPCObjectName(GetModuleName().GetModuleName(), m_ownSessionId));

	ModuleNameMessage mnMsg;
    IPCNameMessage ipcname;
	ipcname.GetMessage()->module_name = (char*)GetModuleName().GetModuleName().c_str();
    ipcname.GetMessage()->host_name = (char*)GetModuleName().GetHostName().c_str();
    ipcname.GetMessage()->conn_id = (char*)GetModuleName().GetConnId().c_str();
	mnMsg.GetMessage()->ip = "";
	mnMsg.GetMessage()->port = 0;
	mnMsg.GetMessage()->access_id = (char*)m_userName.c_str();
    mnMsg.GetMessage()->ipc_name = ipcname.GetMessage();
    mnMsg.GetMessage()->conn_id = "";
    mnMsg.GetMessage()->has_is_exist = false;
	toMessage(mnMsg);
}
