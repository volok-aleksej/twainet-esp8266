#ifndef THREAD_H
#define THREAD_H

#include "thread_manager.h"

class Thread
{
public:
    Thread(bool destroyable);
    virtual ~Thread();
    
    friend class ThreadManager;
	bool StartThread();
    bool IsDestroyable() const;
    
	bool IsRunning() const;
    bool IsSuspend() const;
    bool IsWaiting() const;
	bool IsStopped() const;
    bool IsAbsent() const;
	bool IsStop() const;
    void StopThread();

	static void sleep(unsigned long millisec);
    static void ThreadFunc(Thread*);

protected:
	virtual void ThreadFunc() = 0;
	virtual void OnStop() = 0;
	virtual void OnStart() = 0;
	virtual void Stop() = 0;

private:
    unsigned int m_threadId;
    bool m_destroyable;
    ThreadDescription::State m_state;
};

#endif/*THREAD_H*/