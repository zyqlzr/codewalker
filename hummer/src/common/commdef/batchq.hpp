/*****************************************************
 * Copyright zhengyang
 * FileName: batchq.hpp
 * Description: batch queue
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef __COMMAND_QUEUE_H__
#define __COMMAND_QUEUE_H__
#include "mutex.hpp"

#include <stdint.h>
#include "Logger.h"

#include <list>
using std::list;

template<class T>
class BatchQueue
{
public:
	BatchQueue(): m_Control(0)
	{
		m_Queue[0].uCurr = 0; 
		m_Queue[1].uCurr= 0;
	}

	~BatchQueue(){};

	bool Produce(T& obj)
	{
		CGuard guard(&m_lock);
		size_t& curr = m_Queue[m_Control].uCurr;
		T* objs = m_Queue[m_Control].objs;
		if (curr >= BATCH_QUEUE_LEN)
		{
			return false;
		}

		objs[curr++] = obj;
		return true;
	}

	bool BatchConsume(list<T>& objs)
	{
		size_t consumePos = 0;
		{
			CGuard guard(&m_lock);
			if (0 != m_Queue[m_Control ^ 1].uCurr)
			{
				m_Queue[m_Control ^ 1].uCurr = 0;
			}

			consumePos = m_Control;
			m_Control = m_Control ^ 1;
		}

		for (size_t i = 0; i < m_Queue[consumePos].uCurr; i++)
		{
			objs.push_back(T());
			objs.back() = m_Queue[consumePos].objs[i];
			m_Queue[consumePos].objs[i].reset();
		}

		m_Queue[consumePos].uCurr = 0;
		return true;
	}

	enum
	{
		BATCH_QUEUE_LEN = 4000	
	};
private:
	CMutex m_lock;
	int32_t m_Control;

	struct
	{
		size_t uCurr;
		T objs[BATCH_QUEUE_LEN];
	}m_Queue[2];
};

#endif
