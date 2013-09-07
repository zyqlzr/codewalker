/*****************************************************
 * Copyright zhengyang   
 * FileName: timedwatcher.hpp 
 * Description: message timedwatcher
 * Author: zhengyang
 * Date: 2013.08.03
 * *****************************************************/
#ifndef TIMED_WATCHER
#define TIMED_WATCHER

#include "msg.hpp"
#include "thread.hpp"
#include "tasker.hpp"
#include "datacache.hpp"

namespace hummer
{

class CTimedWatcher : public CMsgWatcher
{
public:
	CTimedWatcher();
	~CTimedWatcher();
	bool Start(size_t stepNum, uint32_t timeout_sec);
	void Stop();
	void Info(msg_stat_t& stat);
	void Watch(MID id, MsgPTR& msg);
	void Ack(MID id);
	void Fail(MID id);

	void GetRedoMsg(list<MsgPTR>& msgs, size_t num);
	size_t RedoNum();
	void Print();
private:
	static void* WatcherRoutine(void* arg);
private:
	bool m_ExitFlag;
	uint32_t m_Timeout;
	CThread m_Thread;
	CTimedCached m_WatchDatas;
	CFailedCached m_FailedDatas;
	boost::atomic_uint32_t m_Total;
	boost::atomic_uint32_t m_Ack;
	boost::atomic_uint32_t m_Failed;
};

};

#endif

