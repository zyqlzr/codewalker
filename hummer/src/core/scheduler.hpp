/*****************************************************
 * Copyright zhengyang   
 * FileName: scheduler.hpp 
 * Description: the abstract api of scheduler.hpp
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "task.hpp"
#include "job.hpp"

#include <boost/shared_ptr.hpp>

namespace hummer
{

enum SchedulerType
{
	SCHEDULE_INVALID = -1,
	SCHEDULE_SIM = 0,
	SCHEDULE_MAP,
	SCHEDULE_MAX
};

class Cluster;

class CScheduler
{
public:
	CScheduler(){}
	virtual ~CScheduler(){}
	virtual SchedulerType GetType() = 0;
	virtual bool Schedule(JobPTR& job, set<TaskPTR>& tasks, Cluster& cluster) = 0;
};

class CSimScheduler : public CScheduler
{
public:
	CSimScheduler();
	~CSimScheduler();
	SchedulerType GetType();
	bool Schedule(JobPTR& job, set<TaskPTR>& tasks, Cluster& cluster);
};

class CMapScheduler : public CScheduler
{
public:
	CMapScheduler();
	~CMapScheduler();
	SchedulerType GetType();
	bool Schedule(JobPTR& job, set<TaskPTR>& tasks, Cluster& cluster);
private:
	TaskType GetTaskTypeBySplit(SplitPTR& splitptr);	
};

};

typedef boost::shared_ptr<hummer::CScheduler> SchedulerPTR;

#endif

