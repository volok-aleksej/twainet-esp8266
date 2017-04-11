#include "connector_manager.h"
#include "connector_messages.h"
#include "connector.h"
#include "logger.h"

ConnectorManager::ConnectorManager()
{
    ManagersContainer::GetInstance().AddManager(this);
}

ConnectorManager::~ConnectorManager()
{
    ManagersContainer::GetInstance().RemoveManager(this);
}

void ConnectorManager::AddConnection(Connector* conn)
{
    for(twnstd::list<Connector*>::iterator it = m_connectors.begin();
        it != m_connectors.end(); ++it) {
        (*it)->onNewConnector(conn);
    }
	conn->StartThread();
	m_connectors.insert(m_connectors.begin(), conn);
}

void ConnectorManager::StopConnection(const String& moduleName)
{
    for(twnstd::list<Connector*>::iterator it = m_connectors.begin();
        it != m_connectors.end(); ++it) {
        if((*it)->GetId() == moduleName) {
            DisconnectedMessage msg((*it)->GetId(), (*it)->GetConnectorId());
            onSignal(msg);
            (*it)->StopThread();
            delete (*it);
            m_connectors.erase(it);
            break;
        }
    }
}

void ConnectorManager::StopAllConnection()
{
    for(twnstd::list<Connector*>::iterator it = m_connectors.begin();
        it != m_connectors.end();) {
        DisconnectedMessage msg((*it)->GetId(), (*it)->GetConnectorId());
        onSignal(msg);
        (*it)->StopThread();
        delete (*it);
        it = m_connectors.erase(it);
    }
}

void ConnectorManager::ManagerFunc()
{
    for(twnstd::list<Connector*>::iterator it = m_connectors.begin();
        it != m_connectors.end();) {
        if((*it)->IsAbsent()) {
            LOG_INFO("connector %04x", *it);
            DisconnectedMessage msg((*it)->GetId(), (*it)->GetConnectorId());
            onSignal(msg);
            delete (*it);
            it = m_connectors.erase(it);
        } else {
            ++it;
        }
    }
}

