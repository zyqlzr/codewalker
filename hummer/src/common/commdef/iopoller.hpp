/*****************************************************
 * Copyright zhengyang
 * FileName: iopoller.hpp 
 * Description: io event loop
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef IO_POLLER_H
#define IO_POLLER_H

#include "boost/unordered_map.hpp"
#include <list>
using std::list;

#include "Logger.h"

#include <string.h>
#include <stdint.h>

#include "event.hpp"
#include "pollerbase.hpp"

class CIoPoller
{
public:
	typedef boost::unordered_map<int, CIoEvent*> EVENTS;
	typedef list<CTimeEvent*> TIMES;
	CIoPoller();
	~CIoPoller();

	int Init(io_type_e type);
	void UnInit();
	int AddRead(int sock, CIoEvent* io);
	int AddWrite(int sock, CIoEvent* io);
	int DelRead(int sock, CIoEvent* io);
	int DelWrite(int sock, CIoEvent* io);
	int AddTime(CTimeEvent* time);
	int DelTime(CTimeEvent* time);
	void Event();

	size_t EventNum()
	{
		return m_events.size();
	}

	size_t TimeNum()
	{
		return m_TimeList.size();
	}
private:
	int InitIo(io_poller_t ** pAction, io_type_e type);
	bool ComputeNearTime(struct timeval& time);
	void ProcessTime();
private:
	static const int DEF_HASH_BUCKET;
	static const int MAX_FD_READY;
private:
	void * m_handle;
	io_poller_t * m_Action;
	//sokcet-event map
	EVENTS m_events;
	//the set of the ready socket
	result_t m_results;

	size_t m_TimeEventCount;
	TIMES m_TimeList;
};

#endif

