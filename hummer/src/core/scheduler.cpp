#include "scheduler.hpp"
#include "Logger.h"
#include "workslot.hpp"
#include "cluster.hpp"
#include "utildef.hpp"
#include "split.hpp"
/************CSimScheduler definition*************/
hummer::CSimScheduler::CSimScheduler(){}

hummer::CSimScheduler::~CSimScheduler(){}

hummer::SchedulerType hummer::CSimScheduler::GetType()
{
	return hummer::SCHEDULE_SIM;
}

bool hummer::CSimScheduler::Schedule(JobPTR& job, set<TaskPTR>& tasks, Cluster& cluster)
{
	HUMMER_ASSERT(JOB_SIM == job->GetJobType());

	CJob::JobStatus js = job->GetStatus();
	JID jid = job->GetJobId();
	BusinessType bt = job->GetBusi();
	if (js == CJob::JOB_STATUS_SUBMIT
		|| js == CJob::JOB_STATUS_ERR
		|| js == CJob::JOB_STATUS_PART)
	{
		list<SlotPTR> slots;
		list<SplitPTR> splits;
		//get unassigned split
		job->Print();
		job->UnAssignSplit(splits);
		size_t freeSlots = cluster.FreeSlotNum();
		size_t scheNum = freeSlots < splits.size() ? freeSlots : splits.size();
		if (!cluster.FreeSlot(scheNum, slots) 
			|| 0 == scheNum	|| 0 == slots.size())
		{
			LOG_ERROR("Get Slot failed,schenum="<<scheNum<<",slot="<<slots.size()<<",un-split="<<splits.size());
			return false;
		}

		LOG_TRACE("sche: split="<<splits.size()<<",slot="<<slots.size()<<",scheNum="<<scheNum);
		for (size_t i = 0; i < scheNum; ++i)
		{
			TID tid = 0;
			SlotPTR slot = slots.front();
			slots.pop_front();
			SplitPTR split = splits.front();
			splits.pop_front();

			if (!cluster.GenerateTID(tid))
			{
				LOG_ERROR("Generate tid err");
				continue;
			}

			TaskPTR taskptr(new(std::nothrow) CTask());
			taskptr->SetJobId(jid);
			taskptr->SetTaskId(tid);
			taskptr->SetServerDes(slot->ServerDes());
			taskptr->SetSlot(slot->Slot());
			taskptr->SetTaskType(TASK_SIM);
			taskptr->SetBusinessType(bt);
			taskptr->SetSplit(split);
			taskptr->SetSrcAndDst(job->GetSrc(), job->GetDst());
			tasks.insert(taskptr);
			LOG_TRACE("sche result: slot="<<slot->ServerDes()<<",split="<<split->GetId()<<",jid="<<jid<<",tid="<<tid);
		}
		return true;
	}
	else
	{
		HUMMER_ASSERT(false);
	}	
	return false;
}

/*************CMapScheduler definition***************/
hummer::CMapScheduler::CMapScheduler(){}

hummer::CMapScheduler::~CMapScheduler(){}

hummer::SchedulerType hummer::CMapScheduler::GetType()
{
	return hummer::SCHEDULE_MAP;
}

bool hummer::CMapScheduler::Schedule(JobPTR& job, set<TaskPTR>& tasks
		, Cluster& cluster)
{
	HUMMER_ASSERT(JOB_MAP == job->GetJobType());

	CJob::JobStatus js = job->GetStatus();
	JID jid = job->GetJobId();
	BusinessType bt = job->GetBusi();
	if (js == CJob::JOB_STATUS_SUBMIT
		|| js == CJob::JOB_STATUS_ERR
		|| js == CJob::JOB_STATUS_PART)
	{
		list<SlotPTR> slots;
		list<SplitPTR> splits;
		//get unassigned split
		job->Print();
		job->UnAssignSplit(splits);
		size_t freeSlots = cluster.FreeSlotNum();
		size_t scheNum = freeSlots < splits.size() ? freeSlots : splits.size();
		if (!cluster.FreeSlot(scheNum, slots) 
			|| 0 == scheNum	|| 0 == slots.size())
		{
			LOG_ERROR("Get Slot failed,schenum="<<scheNum<<",slot="<<slots.size()<<",un-split="<<splits.size());
			return false;
		}

		LOG_TRACE("sche: split="<<splits.size()<<",slot="<<slots.size()<<",scheNum="<<scheNum);
		for (size_t i = 0; i < scheNum; ++i)
		{
			TID tid = 0;
			SlotPTR slot = slots.front();
			slots.pop_front();
			SplitPTR split = splits.front();
			splits.pop_front();

			if (!cluster.GenerateTID(tid))
			{
				LOG_ERROR("Generate tid err");
				continue;
			}
			TaskType tt = GetTaskTypeBySplit(split);
	
			TaskPTR taskptr(new(std::nothrow) CTask());
			taskptr->SetJobId(jid);
			taskptr->SetTaskId(tid);
			taskptr->SetServerDes(slot->ServerDes());
			taskptr->SetSlot(slot->Slot());
			taskptr->SetTaskType(tt);
			taskptr->SetBusinessType(bt);
			taskptr->SetSplit(split);
			taskptr->SetSrcAndDst(job->GetSrc(), job->GetDst());
			tasks.insert(taskptr);
			LOG_TRACE("sche result: slot="<<slot->ServerDes()<<",split="<<split->GetId()<<",jid="<<jid<<",tid="<<tid);
		}
		return true;
	}
	else
	{
		HUMMER_ASSERT(false);
	}	
	return false;	
}

TaskType hummer::CMapScheduler::GetTaskTypeBySplit(SplitPTR& splitptr)
{
	CSplit::SplitType type = splitptr->GetType();
	LOG_TRACE("Split:Id="<<splitptr->GetId()<<",Type="<<splitptr->GetType()<<",text="<<splitptr->GetText());
	switch(type)
	{
	case CSplit::SPLIT_PRE:
		return TASK_SPLIT;
	case CSplit::SPLIT_MAP:
		return TASK_MAP;
	default:
		HUMMER_ASSERT(false);
		return TASK_INVALID;
	}
}

