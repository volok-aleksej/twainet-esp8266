#ifndef IPC_CHECKER_THREAD_H
#define IPC_CHECKER_THREAD_H

#include "managers_container.h"

class IPCConnector;

class IPCCheckerThread : public IManager
{
public:
	IPCCheckerThread(IPCConnector* connector);
	virtual ~IPCCheckerThread();
    
    void Stop();
protected:
	virtual void ManagerFunc();
	virtual void ManagerStop(){};
    virtual void ManagerStart(){};
    virtual bool IsStop(){ return m_isStop; }
    virtual bool IsDestroyable(){ return true; }
private:
	IPCConnector* m_connector;
	int m_count;
    bool m_isStop;
    bool isExit;
};

#endif/*IPC_CHECKER_THREAD_H*/