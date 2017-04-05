#ifndef IPC_OBJECT_NAME_H
#define IPC_OBJECT_NAME_H

#include <WString.h>
#include "ipc.pb-c.h"

class IPCObjectName
{
public:
	IPCObjectName(const String& moduleName = "", const String& hostName = "", const String& connId = "");
    IPCObjectName(const IPCObjectName& ipcName);
	IPCObjectName(const Ipc__IPCName& ipcName);
	~IPCObjectName();

    bool operator == (const IPCObjectName& object) const;
    bool operator < (const IPCObjectName& object) const;
    
	bool operator == (const Ipc__IPCName& object) const;
	bool operator < (const Ipc__IPCName& object) const;
    
    void operator = (const Ipc__IPCName& object);
    void operator = (const IPCObjectName& ipcName);

	String GetModuleNameString() const;
    const String& GetModuleName() const;
    const String& GetHostName() const;
    const String& GetConnId() const;
    
    void SetModuleName(const String& name);
    void SetHostName(const String& name);
    void SetConnId(const String& id);
    
	static IPCObjectName GetIPCName(const String& ipcName);
protected:
	void ModuleNameString();
private:
	String m_moduleNameString;
    String m_moduleName;
    String m_hostName;
    String m_connId;
};

#endif/*IPC_OBJECT_NAME_H*/