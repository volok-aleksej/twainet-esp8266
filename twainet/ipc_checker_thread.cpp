#include "ipc_checker_thread.h"
#include "ipc_connector.h"

IPCCheckerThread::IPCCheckerThread(IPCConnector* connector)
: m_connector(connector), m_count(50), m_isStop(false)
{
    ManagersContainer::GetInstance().AddManager(this);
}

IPCCheckerThread::~IPCCheckerThread()
{
}

void IPCCheckerThread::Stop()
{
    m_isStop = true;
    m_connector = 0;
}

void IPCCheckerThread::ManagerFunc()
{
	if(--m_count > 0)
	{
	      return;
	}

	if(!m_count && m_connector)
	{
		m_connector->StopThread();
	}
}
