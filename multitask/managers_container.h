#ifndef MANAGERS_CONTAINER_H
#define MANAGERS_CONTAINER_H

#include "list.h"
#include "singleton.h"

class IManager
{
public:
	IManager(){}
	virtual ~IManager(){}

	virtual void ManagerFunc() = 0;
	virtual void ManagerStart() = 0;
	virtual void ManagerStop() = 0;
    virtual bool IsStop() = 0;
    virtual bool IsDestroyable() = 0;
};

class ManagersContainer : public Singleton<ManagersContainer>
{
protected:
	friend class Singleton<ManagersContainer>;
	ManagersContainer();
	~ManagersContainer();

public:
	void AddManager(IManager* manager);
	void RemoveManager(IManager* manager);
    void CheckManagers();
private:
	twnstd::list<IManager*> m_managers;
};

#endif/*MANAGERS_CONTAINER_H*/