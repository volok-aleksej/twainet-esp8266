#include "ipc_object_name.h"
//#include "utils/utils.h"

IPCObjectName::IPCObjectName(const String& moduleName, const String& hostName, const String& connId)
{
	m_moduleName = moduleName;
	m_hostName = hostName;
	m_connId = connId;
	m_moduleNameString = GetModuleNameString();
}

IPCObjectName::IPCObjectName(const Ipc__IPCName& ipcName)
: m_moduleName(ipcName.module_name), m_hostName(ipcName.host_name), m_connId(ipcName.conn_id)
{
	m_moduleNameString = GetModuleNameString();
}

IPCObjectName::~IPCObjectName()
{
}

bool IPCObjectName::operator == (const IPCObjectName& object) const
{
    return m_moduleName == object.GetModuleName() &&
        m_hostName == object.GetHostName() &&
        m_connId == object.GetConnId();
}

bool IPCObjectName::operator < (const IPCObjectName& object) const
{
    if (m_moduleName < object.GetModuleName() ||
        m_moduleName == object.GetModuleName() && m_hostName < object.GetHostName() ||
        m_moduleName == object.GetModuleName() && m_hostName == object.GetHostName() && m_connId < object.GetConnId())
        return true;
    else
        return false;
}

bool IPCObjectName::operator == (const Ipc__IPCName& ipcName) const
{
	return	m_moduleName == ipcName.module_name &&
		m_hostName == ipcName.host_name &&
		m_connId == ipcName.conn_id;
}

bool IPCObjectName::operator < (const Ipc__IPCName& ipcName) const
{
	if (m_moduleName < ipcName.module_name ||
		m_moduleName == ipcName.module_name && m_hostName < ipcName.host_name ||
		m_moduleName == ipcName.module_name && m_hostName == ipcName.host_name && m_connId < ipcName.conn_id)
		return true;
	else
		return false;
}

void IPCObjectName::operator = (const Ipc__IPCName& object)
{
    m_moduleName = object.module_name;
    m_hostName = object.host_name;
    m_connId = object.conn_id;
}

String IPCObjectName::GetModuleNameString() const
{
	String moduleNameString = m_moduleName;
	if(m_hostName.length())
	{
		moduleNameString += ".";
		moduleNameString += m_hostName.c_str();
	}
	if(m_connId.length())
	{
		moduleNameString += ".";
		moduleNameString += m_connId.c_str();
	}

	return moduleNameString;
}

String IPCObjectName::GetModuleName() const
{
    return m_moduleName;
}

String IPCObjectName::GetHostName() const
{
    return m_hostName;
}

String IPCObjectName::GetConnId() const
{
    return m_connId;
}

void IPCObjectName::SetModuleName(const String& name)
{
    m_moduleName = name;
}

void IPCObjectName::SetHostName(const String& name)
{
    m_hostName = name;
}

void IPCObjectName::SetConnId(const String& id)
{
    m_connId = id;
}
    
IPCObjectName IPCObjectName::GetIPCName(const String& ipcName)
{
	IPCObjectName name;
    int iter = 0;
    int posBegin = 0, posEnd = 0;
    for(int i = 0; i < ipcName.length(); i++, posEnd++) {
        if(ipcName[i] == '.') {
            if(iter == 0) {
                name.SetModuleName(ipcName.substring(posBegin, posEnd));
            } else if(iter == 1) {
                name.SetHostName(ipcName.substring(posBegin, posEnd));
            } else if(iter == 2) {
                name.SetConnId(ipcName.substring(posBegin, posEnd));
            }
            iter++;
            posBegin = posEnd + 1;
        }
    }
    
    if(iter == 0) {
        name.SetModuleName(ipcName.substring(posBegin, posEnd));
    } else if(iter == 1) {
        name.SetHostName(ipcName.substring(posBegin, posEnd));
    } else if(iter == 2) {
        name.SetConnId(ipcName.substring(posBegin, posEnd));
    }
    
	return name;
}
