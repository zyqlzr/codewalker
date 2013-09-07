#include "taskhb.hpp"
#include "utildef.hpp"
#include "msg.hpp"
#include "Logger.h"

#include "job.pb.h"

#include <stdio.h>
#include <string.h>

hummer::CTaskHB::CTaskHB()
: m_Tid(0), m_Jid(0), m_Busi(BUSI_INVALID)
, m_Status(CTask::TASK_STATUS_INVALID)
{
	memset(&m_stat, 0, sizeof(m_stat));
}

hummer::CTaskHB::~CTaskHB(){}

bool hummer::CTaskHB::ConvertByMsg(MsgPTR& msg)
{
	hummer::taskHB* pthb = dynamic_cast<hummer::taskHB*>(msg.get());
	if (!pthb)
	{
		LOG_TRACE("dynamic heartbeat failed");
		return false;
	}

	m_Tid = pthb->taskid();
	m_Jid = pthb->jobid();
	m_Busi = (BusinessType)(pthb->business());
	m_ServerDes = pthb->serverdes();
	m_Status = (CTask::TaskStatus)pthb->status();
	m_stat.uMsgErr = pthb->stat().msgerr();
	m_stat.uMsgOk = pthb->stat().msgok();
	m_stat.uMsgTotal = pthb->stat().msgtotal();
	int splitNum = pthb->splits_size();
	for (int i = 0; i < splitNum; ++i)
	{
		SplitPTR splitptr(new(std::nothrow) CSplit());
		splitptr->Init((CSplit::SplitType)(pthb->splits(i).type()), pthb->splits(i).sid()
			, pthb->splits(i).svalue());
		m_Splits.push_back(splitptr);
	}
	//LOG_TRACE("TaskHB ConvertBy split num="<<m_Splits.size());
	return true;
}

bool hummer::CTaskHB::ConvertToMsg(MsgPTR& msg)
{
	hummer::taskHB* newthb = new(std::nothrow) taskHB();
	MsgPTR thbptr(newthb);
	if (!thbptr.get())
	{
		return false;
	}

	newthb->set_taskid(m_Tid);
	newthb->set_jobid(m_Jid);
	newthb->set_business(m_Busi);
	newthb->set_serverdes(m_ServerDes);
	newthb->set_status(m_Status);
	hummer::statistic* pstat = newthb->mutable_stat();
	pstat->set_msgtotal(m_stat.uMsgTotal);
	pstat->set_msgerr(m_stat.uMsgErr);
	pstat->set_msgok(m_stat.uMsgOk);
	msg = thbptr;
	list<SplitPTR>::iterator sIter = m_Splits.begin();
	for (; sIter != m_Splits.end(); ++sIter)
	{
		if (!(*sIter).get())
		{
			continue;
		}
		hummer::split* pSplit = newthb->add_splits();
		pSplit->set_type((*sIter)->GetType());
		pSplit->set_sid((*sIter)->GetId());
		pSplit->set_svalue((*sIter)->GetText());
	}
	//LOG_TRACE("TaskHB ConvertTo split num="<<m_Splits.size());
	return true;
}

void hummer::CTaskHB::Print()
{
	LOG_TRACE("TaskHB:T="<<m_Tid<<",J="<<m_Jid<<",B="<<m_Busi<<",Des="<<m_ServerDes.c_str()<<",S="<<m_Status);
	LOG_TRACE("TaskHB msg_statis,T="<<m_stat.uMsgTotal<<",OK="<<m_stat.uMsgOk<<",ERR="<<m_stat.uMsgErr<<",split="<<m_Splits.size());
}

void hummer::CTaskHB::SetInfo(JID jid, TID tid, BusinessType bt
			, const string& des, hummer::CTask::TaskStatus ts)
{
	m_Jid = jid;
	m_Tid = tid;
	m_Busi = bt;
	m_ServerDes = des;
	m_Status = ts;
}

void hummer::CTaskHB::SplitResult(list<SplitPTR>& results)
{
	m_Splits = results;
}

void hummer::CTaskHB::Statistic(const msg_stat_t& msgstat)
{
	m_stat.uMsgTotal = msgstat.uMsgTotal;
	m_stat.uMsgErr = msgstat.uMsgErr;
	m_stat.uMsgOk = msgstat.uMsgOk;
}

list<SplitPTR>& hummer::CTaskHB::GetSplitResult()
{
	return m_Splits;
}

TID hummer::CTaskHB::GetTid()const
{
	return m_Tid;
}

JID hummer::CTaskHB::GetJid()const
{
	return m_Jid;
}

BusinessType hummer::CTaskHB::GetBusi()const
{
	return m_Busi;
}

const string& hummer::CTaskHB::GetDes()const
{
	return m_ServerDes;
}

hummer::CTask::TaskStatus hummer::CTaskHB::GetTaskStatus()const
{
	return m_Status;
}

void hummer::CTaskHB::GetStatistic(uint64_t& total, uint64_t& err, uint64_t& ok)
{
	total = m_stat.uMsgTotal;
	err = m_stat.uMsgErr;
	ok = m_stat.uMsgOk;
}



