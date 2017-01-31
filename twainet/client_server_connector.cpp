#include "client_server_connector.h"
#include "connector_messages.h"
#include "client_module.h"
#include "common_func.h"
#include "Arduino.h"

template<> const char* LoginMessage::messageName = client_server__login__descriptor.name;
template<> const char* LoginResultMessage::messageName = client_server__login_result__descriptor.name;

ClientServerConnector::ClientServerConnector(AnySocket* socket, const IPCObjectName& moduleName)
: IPCConnector(socket, moduleName)
{
	addMessage(new LoginMessage(this, client_server__login__descriptor));
	addMessage(new LoginResultMessage(this, client_server__login_result__descriptor));
}

ClientServerConnector::~ClientServerConnector()
{
	removeReceiver();
}

void ClientServerConnector::SubscribeConnector(const IPCConnector* connector)
{
	IPCConnector* ipcConn = const_cast<IPCConnector*>(connector);
	if(ipcConn)
	{
		ipcSubscribe(ipcConn, SIGNAL_FUNC(this, ClientServerConnector, IPCProtoMessage, onIPCMessage));
	}
	ClientServerConnector* conn = static_cast<ClientServerConnector*>(ipcConn);
	if(conn)
	{
		ipcSubscribe(conn, SIGNAL_FUNC(this, ClientServerConnector, AddIPCObjectMessage, onAddIPCObjectMessage));
		ipcSubscribe(ipcConn, SIGNAL_FUNC(this, ClientServerConnector, RemoveIPCObjectMessage, onRemoveIPCObjectMessage));
	}
}

void ClientServerConnector::SubscribeModule(::SignalOwner* owner)
{
	IPCConnector::SubscribeModule(owner);
}

void ClientServerConnector::SetUserName(const String& userName)
{
	m_userName = userName;
}

void ClientServerConnector::SetPassword(const String& password)
{
	m_password = password;
}

void ClientServerConnector::OnStart()
{
	m_checker = new IPCCheckerThread(this);
	LoginMessage loginMsg(this, client_server__login__descriptor);
	loginMsg.GetMessage()->name = (char*)m_userName.c_str();
	loginMsg.GetMessage()->password = (char*)m_password.c_str();
	toMessage(loginMsg);
}

bool ClientServerConnector::SetModuleName(const IPCObjectName& moduleName)
{
	return false;
}

void ClientServerConnector::OnConnected()
{
 	m_bConnected = true;
	ClientServerConnectedMessage msg(GetId());
	onSignal(msg);
}

IPCObjectName ClientServerConnector::GetIPCName()
{
	IPCObjectName name = IPCObjectName::GetIPCName(GetId());
	name.SetHostName(m_ownSessionId);
	return name;
}

void ClientServerConnector::onIPCMessage(const IPCProtoMessage& msg)
{
	IPCObjectName path(*const_cast<IPCProtoMessage&>(msg).GetMessage()->ipc_path[0]);
	if(path == GetModuleName())
	{
		IPCProtoMessage newMsg(&m_handler, ipc__ipcmessage__descriptor);
        newMsg.GetMessage()->has_message = const_cast<IPCProtoMessage&>(msg).GetMessage()->has_message;
        newMsg.GetMessage()->message = const_cast<IPCProtoMessage&>(msg).GetMessage()->message;
        newMsg.GetMessage()->message_name = const_cast<IPCProtoMessage&>(msg).GetMessage()->message_name;
        newMsg.GetMessage()->n_ipc_sender = const_cast<IPCProtoMessage&>(msg).GetMessage()->n_ipc_sender;
        newMsg.GetMessage()->ipc_sender = const_cast<IPCProtoMessage&>(msg).GetMessage()->ipc_sender;
 		IPCObjectName client = IPCObjectName::GetIPCName(m_id);
        newMsg.GetMessage()->n_ipc_path = const_cast<IPCProtoMessage&>(msg).GetMessage()->n_ipc_path;
        newMsg.GetMessage()->ipc_path = (Ipc__IPCName**)malloc(sizeof(Ipc__IPCName*)*newMsg.GetMessage()->n_ipc_path);
        IPCNameMessage names(&m_handler, ipc__ipcname__descriptor);
        names.GetMessage()->module_name = (char*)client.GetModuleName().c_str();
        names.GetMessage()->host_name = (char*)client.GetHostName().c_str();
        names.GetMessage()->conn_id = (char*)client.GetConnId().c_str();
        newMsg.GetMessage()->ipc_path[0] = names.GetMessage();
 		for(int i = 1; i < const_cast<IPCProtoMessage&>(msg).GetMessage()->n_ipc_path; i++)
 		{
            newMsg.GetMessage()->ipc_path[i] = const_cast<IPCProtoMessage&>(msg).GetMessage()->ipc_path[i];
 		}
 		toMessage(newMsg);
        free(newMsg.GetMessage()->ipc_path);
	}
	else if(path.GetModuleNameString() == m_id)
	{
		toMessage(msg);
	}
	  
}

