#include <Arduino.h>
#include "thread.h"
extern "C" {
#include <cont.h>
}

void Thread::sleep(unsigned long millisec)
{
    delay(millisec);
}

void Thread::ThreadFunc(Thread* thread)
{
    thread->OnStart();
    thread->ThreadFunc();
    thread->OnStop();
}

Thread::Thread(bool isDestroyable)
: m_threadId(0), m_state(ThreadDescription::State::ABSENT), m_destroyable(isDestroyable)
{
}

    
Thread::~Thread()
{
}

bool Thread::StartThread()
{
    ThreadManager::GetInstance().AddThread(this);
    if(m_threadId) {
        ets_post(m_threadId, m_threadId, 0);
    }
}

bool Thread::IsDestroyable() const
{
    return m_destroyable;
}
    
bool Thread::IsStopped() const
{
	return m_state == ThreadDescription::STOPPED;
}

bool Thread::IsAbsent() const
{
    return m_state == ThreadDescription::ABSENT;
}

bool Thread::IsWaiting() const
{
    return m_state == ThreadDescription::WAITING;
}

bool Thread::IsStop() const
{
    return m_state == ThreadDescription::STOP_PENDING;
}

bool Thread::IsRunning() const
{
    return m_state == ThreadDescription::RUNNING;
}

bool Thread::IsSuspend() const
{
    return m_state == ThreadDescription::SUSPENDED;
}

void Thread::StopThread()
{
	if(!IsStopped())
	{
        ThreadManager::GetInstance().RemoveThread(this);
	}
}
