#include "datacache.hpp"
#include "utildef.hpp"
#include "Logger.h"

hummer::DataSet::DataSet(){}

hummer::DataSet::~DataSet(){}
 
bool hummer::DataSet::Find(MID id, MsgPTR& ptr)
{
	CGuard guard(&m_Lock);
	if (!m_Datas.get())
	{
		return false;
	}

	DATAS::iterator iter = m_Datas->find(id);
	if (iter != m_Datas->end())
	{
		ptr = iter->second;
		return true;
	}
	else
	{
		return false;
	}
}

bool hummer::DataSet::Erase(MID id)
{	
	CGuard guard(&m_Lock);
	if (!m_Datas.get())
	{
		return false;
	}

	DATAS::iterator iter = m_Datas->find(id);
	if (iter != m_Datas->end())
	{
		m_Datas->erase(iter);
		return true;
	}
	else
	{
		return false;
	}
}

bool hummer::DataSet::Erase(MID id, MsgPTR& ptr)
{
	CGuard guard(&m_Lock);
	if (!m_Datas.get())
	{
		return false;
	}

	DATAS::iterator iter = m_Datas->find(id);
	if (iter != m_Datas->end())
	{
		ptr = iter->second;
		m_Datas->erase(iter);
		return true;
	}
	else
	{
		return false;
	}
}

void hummer::DataSet::Push(MID id, MsgPTR& ptr)
{
	CGuard guard(&m_Lock);
	if (!m_Datas.get())
	{
		DatasPTR dataptr(new(std::nothrow) DATAS());
		if (!dataptr.get())
		{
			LOG_ERROR("data push, create failed");
			return;
		}
		m_Datas = dataptr;
	}
	HUMMER_ASSERT(m_Datas.get());
	m_Datas->insert(DATAS::value_type(id, ptr));
	return;
}

void hummer::DataSet::DataMove(DatasPTR& dataptr)
{
	CGuard guard(&m_Lock);
	dataptr = m_Datas;
	m_Datas.reset();
}

void hummer::DataSet::Clear()
{
	CGuard guard(&m_Lock);
	m_Datas.reset();
}

size_t hummer::DataSet::DataNum()
{
	CGuard guard(&m_Lock);
	if (m_Datas.get())
	{
		return m_Datas->size();
	}
	else
	{
		return 0;
	}
}

/**************CTimedCached definition*************/
hummer::CTimedCached::CTimedCached()
: m_TimePoint(0), m_WheelNum(0)
, m_WheelArr(NULL){}

hummer::CTimedCached::~CTimedCached(){}

bool hummer::CTimedCached::Init(uint32_t step)
{
	if (0 == step)
	{
		return false;
	}

	if (m_WheelArr)
	{
		delete [] m_WheelArr;
		m_WheelArr = NULL;
	}
	
	m_WheelArr = new DataSet[step];
	if (!m_WheelArr)
	{
		LOG_ERROR("malloc DataSet arr err");
		return false;
	}
	m_WheelNum.store(step);	
	return true;
}

void hummer::CTimedCached::UnInit()
{
	if (m_WheelArr)
	{
		delete [] m_WheelArr;
		m_WheelArr = NULL;
	}
	m_TimePoint.store(0);
	m_WheelNum.store(0);
	return;
}
	
bool hummer::CTimedCached::PushValue(MID id, MsgPTR& ptr)
{
	uint32_t Curr = m_TimePoint.load();
	uint32_t End = m_WheelNum.load() - 1;
	if (Curr > End
		|| !m_WheelArr)
	{
		LOG_ERROR("array is null,"<<Curr<<","<<End);
		return false;
	}
	LOG_TRACE("TimedCached, size after push="<<MsgNum()<<",pos="<<Curr);
	m_WheelArr[Curr].Push(id, ptr);
	return true;
}

bool hummer::CTimedCached::EraseValue(MID id)
{
	bool succFlag = false;
	uint32_t Curr = m_TimePoint.load();
	uint32_t End = m_WheelNum.load() - 1;
	uint32_t pos = Curr;
	do
	{
		if (m_WheelArr
			&& m_WheelArr[pos].Erase(id))
		{
			succFlag = true;
			LOG_TRACE("TimedCached, size after delete="<<MsgNum()<<",pos="<<pos);
			break;
		}

		if (pos == 0)
		{
			pos = End;
		}
		else
		{
			--pos;
		}
	}while(pos != Curr);

	return succFlag;
}