void ClientServerConnector::onAddIPCObjectMessage(const AddIPCObjectMessage& msg)
{
	IPCObjectName idName = IPCObjectName::GetIPCName(m_id);
	if (idName.GetModuleName() == ClientModule::m_clientIPCName &&
        GetAccessId() == const_cast<AddIPCObjectMessage&>(msg).GetMessage()->access_id)
	{
		toMessage(msg);
	}
}

void ClientServerConnector::onRemoveIPCObjectMessage(const RemoveIPCObjectMessage& msg)
{
	IPCConnector::onRemoveIPCObjectMessage(msg);
}

void ClientServerConnector::onMessage(const ClientServer__LoginResult& msg)
{
	if(m_checker)
	{
        ManagersContainer::GetInstance().RemoveManager(m_checker);
		m_checker = 0;
	}
	

	LoginResultMessage lrMsg(this, client_server__login_result__descriptor);
    lrMsg.GetMessage()->login_result = msg.login_result;
    lrMsg.GetMessage()->own_session_id = msg.own_session_id;
	onSignal(lrMsg);
	if(msg.login_result == CLIENT_SERVER__RESULT_CODE__LOGIN_FAILURE)
	{
        Serial.print("Login failed: m_moduleName - ");
 		Serial.println(GetModuleName().GetModuleNameString().c_str());
		StopThread();
		return;
	}

	m_ownSessionId = msg.own_session_id;
	SetAccessId(m_userName);

	IPCObjectName name(GetId(), m_ownSessionId);
	SetId(name.GetModuleNameString());
	IPCConnector::SetModuleName(IPCObjectName(GetModuleName().GetModuleName(), m_ownSessionId));
	
    Serial.print("Login succesful: m_id - ");
    Serial.print(name.GetModuleNameString().c_str());
    Serial.print(", m_moduleName - ");
    Serial.println(GetModuleName().GetModuleNameString().c_str());

	ModuleNameMessage mnMsg(&m_handler, ipc__module_name__descriptor);
    IPCNameMessage ipcname(&m_handler, ipc__ipcname__descriptor);
	ipcname.GetMessage()->module_name = (char*)GetModuleName().GetModuleName().c_str();
    ipcname.GetMessage()->host_name = (char*)GetModuleName().GetHostName().c_str();
    ipcname.GetMessage()->conn_id = (char*)GetModuleName().GetConnId().c_str();
	mnMsg.GetMessage()->ip = "";
	mnMsg.GetMessage()->port = 0;
	mnMsg.GetMessage()->access_id = (char*)m_userName.c_str();
    mnMsg.GetMessage()->ipc_name = ipcname.GetMessage();
	toMessage(mnMsg);
}

void ClientServerConnector::onMessage(const ClientServer__Login& msg)
{
	if(m_checker)
	{
        ManagersContainer::GetInstance().RemoveManager(m_checker);
		m_checker = 0;
	}

	LoginMessage loginMsg(this, client_server__login__descriptor);
    loginMsg.GetMessage()->name = msg.name;
    loginMsg.GetMessage()->password = msg.password;
	onSignal(loginMsg);

    String Guid = CreateGUID();
	LoginResultMessage loginResultMsg(this, client_server__login_result__descriptor);
	loginResultMsg.GetMessage()->login_result = loginMsg.GetMessage()->login_result;
	loginResultMsg.GetMessage()->own_session_id = (char*)Guid.c_str();
	toMessage(loginResultMsg);

	if(loginMsg.GetMessage()->login_result == CLIENT_SERVER__RESULT_CODE__LOGIN_FAILURE)
	{
        Serial.print("Login failed: m_moduleName - ");
        Serial.println(GetModuleName().GetModuleNameString().c_str());
		StopThread();
		return;
	}

	m_ownSessionId = loginResultMsg.GetMessage()->own_session_id;
	SetAccessId(msg.name);
	
	IPCObjectName name(GetId(), m_ownSessionId);
	SetId(name.GetModuleNameString());
	IPCConnector::SetModuleName(IPCObjectName(GetModuleName().GetModuleName(), m_ownSessionId));
	
    Serial.print("Login succesful: m_id - ");
    Serial.print(name.GetModuleNameString().c_str());
    Serial.print(", m_moduleName - ");
    Serial.println(GetModuleName().GetModuleNameString().c_str());

    ModuleNameMessage mnMsg(&m_handler, ipc__module_name__descriptor);
    IPCNameMessage ipcName(&m_handler, ipc__ipcname__descriptor);
	ipcName.GetMessage()->module_name = (char*)GetModuleName().GetModuleName().c_str();
    ipcName.GetMessage()->host_name = (char*)GetModuleName().GetHostName().c_str();
    ipcName.GetMessage()->conn_id = (char*)GetModuleName().GetConnId().c_str();
    mnMsg.GetMessage()->ipc_name = ipcName.GetMessage();
	mnMsg.GetMessage()->ip = "";
	mnMsg.GetMessage()->port = 0;
	mnMsg.GetMessage()->access_id = msg.name;
	toMessage(mnMsg);
}