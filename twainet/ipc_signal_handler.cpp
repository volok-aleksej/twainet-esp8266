#include <stdio.h>
#include <time.h>
#include "ipc_signal_handler.h"
#include "ipc_module.h"
#include "thread_manager.h"
#include "logger.h"

IPCSignalHandler::IPCSignalHandler(IPCModule* module)
: m_module(module)
{
}

IPCSignalHandler::~IPCSignalHandler()
{
	removeReceiver();
}

void IPCSignalHandler::onAddConnector(const ConnectorMessage& msg)
{
	m_module->AddConnector(msg.m_conn);
}

void IPCSignalHandler::onConnected(const ConnectedMessage& msg)
{
	m_module->OnConnected(msg.m_id);
}

void IPCSignalHandler::onIPCMessage(const IPCProtoMessage& msg)
{
 	twnstd::vector<String> path;
 	for(int i = 0; i < const_cast<IPCProtoMessage&>(msg).GetMessage()->n_ipc_sender; i++)
 	{
 		IPCObjectName sender(*const_cast<IPCProtoMessage&>(msg).GetMessage()->ipc_sender[i]);
 		path.push_back(sender.GetModuleNameString());
 	}
 	
 	m_module->OnMessage(const_cast<IPCProtoMessage&>(msg).GetMessage()->message_name, path,
                        (const char*)const_cast<IPCProtoMessage&>(msg).GetMessage()->message.data,
                        const_cast<IPCProtoMessage&>(msg).GetMessage()->message.len);
}

void IPCSignalHandler::onAddIPCObject(const AddIPCObjectMessage& msg)
{
	IPCModule::IPCObject object(*const_cast<AddIPCObjectMessage&>(msg).GetMessage()->ipc_name,
                                const_cast<AddIPCObjectMessage&>(msg).GetMessage()->ip,
                                const_cast<AddIPCObjectMessage&>(msg).GetMessage()->port,
                                const_cast<AddIPCObjectMessage&>(msg).GetMessage()->access_id);
	m_module->m_ipcObject.insert(m_module->m_ipcObject.end(), object);
	m_module->OnIPCObjectsChanged();    
    LOG_INFO("AddIPCObject: name-%s",
                 object.m_ipcName.GetModuleNameString().c_str());

}

void IPCSignalHandler::onRemoveIPCObject(const RemoveIPCObjectMessage& msg)
{
    IPCObjectName ipcName = IPCObjectName::GetIPCName(const_cast<RemoveIPCObjectMessage&>(msg).GetMessage()->ipc_name);
    for(twnstd::list<IPCModule::IPCObject>::iterator it = m_module->m_ipcObject.begin();
        it != m_module->m_ipcObject.end(); ++it)
    {
        if(it->m_ipcName == ipcName)
        {
            m_module->m_ipcObject.erase(it);
            break;
        }
    }
    
    m_module->OnIPCObjectsChanged();  
    LOG_INFO("RemoveIPCObject: name-%s",
                 ipcName.GetModuleNameString().c_str());
}

void IPCSignalHandler::onModuleName(const ModuleNameMessage& msg)
{
 	const_cast<ModuleNameMessage&>(msg).GetMessage()->is_exist = false;
}

void IPCSignalHandler::onDisconnected(const DisconnectedMessage& msg)
{
    if(!m_module->CheckFireConnector(msg.m_id))
	{
		return;
	}
    
	m_module->OnFireConnector(msg.m_id);
}
