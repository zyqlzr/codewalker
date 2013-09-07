#include "timedwatcher.hpp"
#include "Logger.h"

hummer::CTimedWatcher::CTimedWatcher()
: m_ExitFlag(false), m_Timeout(0)
, m_Total(0), m_Ack(0), m_Failed(0){}

hummer::CTimedWatcher::~CTimedWatcher(){}

void hummer::CTimedWatcher::Info(msg_stat_t& stat)
{
	stat.uMsgTotal = m_Total.load();
	stat.uMsgOk = m_Ack.load();
	stat.uMsgErr = m_Failed.load();
}

void hummer::CTimedWatcher::Watch(MID id, MsgPTR& msg)
{
	m_Total.fetch_add(1);
	if (!m_WatchDatas.PushValue(id, msg))
	{
		LOG_ERROR("Watch err, id="<<id);
	}
}

void hummer::CTimedWatcher::Ack(MID id)
{
	m_Ack.fetch_add(1);
	m_WatchDatas.EraseValue(id);
}

void hummer::CTimedWatcher::Fail(MID id)
{
	m_Failed.fetch_add(1);
	MsgPTR msg;
	if (m_WatchDatas.EraseValue(id, msg))
	{
		m_FailedDatas.MsgFailed(id, msg);
	}
	LOG_TRACE("FAIL, redosize="<<m_FailedDatas.FailedNum()<<",msgsize="<<m_WatchDatas.MsgNum());
}

void hummer::CTimedWatcher::GetRedoMsg(list<MsgPTR>& msgs, size_t num)
{
	m_FailedDatas.GetFailedByNumber(msgs, num);
}

size_t hummer::CTimedWatcher::RedoNum()
{
	return m_FailedDatas.FailedNum();
}

bool hummer::CTimedWatcher::Start(size_t stepNum, uint32_t timeout_sec)
{
	if (!m_WatchDatas.Init(stepNum))
	{
		LOG_ERROR("init watcher datas failed");
		return false;
	}

	m_Timeout = timeout_sec;
	m_ExitFlag = true;
	return m_Thread.Start(hummer::CTimedWatcher::WatcherRoutine, (void*)this);
}

void hummer::CTimedWatcher::Stop()
{
	m_ExitFlag = false;
	m_Thread.Stop();
}

void hummer::CTimedWatcher::Print()
{
	char log[256] = {0};
	sprintf(log, "TimedWatcher, total=%u,ack=%u,fail=%u,w-cache=%u,f-cache=%u"
		, m_Total.load(), m_Ack.load(), m_Failed.load()
		, m_WatchDatas.MsgNum(), m_FailedDatas.FailedNum());
	LOG_TRACE(log);	
}

void* hummer::CTimedWatcher::WatcherRoutine(void* arg)
{
	CTimedWatcher* pWatcher = (CTimedWatcher*)arg;
	if (!pWatcher)
	{
		return NULL;
	}
	
	while(pWatcher->m_ExitFlag)
	{
		sleep(pWatcher->m_Timeout);
		DatasPTR timeoutdatas;
		pWatcher->m_WatchDatas.RoundOneStep(timeoutdatas);
		if (timeoutdatas.get())
		{
			LOG_TRACE("Watcher Timeout data:"<<timeoutdatas->size());
			pWatcher->m_FailedDatas.MsgsTimeout(timeoutdatas);
		}
		
		pWatcher->Print();
	}
	return NULL;
}


