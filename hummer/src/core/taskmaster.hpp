#ifndef UTIL_TASK_MASTER_H
#define UTIL_TASK_MASTER_H

#include "taskworker.hpp"
#include "mutex.hpp"

class CTaskMaster
{
public:
	CTaskMaster();
	~CTaskMaster();
	bool Init(size_t numOfWorker);
	void UnInit();

	int DoTask(TaskPTR& task);
private:
	void Rollback(size_t n);
	size_t RandomRobin();
private:
	size_t m_curr;
	size_t m_total;
	CMutex m_lock;
	CTaskWorker* m_Workers;	
};

#endif

