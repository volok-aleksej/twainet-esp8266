#include "managers_container.h"
#include <Arduino.h>

/****************************************************************************************************************/
/*                                             ManagersContainer                                                */
/****************************************************************************************************************/
ManagersContainer::ManagersContainer()
{
    Serial.println("Managers Container");
}

ManagersContainer::~ManagersContainer()
{
    for(twnstd::list<IManager*>::iterator it = m_managers.begin();
        it != m_managers.end(); ++it) {
        (*it)->ManagerStop();
        if((*it)->IsDestroyable()) {
            delete (*it);
        }
    }
}

void ManagersContainer::AddManager(IManager* manager)
{
    Serial.println("Add Manager");
	manager->ManagerStart();
	m_managers.insert(m_managers.end(), manager);
}

void ManagersContainer::RemoveManager(IManager* manager)
{
	manager->ManagerStop();
    for(twnstd::list<IManager*>::iterator it = m_managers.begin();
        it != m_managers.end(); ++it) {
        if(*it == manager) {
            m_managers.erase(it);
            break;
        }
    }
}

void ManagersContainer::CheckManagers()
{
    for(twnstd::list<IManager*>::iterator it = m_managers.begin();
        it != m_managers.end(); ++it) {
        (*it)->ManagerFunc();
        if((*it)->IsStop()) {
            (*it)->ManagerStop();
            if((*it)->IsDestroyable()) {
                delete (*it);
            }
        }
    }
}
