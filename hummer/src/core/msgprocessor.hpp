/*****************************************************
 * Copyright zhengyang   
 * FileName: msgprocessor.hpp 
 * Description: message queue 
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef MSG_PROCESSOR_HPP
#define MSG_PROCESSOR_HPP

#include "msg.hpp"
#include "signaler.hpp"
#include "batchq.hpp"
#include "iopoller.hpp"
#include "event.hpp"
#include "thread.hpp"
#include "mutex.hpp"
#include <list>
using std::list;

namespace hummer
{

class CMsgQueue : public CTimeEvent, public CIoEvent
{
public:
	CMsgQueue();
	~CMsgQueue();
	bool Start();
	void Stop();
	bool PushMsg(MsgPTR& msg);
	virtual void ConsumeMsg(MsgPTR& msg) = 0;
	int IoRead(int fd);
	int IoWrite(int fd);
	static void* IoEventLoop(void* arg);
	enum
	{
		PUSH_TRY_COUNT = 10,
		PUSH_TRY_SLEEPGAP = 100000,
		MSG_QUEUE_TIMER = 1 
	};
private:
	bool m_ExitFlag;
	CThread m_Thread;
	CIoPoller m_EventLoop;
	CSignal m_Signal;
	BatchQueue<MsgPTR> m_MsgBatchQ;
};

class CBatchMsgQueue : public CTimeEvent, public CIoEvent
{
public:
	CBatchMsgQueue();
	~CBatchMsgQueue();

	bool Start(size_t perNum = 0);
	void Stop();
	bool PushMsg(MsgPTR& msg);
	virtual void ConsumeMsg(list<MsgPTR>& msgs) = 0;
	int IoRead(int fd);
	int IoWrite(int fd);
	static void* IoEventLoop(void* arg);
	enum
	{
		PUSH_TRY_COUNT = 10,
		PUSH_TRY_SLEEPGAP = 10000,
		MSG_QUEUE_TIMER = 1,
		MAX_QUEUE_LENGTH = 5000,
		DEF_PER_CONSUME = 30
	};
private:
	bool m_ExitFlag;
	CThread m_Thread;
	CIoPoller m_EventLoop;
	CSignal m_Signal;

	CMutex m_Lock;
	size_t m_PerNum;
	list<MsgPTR> m_List;
};

};

#endif

