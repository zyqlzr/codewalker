#include "taskmaster.hpp"
#include "Logger.h"

CTaskMaster::CTaskMaster(): m_curr(0), m_total(0){}

CTaskMaster::~CTaskMaster(){}

bool CTaskMaster::Init(size_t numOfWorker)
{
	CGuard guard(&m_lock);
	m_Workers = new(std::nothrow) CTaskWorker[numOfWorker];
	if (NULL == m_Workers)
	{
		return false;
	}

	for (size_t i = 0; i < numOfWorker; ++i)
	{
		if (!(m_Workers[i].Init(i)))
		{
			Rollback(i);
			return false;	
		}	
	}
	m_curr = 0;
	m_total = numOfWorker;
	LOG_TRACE("Task Master, setup "<<m_total<<" thread");
	return true;
}

void CTaskMaster::UnInit()
{
	if (NULL == m_Workers)
	{
		return;
	}
	
	CGuard guard(&m_lock);
	for (size_t i = 0; i < m_total; ++i)
	{
		m_Workers[i].UnInit();
	}
	delete [] m_Workers;
	m_Workers = NULL;
	m_curr = 0;
	m_total = 0;
	LOG_TRACE("Task Master, exit");
	return;
}

void CTaskMaster::Rollback(size_t n)
{
	if (NULL == m_Workers)
	{
		return;
	}

	for (size_t i = 0; i < n; ++i)
	{
		m_Workers[i].UnInit();
	}
	
	delete [] m_Workers;
	m_Workers = NULL;
	return;
}

int CTaskMaster::DoTask(TaskPTR& task)
{
	size_t pos = 0;
	{
		CGuard guard(&m_lock);
		pos = RandomRobin();
	}

	if (NULL == m_Workers || pos >= m_total)
	{
		return -1;
	}
	
	return m_Workers[pos].PushTask(task);
}

size_t CTaskMaster::RandomRobin()
{
	if (m_curr < (m_total - 1))
	{
		++m_curr;
	}
	else
	{
		m_curr = 0;
	}
	return m_curr;
}



