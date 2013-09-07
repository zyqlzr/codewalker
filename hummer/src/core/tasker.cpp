#include "tasker.hpp"
#include "testtask.hpp"
#include "sketchtask.hpp"
#include "Logger.h"

hummer::CTasker* hummer::CTasker::CreateTasker(BusinessType bt, TaskType tt)
{
	hummer::CTasker* ptask = NULL;
	if (bt == BUSI_TEST && tt == TASK_SIM)
	{
		LOG_TRACE("create test tasker");
		ptask = new(std::nothrow) CTestTasker();
		return ptask;
	}
	else if (bt == BUSI_TEST && tt == TASK_MAP)
	{
		LOG_TRACE("create testtasker");
		ptask = new(std::nothrow) CTestTasker();
		return ptask;
	}
	else if (bt == BUSI_TEST && tt == TASK_SPLIT)
	{
		LOG_TRACE("create testsplit tasker");
		ptask = new(std::nothrow) CTestSplitTasker();
		return ptask;
	}
	else if (bt == BUSI_SKETCH && tt == TASK_SIM)
	{
		LOG_TRACE("create sketck per tasker");
		ptask = new(std::nothrow) CSketchTasker();
		return ptask;
	}
	else if (bt == BUSI_SKETCH && tt == TASK_SPLIT)
	{
		LOG_TRACE("create sketck split tasker");
		ptask = new(std::nothrow) CSketchSplitTasker();
		return ptask;
	}
	else if (bt == BUSI_SKETCH && tt == TASK_MAP)
	{
		LOG_TRACE("create sketck map tasker");
		ptask = new(std::nothrow) CSketchTasker();
		return ptask;
	}
	else
	{
		return NULL;
	}
}

void hummer::CTasker::DestroyTasker(CTasker* tasker)
{
	if (tasker)
	{
		delete tasker;
	}
	return;
}

void hummer::CTasker::GetSupportBusiness(set<BusinessType>& busis)
{
	busis.insert(BUSI_TEST);
	busis.insert(BUSI_SKETCH);
}

/***********CTaskerInfo***********/
hummer::CTaskerInfo::CTaskerInfo()
{
	memset(&m_Stat, 0, sizeof(m_Stat));
}

hummer::CTaskerInfo::~CTaskerInfo(){}

const hummer::CTaskerInfo& hummer::CTaskerInfo::operator=(const CTaskerInfo& info)
{
	memcpy(&m_Stat, &(info.m_Stat), sizeof(m_Stat));
	m_SplitResult = info.m_SplitResult;
	return *this;
}

void hummer::CTaskerInfo::SetStat(const msg_stat_t& stat)
{
	memcpy(&m_Stat, &stat, sizeof(m_Stat));
}

void hummer::CTaskerInfo::AddSplit(SplitPTR& split)
{
	m_SplitResult.push_back(split);
}

const msg_stat_t& hummer::CTaskerInfo::GetStat()const
{
	return m_Stat;
}

list<SplitPTR>& hummer::CTaskerInfo::GetSplits()
{
	return m_SplitResult;
}



