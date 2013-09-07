#include "taskworker.hpp"
#include "Logger.h"

#include <assert.h>

/***************CWorkTimer***************/
CWorkTimer::CWorkTimer():m_worker(NULL){}

CWorkTimer::~CWorkTimer(){}

void CWorkTimer::SetWorker(CTaskWorker* worker)
{
	m_worker = worker;
}

int CWorkTimer::Time()
{
	//LOG_TRACE("Worker timer");
	if (NULL != m_worker)
	{
		m_worker->Time();
	}
	return 0;
}

/***************CTaskWorker definition*************/
CTaskWorker::CTaskWorker()
: m_WorkId(0), m_TaskCount(0), m_LastCount(0){}

CTaskWorker::~CTaskWorker(){}

bool CTaskWorker::Init(size_t workerId)
{
	if (!m_signal.Init() 
		|| 0 != m_poller.Init(IO_TYPE_EPOLL))
	{
		return false;
	}

	m_timer.SetTimerSec(TIMER_VALUE);
	m_timer.SetWorker(this);
	if (0 != m_poller.AddRead(m_signal.GetFd(), this)
		|| 0 != m_poller.AddTime(&m_timer))
	{
		return false;
	}
	
	m_WorkId = workerId;
	m_exit = LOOP_KEEP;
	return m_thread.Start(CTaskWorker::IoEventLoop, (void*)this);
}

void CTaskWorker::UnInit()
{
	m_exit = LOOP_EXIT;
	m_thread.Stop();
	m_signal.UnInit();
	m_poller.UnInit();
	return;
}

size_t CTaskWorker::WorkId()const
{
	return m_WorkId;
}

int CTaskWorker::PushTask(TaskPTR& task)
{
	if (0 == m_queue.produce(task))
	{
		m_signal.Send();
		return 0;
	}

	return -1;
}

int CTaskWorker::IoRead(int fd)
{
	if (fd != m_signal.GetFd())
	{
		assert(false);
	}
	m_signal.Recv();
	size_t taskNum = m_queue.consume(m_WorkId);
	//LOG_TRACE("consum task="<<taskNum);
	//m_TaskCount += taskNum;
	return 0;
}

int CTaskWorker::IoWrite(int fd)
{
	(void)fd;
	assert(false);
	return 0;
}

void CTaskWorker::Time()
{
	//size_t tps = (m_TaskCount - m_LastCount) / TIMER_VALUE;
	//LOG_TRACE("Task tps="<<tps<<",total task="<<m_TaskCount);
	//m_LastCount = m_TaskCount;
}

void* CTaskWorker::IoEventLoop(void* arg)
{
	CTaskWorker* worker = (CTaskWorker*)arg;
	if (NULL == worker)
	{
		return NULL;
	}

	while(worker->m_exit != CTaskWorker::LOOP_EXIT)
	{
		worker->m_poller.Event();
	}
	
	LOG_TRACE("Task worker exit, total task="<<worker->m_TaskCount);
	return NULL;
}

const size_t CTaskWorker::TIMER_VALUE = 1;
const bool CTaskWorker::LOOP_EXIT = true;
const bool CTaskWorker::LOOP_KEEP = false;
