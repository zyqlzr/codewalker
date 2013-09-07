#include "msgprocessor.hpp"
#include "utildef.hpp"

hummer::CMsgQueue::CMsgQueue(){}

hummer::CMsgQueue::~CMsgQueue(){}

bool hummer::CMsgQueue::Start()
{
	SetTimerSec(MSG_QUEUE_TIMER);
	if (!m_Signal.Init() 
		|| 0 != m_EventLoop.Init(IO_TYPE_EPOLL)
	 	|| 0 != m_EventLoop.AddRead(m_Signal.GetFd(), this)
		|| 0 != m_EventLoop.AddTime(this))
	{
		return false;
	}

	m_ExitFlag = true;
	return m_Thread.Start(hummer::CMsgQueue::IoEventLoop, (void*)this);
}

void hummer::CMsgQueue::Stop()
{
	m_ExitFlag = false;
	m_Thread.Stop();
	m_EventLoop.UnInit();
	m_Signal.UnInit();
	return;
}

bool hummer::CMsgQueue::PushMsg(MsgPTR& msg)
{
	if (m_MsgBatchQ.Produce(msg))
	{
		m_Signal.Send();
		return true;
	}

	return false;
}

int hummer::CMsgQueue::IoRead(int fd)
{
	HUMMER_ASSERT(fd == m_Signal.GetFd());
	m_Signal.Recv();

	list<MsgPTR> msgs;
	if (m_MsgBatchQ.BatchConsume(msgs))
	{
		list<MsgPTR>::iterator mIter = msgs.begin();
		for (; mIter != msgs.end(); ++mIter)
		{
			ConsumeMsg(*mIter);	
		}
		return 0;
	}

	return -1;
}

int hummer::CMsgQueue::IoWrite(int fd)
{
	HUMMER_NOUSE(fd);
	HUMMER_ASSERT(false);
	return -1;
}

void* hummer::CMsgQueue::IoEventLoop(void* arg)
{
	CMsgQueue* mq = (CMsgQueue*)arg;
	if (NULL == mq)
	{
		return NULL;
	}

	while(mq->m_ExitFlag)
	{
		mq->m_EventLoop.Event();		
	}
	LOG_TRACE("msgqueue exit loop");

	return  NULL;
}

/**************CBatchMsgQueue***************/
hummer::CBatchMsgQueue::CBatchMsgQueue()
:m_ExitFlag(0), m_PerNum(0){}

hummer::CBatchMsgQueue::~CBatchMsgQueue(){}

bool hummer::CBatchMsgQueue::Start(size_t perNum)
{
	SetTimerSec(MSG_QUEUE_TIMER);
	if (!m_Signal.Init() 
		|| 0 != m_EventLoop.Init(IO_TYPE_EPOLL)
	 	|| 0 != m_EventLoop.AddRead(m_Signal.GetFd(), this)
		|| 0 != m_EventLoop.AddTime(this))
	{
		return false;
	}
	
	if (0 == perNum)
	{
		m_PerNum = DEF_PER_CONSUME;
	}
	else
	{ 
		m_PerNum = perNum;
	}

	m_ExitFlag = true;
	return m_Thread.Start(hummer::CMsgQueue::IoEventLoop, (void*)this);
}

void hummer::CBatchMsgQueue::Stop()
{
	m_ExitFlag = false;
	m_Thread.Stop();
	m_EventLoop.UnInit();
	m_Signal.UnInit();
	m_List.clear();
	return;
}

bool hummer::CBatchMsgQueue::PushMsg(MsgPTR& msg)
{
	size_t listNum = m_List.size();
	if (listNum >= MAX_QUEUE_LENGTH)
	{
		LOG_TRACE("Msgqueue is full");
		return false;
	}

	{
		CGuard guard(&m_Lock);
		m_List.push_back(msg);
	}
	m_Signal.Send();
	return true;
}

int hummer::CBatchMsgQueue::IoRead(int fd)
{
	HUMMER_ASSERT(fd == m_Signal.GetFd());
	m_Signal.Recv();
	size_t msgNum = m_List.size();
	if (msgNum == 0)
	{
		return 0;
	}

	list<MsgPTR> msgs;
	{
		size_t consume = msgNum > m_PerNum ? m_PerNum : msgNum;
		CGuard guard(&m_Lock);
		for (size_t i = 0; i < consume; ++i)
		{
			msgs.push_back(m_List.front());
			m_List.pop_front();	
		}
	}

	ConsumeMsg(msgs);
	return 0;
}

int hummer::CBatchMsgQueue::IoWrite(int fd)
{
	HUMMER_NOUSE(fd);
	HUMMER_ASSERT(false);
	return -1;
}

void* hummer::CBatchMsgQueue::IoEventLoop(void* arg)
{
	CBatchMsgQueue* mq = (CBatchMsgQueue*)arg;
	if (NULL == mq)
	{
		return NULL;
	}

	while(mq->m_ExitFlag)
	{
		mq->m_EventLoop.Event();
	}
	LOG_TRACE("batch msgqueue exit loop");

	return  NULL;
}



