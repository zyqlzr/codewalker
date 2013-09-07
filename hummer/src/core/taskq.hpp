/*****************************************************
 * Copyright zhengyang
 * FileName: taskq.hpp
 * Description: the definition of task queue
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef __TASK_QUEUE_H__
#define __TASK_QUEUE_H__
#include "mutex.hpp"

#include <boost/shared_ptr.hpp>

class CTask
{
public:
	CTask(){}
	virtual ~CTask(){}
	virtual int Process(size_t workerId) = 0;
};

typedef boost::shared_ptr<CTask> TaskPTR;

class CTaskQueue
{
public:
	CTaskQueue(): m_Control(0)
	{
		m_Queue[0].uCurr = 0; 
		m_Queue[1].uCurr= 0;
	}

	~CTaskQueue(){};
	
	int produce(TaskPTR& task)
	{
		CGuard taskguard(&m_lock);
		
		if (NULL == task)
		{
			return -1;
		}

		size_t& curr = m_Queue[m_Control].uCurr;
		TaskPTR* tasks = m_Queue[m_Control].tasks;
		if (curr >= MAX_TASK_NUM)
		{
			return -1;
		}

		tasks[curr++] = task;
		task.reset();
		return 0;
	}

	size_t consume(size_t workId)
	{
		size_t consumeNum = 0;
		size_t consumePos = 0;
		{
			CGuard taskguard(&m_lock);
			if (0 != m_Queue[m_Control ^ 1].uCurr)
			{
				m_Queue[m_Control ^ 1].uCurr = 0;
			}
			
			consumePos = m_Control;
			m_Control = m_Control ^ 1;
		}

		for (size_t i = 0; i < m_Queue[consumePos].uCurr; i++)
		{
			m_Queue[consumePos].tasks[i]->Process(workId);
			m_Queue[consumePos].tasks[i].reset();
		}

		consumeNum = m_Queue[consumePos].uCurr;
		m_Queue[consumePos].uCurr = 0;
		return consumeNum;
	}
	static const size_t MAX_TASK_NUM = 2000;
private:
	CMutex m_lock;
	int32_t m_Control;
	
	struct
	{
		size_t uCurr;
		TaskPTR tasks[MAX_TASK_NUM];
	}m_Queue[2];
};

#endif
