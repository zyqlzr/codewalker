#include "task.hpp"
#include "utildef.hpp"
#include "job.pb.h"
#include "Logger.h"

#include <stdio.h>
#include <string.h>

hummer::CTask::CTask()
: m_TaskId(0), m_JobId(0), m_TaskType(TASK_INVALID)
, m_Busi(BUSI_INVALID){}

hummer::CTask::~CTask(){}

void hummer::CTask::Reset()
{
	m_TaskId = 0;
	m_JobId = 0;
	m_ServerDes.clear();
}

void hummer::CTask::SetJobId(JID jId)
{
	m_JobId = jId;
}

void hummer::CTask::SetTaskId(TID tId)
{
	m_TaskId = tId;
}

void hummer::CTask::SetServerDes(const string& des)
{
	m_ServerDes = des;
}

JID hummer::CTask::GetJobId()const
{
	return m_JobId;
}

TID hummer::CTask::GetTaskId()const
{
	return m_TaskId;
}

const string& hummer::CTask::GetServerDes()const
{
	return m_ServerDes;
}

void hummer::CTask::SetTaskType(TaskType tt)
{
	m_TaskType = tt;
}

void hummer::CTask::SetBusinessType(BusinessType bt)
{
	m_Busi = bt;
}

void hummer::CTask::SetSplit(SplitPTR& split)
{
	m_Split = split;
}

TaskType hummer::CTask::GetTaskType()const
{
	return m_TaskType;
}

BusinessType hummer::CTask::GetBusiType()const
{
	return m_Busi;
}

SplitPTR& hummer::CTask::GetSplit()
{
	return m_Split;
}

const string& hummer::CTask::GetSlot()const
{
	return m_Slot;
}

void hummer::CTask::SetSlot(const string& slot)
{
	m_Slot = slot;
}

bool hummer::CTask::ConvertByMsg(MsgPTR& msg)
{
	hummer::task* ptask = dynamic_cast<hummer::task*>(msg.get());
	if (!ptask)
	{
		LOG_ERROR("msg is null");
		return false;
	}

	m_TaskId = ptask->taskid();
	m_JobId = ptask->jobid();
	m_ServerDes = ptask->serverdes();
	m_TaskType = (TaskType)(ptask->tasktype());
	m_Busi = (BusinessType)(ptask->business());
	m_Slot = ptask->slot();
	int srcnum = ptask->srcdes_size();
	for (int i = 0; i < srcnum; ++i)
	{
		m_SrcDes.insert(ptask->srcdes(i));
	}
	HUMMER_ASSERT((size_t)srcnum == m_SrcDes.size());

	int dstnum = ptask->dstdes_size();
	for (int j = 0; j < dstnum; ++j)
	{
		m_DstDes.insert(ptask->dstdes(j));
	}

	int splitnum = ptask->splits_size();
	if (0 == splitnum)
	{
		return true;
	}

	SplitPTR newsplit(new(std::nothrow) CSplit());
	if (!newsplit.get())
	{
		LOG_ERROR("create split failed");
		return false;
	}

	if (ptask->splits(0).has_type())
	{
		newsplit->Init((CSplit::SplitType)(ptask->splits(0).type())
			, ptask->splits(0).sid()
			, ptask->splits(0).svalue());
	}
	else
	{
		newsplit->Init(CSplit::SPLIT_DEF, ptask->splits(0).sid()
			, ptask->splits(0).svalue());
	}
	m_Split = newsplit;
	return true;
}

bool hummer::CTask::ConvertToMsg(MsgPTR& msg)
{
	hummer::task* newTask = new(std::nothrow) hummer::task();
	MsgPTR taskptr(newTask);
	if (!taskptr.get())
	{
		return false;
	}
	newTask->set_taskid(m_TaskId);
	newTask->set_tasktype(m_TaskType);
	newTask->set_jobid(m_JobId);
	newTask->set_business(m_Busi);
	newTask->set_serverdes(m_ServerDes);
	newTask->set_slot(m_Slot);
	hummer::split* psplit =  newTask->add_splits();
	if (!psplit)
	{
		return false;
	}
	psplit->set_sid(m_Split->GetId());
	psplit->set_svalue(m_Split->GetText());

	set<string>::iterator srcIter = m_SrcDes.begin();
	for (; srcIter != m_SrcDes.end(); ++srcIter)
	{
		newTask->add_srcdes(*srcIter);
	}
	
	set<string>::iterator dstIter = m_DstDes.begin();
	for (; dstIter != m_DstDes.end(); ++dstIter)
	{
		newTask->add_dstdes(*dstIter);
	}
	msg = taskptr;
	return true;
}

void hummer::CTask::Print()
{
	LOG_TRACE("Task, T="<<m_TaskId<<",J="<<m_JobId<<",B="<<m_Busi<<",Type"<<m_TaskType<<",DES="<<m_ServerDes.c_str()<<",Slot="<<m_Slot);
	if (m_Split.get())
	{
		LOG_TRACE("Task info,sn="<<m_SrcDes.size()<<"dn="<<m_DstDes.size()<<",SID="<<m_Split->GetId()<<",STEXT="<<m_Split->GetText());
	}
	else
	{
		LOG_TRACE("Task info,sn="<<m_SrcDes.size()<<"dn="<<m_DstDes.size());
	}	
}

void hummer::CTask::SetSrcAndDst(const set<string>& src, const set<string>& dst)
{
	m_SrcDes = src;
	m_DstDes = dst;	
}


