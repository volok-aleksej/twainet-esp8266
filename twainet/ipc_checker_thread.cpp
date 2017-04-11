#include "ipc_checker_thread.h"
#include "ipc_connector.h"

IPCCheckerThread::IPCCheckerThread(IPCConnector* connector)
: m_connector(connector), m_count(50)
{
    ManagersContainer::GetInstance().AddManager(this);
}

IPCCheckerThread::~IPCCheckerThread()
{
    ManagersContainer::GetInstance().RemoveManager(this);
}

void IPCCheckerThread::ManagerFunc()
{
	if(--m_count > 0)
	{
	      return;
	}

	if(!m_count)
	{
		m_connector->StopThread();
	}
}
