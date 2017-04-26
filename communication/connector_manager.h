#ifndef CONNECTOR_MANAGER_H
#define CONNECTOR_MANAGER_H

#include <WString.h>

#include "list.h"
#include "signal.h"
#include "signal_owner.h"
#include "managers_container.h"

class Connector;

class ConnectorManager : public IManager, public SignalOwner
{
public:
	ConnectorManager();
	~ConnectorManager();

	void AddConnection(Connector* conn);
	void StopConnection(const String& moduleName);
	void StopAllConnection();
    
    virtual void ManagerFunc();
    virtual void ManagerStart(){}
    virtual void ManagerStop(){}
    virtual bool IsStop(){ return false;}
    virtual bool IsDestroyable(){ return false; }

private:
	twnstd::list<Connector*> m_connectors;
};

#endif/*CONNECTOR_MANAGER_H*/