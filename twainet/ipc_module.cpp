#include <stdio.h>
#include <time.h>
#include "ipc_module.h"
#include "common_func.h"
#include "thread_manager.h"

/*******************************************************************************/
/*                               IPCObject                                     */
/*******************************************************************************/
IPCModule::IPCObject::IPCObject()
: m_port(0), m_ipcName("")
{
}

IPCModule::IPCObject::IPCObject(const IPCObject& object)
	: m_ipcName("")
{
	operator = (object);
}

IPCModule::IPCObject::IPCObject(const IPCObjectName& ipcName, const char* ip, int port, const char* accessId)
: m_ipcName(ipcName), m_port(port), m_ip(ip), m_accessId(accessId)
{
}

IPCModule::IPCObject::~IPCObject()
{
}

void IPCModule::IPCObject::operator=(const IPCObject& object)
{
	m_ipcName = object.m_ipcName;
	m_port = object.m_port;
	m_ip = object.m_ip;
	m_accessId = object.m_accessId;
}

bool IPCModule::IPCObject::operator != (const IPCObject& object)
{
	return !(operator == (object));
}

bool IPCModule::IPCObject::operator < (const IPCObject& object) const
{
	return m_ipcName < object.m_ipcName;
}

bool IPCModule::IPCObject::operator==(const IPCObject& object)
{
	return m_ipcName == object.m_ipcName;
}

/*******************************************************************************/
/*                               IPCModule                                     */
/*******************************************************************************/
IPCModule::IPCModule(const IPCObjectName& moduleName, ConnectorFactory* factory)
: m_moduleName(moduleName), m_factory(factory)
, m_ipcSignalHandler(this), m_isExit(false)
{
	m_manager.addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, DisconnectedMessage, onDisconnected));
}

IPCModule::~IPCModule()
{
	m_isExit = true;
}

void IPCModule::DisconnectModule(const IPCObjectName& moduleName)
{
	IPCObjectName module(moduleName);
	m_manager.StopConnection(module.GetModuleNameString());
}

void IPCModule::OnNewConnector(Connector* connector)
{
}

void IPCModule::OnConnected(const String& moduleName)
{
}

void IPCModule::OnFireConnector(const String& moduleName)
{
}

void IPCModule::OnConnectFailed(const String& moduleName)
{
}

void IPCModule::OnMessage(const String& messageName, const twnstd::vector<String>& path, const char* data, unsigned int lenData)
{
}

bool IPCModule::CheckFireConnector(const String& moduleName)
{
	return false;
}

void IPCModule::FillIPCObjectList(twnstd::list<IPCObject>& ipcList)
{
}

void IPCModule::OnIPCObjectsChanged()
{
}

void IPCModule::ipcSubscribe(IPCConnector* connector, SignalReceiver* receiver, IReceiverFunc* func)
{
	connector->addSubscriber(receiver, func);
}

void IPCModule::AddConnector(Connector* conn)
{
	IPCConnector* connector = static_cast<IPCConnector*>(conn);
	if(connector && !m_isExit)
	{
		connector->SetModuleName(m_moduleName);
		connector->SetConnectorId(CreateGUID());

		OnNewConnector(conn);

		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, AddIPCObjectMessage, onAddIPCObject));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ModuleNameMessage, onModuleName));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, RemoveIPCObjectMessage, onRemoveIPCObject));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, ConnectedMessage, onConnected));
		connector->addSubscriber(&m_ipcSignalHandler, SIGNAL_FUNC(&m_ipcSignalHandler, IPCSignalHandler, IPCProtoMessage, onIPCMessage));
		
		connector->SubscribeModule(dynamic_cast<SignalOwner*>(this));
		m_manager.AddConnection(conn);
	}
	else
	{
		delete conn;
	}
}

void IPCModule::SendMsg(const IPCSignalMessage& msg)
{
	onSignal(msg);
}

const IPCObjectName& IPCModule::GetModuleName()
{
	return m_moduleName;
}

bool IPCModule::IsExit()
{
	return m_isExit;
}

void IPCModule::Exit()
{
	m_isExit = true;
}

twnstd::vector<IPCObjectName> IPCModule::GetIPCObjects()
{
    twnstd::vector<IPCObjectName> retVal;
    for(twnstd::list<IPCObject>::iterator it = m_ipcObject.begin();
        it != m_ipcObject.end(); ++it)
    {
        retVal.push_back(it->m_ipcName);
    }
	return retVal;
}

twnstd::vector<IPCObjectName> IPCModule::GetTargetPath(const IPCObjectName& target)
{
    twnstd::vector<IPCObjectName> retpath;
    if(m_moduleName == target)
    {
      return retpath;
    }
    
    for(twnstd::list<IPCObject>::iterator it = m_modules.begin();
        it != m_modules.end(); ++it)
    {
        if (target == it->m_ipcName)
        {
            retpath.push_back(target);
            return retpath;
        }
    }
        
    return retpath;
}

twnstd::vector<IPCObjectName> IPCModule::GetConnectedModules()
{
    twnstd::vector<IPCObjectName> retVal;
    for(twnstd::list<IPCObject>::iterator it = m_modules.begin();
        it != m_modules.end(); ++it)
    {
        retVal.push_back(it->m_ipcName);
    }
    return retVal;
}
