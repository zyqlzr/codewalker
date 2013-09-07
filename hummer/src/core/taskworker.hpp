#ifndef UTIL_TASK_WORKER_H
#define UTIL_TASK_WORKER_H

#include "taskq.hpp"
#include "signaler.hpp"
#include "iopoller.hpp"
#include "thread.hpp"


class CTaskWorker;

class CWorkTimer : public CTimeEvent
{
public:
	CWorkTimer();
	~CWorkTimer();

	void SetWorker(CTaskWorker* worker);
	int Time();
private:
	const CWorkTimer& operator=(const CWorkTimer&);
private:
	CTaskWorker* m_worker;
};

class CTaskWorker : public CIoEvent
{
public:
	CTaskWorker();
	~CTaskWorker();
	bool Init(size_t workerId);
	void UnInit();
	
	int PushTask(TaskPTR& task);

	int IoRead(int fd);
	int IoWrite(int fd);
	static void* IoEventLoop(void* arg);
	void Time();
	size_t WorkId()const;
private:
	static const size_t TIMER_VALUE;
	static const bool LOOP_EXIT;
	static const bool LOOP_KEEP; 
private:
	size_t m_WorkId;
	CIoPoller m_poller;		
	CTaskQueue m_queue;
	CSignal m_signal;
	CThread m_thread;
	CWorkTimer m_timer;
	bool m_exit;
	size_t m_TaskCount;
	size_t m_LastCount;
}; 


#endif

