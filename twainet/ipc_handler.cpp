#include "ipc_handler.h"
#include "ipc_connector.h"
#include "logger.h"

template<> const ProtobufCMessageDescriptor& IPCNameMessage::descriptor = ipc__ipcname__descriptor;
template<> const ProtobufCMessageDescriptor& IPCProtoMessage::descriptor = ipc__ipcmessage__descriptor;
template<> const ProtobufCMessageDescriptor& ModuleNameMessage::descriptor = ipc__module_name__descriptor;
template<> const ProtobufCMessageDescriptor& AddIPCObjectMessage::descriptor = ipc__add_ipcobject__descriptor;
template<> const ProtobufCMessageDescriptor& RemoveIPCObjectMessage::descriptor = ipc__remove_ipcobject__descriptor;
template<> const ProtobufCMessageDescriptor& IPCObjectListMessage::descriptor = ipc__ipcobject_list__descriptor;
template<> const ProtobufCMessageDescriptor& ModuleStateMessage::descriptor = ipc__module_state__descriptor;
template<> const ProtobufCMessageDescriptor& PingMessage::descriptor = ipc__ping__descriptor;

template<> const char* IPCSignalMessage::messageName = ipc__ipcmessage__descriptor.name;

IPCHandler::IPCHandler(IPCConnector* connector)
: m_connector(connector)
{
}

IPCHandler::~IPCHandler()
{
}
	
void IPCHandler::onMessage(const _Ipc__ModuleName& msg)
{
 	if(m_connector->m_checker)
 	{
        m_connector->m_checker->Stop();
 		m_connector->m_checker = 0;
 	}
	
 	IPCObjectName ipcName(*msg.ipc_name);
 	m_connector->SetId(ipcName.GetModuleNameString());

 	AddIPCObjectMessage aoMsg;
    aoMsg.GetMessage()->ip = msg.ip;
    aoMsg.GetMessage()->port = msg.port;
    aoMsg.GetMessage()->access_id = IPCConnector::baseAccessId;
    aoMsg.GetMessage()->ipc_name = msg.ipc_name;
 	m_connector->onSignal(aoMsg);

 	ModuleNameMessage mnMsg;
    mnMsg.GetMessage()->ip = msg.ip;
    mnMsg.GetMessage()->port = msg.port;
    mnMsg.GetMessage()->ipc_name = msg.ipc_name;
 	mnMsg.GetMessage()->is_exist = false;
 	mnMsg.GetMessage()->conn_id = (char*)m_connector->m_connectorId.c_str();
 	mnMsg.GetMessage()->access_id == IPCConnector::baseAccessId;
 	m_connector->onSignal(mnMsg);
 
 	ModuleStateMessage msMsg;
 	msMsg.GetMessage()->exist = mnMsg.GetMessage()->is_exist;
 	msMsg.GetMessage()->rndval = (char*)m_connector->m_rand.c_str();
 	m_connector->toMessage(msMsg);
  	m_connector->OnConnected();
}

void IPCHandler::onMessage(const _Ipc__ModuleState& msg)
{
 	if(msg.exist)
 	{
        LOG_INFO("Module exists: name-%s",
                 m_connector->m_moduleName.GetModuleNameString().c_str());
 		m_connector->StopThread();
 	}
}

void IPCHandler::onMessage(const _Ipc__AddIPCObject& msg)
{
 	AddIPCObjectMessage aoMsg;
	aoMsg.GetMessage()->access_id = msg.access_id;
	aoMsg.GetMessage()->ip = msg.ip;
	aoMsg.GetMessage()->port = msg.port;
	aoMsg.GetMessage()->ipc_name = msg.ipc_name;
 	m_connector->onSignal(aoMsg);
 
 	IPCObjectName ipcName(*msg.ipc_name);
 	m_connector->OnAddIPCObject(ipcName.GetModuleNameString());
}

void IPCHandler::onMessage(const _Ipc__RemoveIPCObject& msg)
{
 	RemoveIPCObjectMessage roMsg;
	roMsg.GetMessage()->ipc_name = msg.ipc_name;
 	m_connector->onSignal(roMsg);
 }

void IPCHandler::onMessage(const _Ipc__IPCMessage& msg)
{
 	if(!m_connector->m_bConnected)
 	{
 		return;
 	}
 	
 	Ipc__IPCName **namesSender = 0;
 
 	IPCProtoMessage newMsg;
    *newMsg.GetMessage() = msg;
 	if(msg.n_ipc_path)
 	{
        
        IPCObjectName ipcPath(*msg.ipc_path[0]);
        newMsg.GetMessage()->ipc_path = 0;
        newMsg.GetMessage()->n_ipc_path = 0;
         
        newMsg.GetMessage()->n_ipc_sender = msg.n_ipc_sender + 1;
        namesSender = (Ipc__IPCName**)malloc(sizeof(Ipc__IPCName*) * (newMsg.GetMessage()->n_ipc_sender));
        newMsg.GetMessage()->ipc_sender = namesSender;
        newMsg.GetMessage()->ipc_sender[msg.n_ipc_sender] = msg.ipc_path[0];
        for(int i = 0; i < msg.n_ipc_sender; i++)
        {
            newMsg.GetMessage()->ipc_sender[i] =  msg.ipc_sender[i];
        }
  	}
    m_connector->onSignal(newMsg);
    
    if(namesSender)
    {
        free(namesSender);
    }
}

void IPCHandler::onMessage(const _Ipc__IPCObjectList& msg)
{
 	for(int i = 0; i < msg.n_ipc_object; i++)
 	{
 		AddIPCObjectMessage aoMsg;
        aoMsg.GetMessage()->access_id = msg.ipc_object[i]->access_id;
        aoMsg.GetMessage()->ip = msg.ipc_object[i]->ip;
        aoMsg.GetMessage()->port = msg.ipc_object[i]->port;
        aoMsg.GetMessage()->ipc_name = msg.ipc_object[i]->ipc_name;
 		m_connector->onSignal(aoMsg);
 		
 		IPCObjectName ipcName(*msg.ipc_object[i]->ipc_name);
 		m_connector->OnAddIPCObject(ipcName.GetModuleNameString());
 	}
}