bool hummer::CTimedCached::EraseValue(MID id, MsgPTR& ptr)
{
	bool succFlag = false;
	uint32_t Curr = m_TimePoint.load();
	uint32_t End = m_WheelNum.load() - 1;
	uint32_t pos = Curr;
	do
	{
		if (m_WheelArr
			&& m_WheelArr[pos].Erase(id, ptr))
		{
			succFlag = true;
			LOG_TRACE("TimedCached, size after delete="<<MsgNum()<<",pos="<<pos);
			break;
		}

		if (pos == 0)
		{
			pos = End;
		}
		else
		{
			--pos;
		}
	}while(pos != Curr);

	return succFlag;
}

void hummer::CTimedCached::RoundOneStep(DatasPTR& datas)
{
	uint32_t End = m_WheelNum.load() - 1;
	uint32_t Curr = m_TimePoint.load();
	uint32_t TimeOut = Curr + 1;
	if (Curr < End)
	{
		LOG_TRACE("Timeout pos:"<<TimeOut);
		m_WheelArr[TimeOut].DataMove(datas);
	}
	else
	{
		LOG_TRACE("Timeout pos: 0");
		m_WheelArr[0].DataMove(datas);
	}

	if (!m_TimePoint.compare_exchange_weak(End, 0))
	{
		m_TimePoint.fetch_add(1);
	}
}

size_t hummer::CTimedCached::MsgNum()const
{
	size_t msgnum = 0;
	for (size_t i = 0; i < m_WheelNum; ++i)
	{
		if (m_WheelArr)
		{
			msgnum += m_WheelArr[i].DataNum();
		}
	}

	return msgnum;
}

/**********CFailedCached************/
hummer::CFailedCached::CFailedCached(){}

hummer::CFailedCached::~CFailedCached(){}

void hummer::CFailedCached::MsgsTimeout(DatasPTR& datas)
{
	CGuard guard(&m_Lock);
	m_Timeout.push_back(datas);
}

void hummer::CFailedCached::MsgFailed(MID id, MsgPTR& msg)
{
	CGuard guard(&m_Lock);
	if (!m_Failed.get())
	{
		DatasPTR dataptr(new(std::nothrow) DATAS());
		if (!dataptr.get())
		{
			return;
		}
		m_Failed = dataptr;
	}

	m_Failed->insert(DATAS::value_type(id, msg));
}

void hummer::CFailedCached::GetFailedByNumber(list<MsgPTR>& msgs, size_t num)
{
	size_t msgNum = 0;
	if (0 == num)
	{
		return;
	}

	CGuard guard(&m_Lock);
	if (m_Failed.get() && m_Failed->size() > 0)
	{
		DATAS::iterator fIter = m_Failed->begin();
		for (; fIter != m_Failed->end(); )
		{
			msgs.push_back(fIter->second);
			m_Failed->erase(fIter++);
			++msgNum;
			if (msgNum == num)
			{
				return;
			}	
		}
	}

	if(m_Timeout.size() == 0)
	{
		return;
	}
	list<DatasPTR>::iterator lIter = m_Timeout.begin();
	for (; lIter != m_Timeout.end(); )
	{
		if (!(*lIter).get()
			|| (*lIter)->size() == 0)
		{
			lIter = m_Timeout.erase(lIter);
			continue;
		}

		DATAS::iterator fIter = (*lIter)->begin();
		for (; fIter != (*lIter)->end(); )
		{
			msgs.push_back(fIter->second);
			m_Failed->erase(fIter++);
			++msgNum;
			if (msgNum == num)
			{
				return;
			}
		}
	}
}

size_t hummer::CFailedCached::FailedNum()
{
	size_t num = 0;
	CGuard guard(&m_Lock);
	if (m_Failed.get())
	{
		num += m_Failed->size();
	}
	
	list<DatasPTR>::iterator lIter = m_Timeout.begin();
	for (; lIter != m_Timeout.end(); )
	{
		if (!(*lIter).get()
			|| (*lIter)->size() == 0)
		{
			lIter = m_Timeout.erase(lIter);
			continue;
		}

		num += (*lIter)->size();
		++lIter;
	}
	return num;
}


