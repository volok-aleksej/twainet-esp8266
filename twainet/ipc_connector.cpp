#include <stdio.h>
#include <WString.h>

#include "ipc_connector.h"
#include "proto_message.h"
#include "ipc.pb-c.h"
#include "common_func.h"
#include "connector_messages.h"

#define MAX_DATA_LEN		1024*1024

char *IPCConnector::baseAccessId = "root";

IPCConnector::IPCConnector(AnySocket* socket, const IPCObjectName& moduleName)
: Connector(socket), m_handler(this), m_moduleName(moduleName)
, m_bConnected(false)
, m_rand(CreateGUID()), m_checker(0)
{
	addMessage(new ModuleNameMessage(&m_handler));
	addMessage(new AddIPCObjectMessage(&m_handler));
	addMessage(new RemoveIPCObjectMessage(&m_handler));
	addMessage(new IPCProtoMessage(&m_handler));
	addMessage(new IPCObjectListMessage(&m_handler));
	addMessage(new ModuleStateMessage(&m_handler));
	addMessage(new PingMessage(&m_handler));
}

IPCConnector::~IPCConnector()
{
	removeReceiver();
}

void IPCConnector::ThreadFunc()
{
    while(!IsStop()) {
        static int len;
        len = 0;
        if (!m_socket->Recv((char*)&len, sizeof(int))) {
            return;
        }

        if(len < 0 || len > MAX_DATA_LEN) {
            return;
        }

        char* data = (char*)malloc(len);
        if(!data) {
            return;
        }
        if(!m_socket->Recv(data, len)) {
            free(data);
            return;
        }
        
        onData(data, len);
        free(data);
    }
}

void IPCConnector::OnStart()
{
    m_checker = new IPCCheckerThread(this);
    SetAccessId(baseAccessId);

    Ipc__IPCName* ipcName = new Ipc__IPCName;
    ipcName->module_name = (char*)m_moduleName.GetModuleName().c_str();
    ipcName->host_name = (char*)m_moduleName.GetHostName().c_str();
    ipcName->conn_id = (char*)m_moduleName.GetConnId().c_str();
    
    String ip;
    int port;
    m_socket->GetIPPort(ip, port);
    
    ModuleNameMessage mnMsg;
    mnMsg.GetMessage()->ipc_name = ipcName;
    mnMsg.GetMessage()->ip = (char*)ip.c_str();
    mnMsg.GetMessage()->port = port;
    mnMsg.GetMessage()->access_id = (char*)GetAccessId().c_str();
    toMessage(mnMsg);
}

void IPCConnector::OnStop()
{
    if(m_bConnected)
 	{
 		OnDisconnected();
 	}
 
    RemoveIPCObjectMessage msg;
    msg.GetMessage()->ipc_name = (char*)m_id.c_str();
    onSignal(msg);
        
 	if(m_checker)
 	{
        m_checker->Stop();
 		m_checker = 0;
 	}
}

void IPCConnector::SubscribeModule(::SignalOwner* owner)
{
	owner->addSubscriber(this, SIGNAL_FUNC(this, IPCConnector, IPCSignalMessage, onIPCMessage));
}

const IPCObjectName& IPCConnector::GetModuleName() const
{
	return m_moduleName;
}

void IPCConnector::SetAccessId(const String& accessId)
{
	m_accessId = accessId;
}

String IPCConnector::GetAccessId()
{
	return m_accessId;
}

bool IPCConnector::SendData(char* data, int len)
{
	char* senddata = (char*)malloc(len + sizeof(int));
    if(!senddata)
    {
        return false;
    }
	os_memcpy(senddata + sizeof(int), data, len);
	os_memcpy(senddata, &len, sizeof(int));
	bool ret = Connector::SendData(senddata, len + sizeof(int));
    free(senddata);
    return ret;
}

void IPCConnector::onIPCMessage(const IPCSignalMessage& msg)
{
	IPCObjectName ipcName("");
	if(const_cast<IPCSignalMessage&>(msg).n_ipc_path != 0)
	{
		ipcName = *(const_cast<IPCSignalMessage&>(msg).ipc_path[0]);
	}

	if (const_cast<IPCSignalMessage&>(msg).n_ipc_path == 0 ||
		ipcName == IPCObjectName::GetIPCName(GetId()))
	{
        IPCProtoMessage ipmMsg;
        *ipmMsg.GetMessage() = msg;
		toMessage(ipmMsg);
	}
}

bool IPCConnector::SetModuleName(const IPCObjectName& moduleName)
{
	m_moduleName = moduleName;
	return true;
}

void IPCConnector::OnConnected()
{
 	m_bConnected = true;
 	ConnectedMessage msg(GetId());
 	onSignal(msg);
}

void IPCConnector::OnDisconnected()
{
	m_bConnected = false;
}

void IPCConnector::OnAddIPCObject(const String& moduleName)
{
}

IPCObjectName IPCConnector::GetIPCName()
{
	return IPCObjectName::GetIPCName(GetId());
}

