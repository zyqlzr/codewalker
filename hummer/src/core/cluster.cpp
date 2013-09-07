#include "cluster.hpp"
#include "utildef.hpp"
#include "clock.hpp"

hummer::Cluster::Cluster(): m_Generator(NULL){}

hummer::Cluster::~Cluster(){}

bool hummer::Cluster::Init(CIdGenerator* idgenerator)
{
	if (!idgenerator)
	{
		return false;
	}

	for (int i = 0; i <= SCHEDULE_MAX; ++i)
	{
		SchedulerPTR ptr;
		m_Schedulers.push_back(ptr);
	}

	SchedulerPTR simptr(new(std::nothrow) CSimScheduler());
	SchedulerPTR mapptr(new(std::nothrow) CMapScheduler());
	if (simptr.get())
	{
		m_Schedulers[SCHEDULE_SIM] = simptr;
	}

	if (mapptr.get())
	{
		m_Schedulers[SCHEDULE_MAP] = mapptr;
	}

	m_Generator = idgenerator;
	return true;
}

void hummer::Cluster::Clean()
{
	m_Workers.clear();
	m_WorkSlots.clear();
	m_FreeSlots.clear();
	m_UsedSlots.clear();
	m_Jobs.clear();
	m_des2job.clear();
	m_CurrTask.clear();
	m_CurrTaskhbs.clear();
	m_RedoJob.clear();
	m_FiniJob.clear();
}

void hummer::Cluster::UnInit()
{
	m_Workers.clear();
	m_WorkSlots.clear();
	m_FreeSlots.clear();
	m_UsedSlots.clear();
	m_Jobs.clear();
	m_des2job.clear();
	m_CurrTask.clear();
	m_CurrTaskhbs.clear();
	m_RedoJob.clear();
	m_FiniJob.clear();
	
	m_Generator = NULL;
	return;
}

void hummer::Cluster::TimeOutCheck(set<TaskPTR>& delTasks)
{
	m_TS = CClock::clock_ms();
	list<TID> timeouts;
	TASKHBS::iterator hbIter = m_CurrTaskhbs.begin();
	for (; hbIter != m_CurrTaskhbs.end();)
	{
		if (m_TS - hbIter->second > TASK_TIMEOUT)
		{
			timeouts.push_back(hbIter->first);
			m_CurrTaskhbs.erase(hbIter++);
		}
		else
		{
			++hbIter;
		}
	}

	list<TID>::iterator tIter = timeouts.begin();
	for (; tIter != timeouts.end(); ++tIter)
	{
		TaskPTR delTask;
		//release task, and add to set to be deleted
		ReleaseTask(*tIter, delTask);
		HUMMER_ASSERT(delTask.get());
		delTasks.insert(delTask);

		//tell job the task err
		JOBS::iterator findJ = m_Jobs.find(delTask->GetJobId());
		HUMMER_ASSERT(findJ != m_Jobs.end());
		findJ->second->TaskErr(*tIter);
		if (findJ->second->GetStatus() == CJob::JOB_STATUS_PART
			|| findJ->second->GetStatus() == CJob::JOB_STATUS_ERR)
		{
			m_RedoJob.insert(findJ->first);
		}
	}

	if (timeouts.size() > 0)
	{
		LOG_TRACE("Timeout task="<<timeouts.size());
	}
}

bool hummer::Cluster::ServerRecover(set<ServerPTR>& servers)
{
	if (servers.size() == 0)
	{
		return true;
	}

	set<ServerPTR>::iterator sIter = servers.begin();
	for (; sIter != servers.end(); ++sIter)
	{
		m_Workers.insert(SERVERS::value_type((*sIter)->GetServerDes(), *sIter));
		//init slot by servers
		SlotPTR slotptr(new(std::nothrow) CWorkSlot());
		GenerateSlot(*sIter, slotptr);
		m_WorkSlots.insert(WORKSLOTS::value_type((*sIter)->GetServerDes(), slotptr));
		m_FreeSlots.insert((*sIter)->GetServerDes());
	}

	HUMMER_ASSERT(m_Workers.size() == m_FreeSlots.size());
	return true;
}

bool hummer::Cluster::ScheduleRecover(set<JobPTR>& jobs, set<TaskPTR>& tasks
		, set<TaskPTR>& delTasks)
{
	//HUMMER_ASSERT(!(jobs.size() == 0 && tasks.size() > 0));
	m_TS = CClock::clock_ms();

	set<JobPTR>::iterator jIter = jobs.begin();
	for (; jIter != jobs.end(); ++jIter)
	{
		m_Jobs.insert(JOBS::value_type((*jIter)->GetJobId(), *jIter));
		m_des2job.insert(DESJOBS::value_type((*jIter)->GetDes(), *jIter));
	}

	set<TaskPTR>::iterator tIter = tasks.begin();
	for (; tIter != tasks.end(); ++tIter)
	{
		TID tid = (*tIter)->GetTaskId();
		JID jid = (*tIter)->GetJobId();
		string slot = (*tIter)->GetSlot();
		JOBS::iterator jfind = m_Jobs.find(jid);
		//task should be exist in un-finish job,
		//and the workslot of task should exist
		if (m_Jobs.end() != jfind
			&& jfind->second->GetStatus() != CJob::JOB_STATUS_FINI
			&& m_WorkSlots.end() != m_WorkSlots.find(slot))
		{
			//valid task
			m_CurrTask.insert(TASKS::value_type(tid, *tIter));
			m_CurrTaskhbs.insert(TASKHBS::value_type(tid, m_TS));
			m_FreeSlots.erase(slot);
			m_UsedSlots.insert(slot);
		}
		//abandon all invalid task, invlid task will be delete
		delTasks.insert(*tIter);
	}

	CheckJob();
	return true;
}

void hummer::Cluster::CheckPerJob(JobPTR& job)
{
	if (job->GetStatus() == CJob::JOB_STATUS_RUNNING)
	{
		m_RedoJob.erase(job->GetJobId());
	}
	else if (job->GetStatus() == CJob::JOB_STATUS_PART 
			|| job->GetStatus() == CJob::JOB_STATUS_ERR)
	{
		m_RedoJob.insert(job->GetJobId());
	}
	else
	{
		HUMMER_ASSERT(false);
	}
}

void hummer::Cluster::CheckJob()
{
	JOBS::iterator jIter = m_Jobs.begin();
	for (; jIter != m_Jobs.end(); ++jIter)
	{
		// job finish
		CJob::JobStatus js = jIter->second->GetStatus();
		if (js == CJob::JOB_STATUS_FINI)
		{
			HUMMER_ASSERT(jIter->second->CheckFini());
			m_FiniJob.insert(jIter->first);
			continue;
		}

		//check all assignment of job
		set<TID> tids; 
		jIter->second->GetRunningTaskID(tids);
		set<TID>::iterator idIter = tids.begin();
		for (; idIter != tids.end(); ++idIter)
		{
			TASKS::iterator cIter = m_CurrTask.find(*idIter);
			if (m_CurrTask.end() == cIter)
			{
				//task of job not exist in valid tasks
				jIter->second->TaskErr(*idIter);
			}
			else
			{
				jIter->second->TaskRun(cIter->second);
			}
		}

		//check job should be re-schedule or not
		if (CJob::JOB_STATUS_ERR ==  js
			|| CJob::JOB_STATUS_PART == js
			|| CJob::JOB_STATUS_SUBMIT == js)
		{
			m_RedoJob.insert(jIter->first);
		}
	}
}

SchedulerPTR& hummer::Cluster::ChooseScheduler(JobPTR& job)
{
	JobType type = job->GetJobType();
	switch(type)
	{
	case JOB_SIM:
		LOG_TRACE("JOB SIM");
		return m_Schedulers[SCHEDULE_SIM];
	case JOB_MAP:
		LOG_TRACE("JOB MAP");
		return m_Schedulers[SCHEDULE_MAP];
	default:
		LOG_TRACE("JOB ERR");
		return m_Schedulers[SCHEDULE_MAX];
	}
}

bool hummer::Cluster::Schedule(JobPTR& job, set<TaskPTR>& tasks)
{
	if (!job.get())
	{
		return false;
	}
	HUMMER_ASSERT(job->GetStatus() == CJob::JOB_STATUS_SUBMIT);

	//submit job
	LOG_TRACE("schedule,des="<<job->GetDes());
	m_Jobs.insert(JOBS::value_type(job->GetJobId(), job));
	m_des2job.insert(DESJOBS::value_type(job->GetDes(), job));
	m_RedoJob.insert(job->GetJobId());

	//schedule job
	SchedulerPTR schedulerptr = ChooseScheduler(job);
	if (schedulerptr.get() && schedulerptr->Schedule(job, tasks, *this))
	{
		return true;
	}

	return false;
}

void hummer::Cluster::Commit(JobPTR& job, set<TaskPTR>& tasks)
{
	JID jid = job->GetJobId();
	HUMMER_ASSERT(m_Jobs.end() != m_Jobs.find(jid));
	m_RedoJob.erase(jid);

	//statistic workslot used by tasks
	set<TaskPTR>::iterator iter = tasks.begin();
	for (; iter != tasks.end(); ++iter)
	{
		HUMMER_ASSERT(jid == (*iter)->GetJobId());
		SetupTask(*iter);
	}
	job->Commit(tasks);

	//check job is running or need redo(have un-assign split)
	if (job->GetStatus() == CJob::JOB_STATUS_RUNNING)
	{
		m_RedoJob.erase(jid);
	}
	else if (job->GetStatus() == CJob::JOB_STATUS_PART 
			|| job->GetStatus() == CJob::JOB_STATUS_ERR)
	{
		m_RedoJob.insert(jid);
	}
	else
	{
		HUMMER_ASSERT(false);
	}
}

void hummer::Cluster::Rollback(JobPTR& job, set<TaskPTR>& tasks)
{
	JID jid = job->GetJobId();
	HUMMER_ASSERT(m_Jobs.end() != m_Jobs.find(jid));
	job->Rollback(tasks);

	//check job status
	if (job->GetStatus() == CJob::JOB_STATUS_RUNNING)
	{
		HUMMER_ASSERT(false);
	}
	else if (job->GetStatus() == CJob::JOB_STATUS_ERR
			|| job->GetStatus() == CJob::JOB_STATUS_PART)
	{
		m_RedoJob.insert(jid);
	}
	else
	{
		HUMMER_ASSERT(false);
	}
}

bool hummer::Cluster::TaskDel(TID tid, JobPTR& modJob, set<TaskPTR>& delTasks)
{
	TaskPTR delTask;
	//release task
	ReleaseTask(tid, delTask);
	if (!delTask.get())
	{
		LOG_TRACE("Task is not exist, tid"<<tid);
		return false;
	}
	delTasks.insert(delTask);

	//tell job the task is err
	JID jid = delTask->GetJobId();
	JOBS::iterator jIter = m_Jobs.find(jid);
	HUMMER_ASSERT(jIter != m_Jobs.end());
	jIter->second->TaskErr(tid);
	if (jIter->second->GetStatus() == CJob::JOB_STATUS_ERR
		|| jIter->second->GetStatus() == CJob::JOB_STATUS_PART)
	{
		m_RedoJob.insert(jid);
	}
	else
	{
		HUMMER_ASSERT(false);
	}
	modJob = jIter->second;
	return true;
}

bool hummer::Cluster::TaskHB(TaskhbPTR& thb, JobPTR& modJob, set<TaskPTR>& delTasks)
{
	if (!thb.get())
	{
		return false;
	}

	CTask::TaskStatus ts = thb->GetTaskStatus();
	TID tid = thb->GetTid();
	JID jid = thb->GetJid();
	JOBS::iterator jIter = m_Jobs.find(jid);
	//check valid
	if (jIter == m_Jobs.end())
	{
		LOG_ERROR("older job");
		return false;
	}

	if (CTask::TASK_STATUS_RUNNING == ts)
	{
		//just update timestamp
		TASKHBS::iterator hbIter = m_CurrTaskhbs.find(tid);
		if (hbIter == m_CurrTaskhbs.end())
		{
			m_CurrTaskhbs.insert(TASKHBS::value_type(tid, m_TS));
		}
		else
		{
			hbIter->second = m_TS;
		}
		//LOG_TRACE("TASK heartbeat");
	}
	else if (CTask::TASK_STATUS_FINI == ts)
	{
		//release task
		TaskPTR delTask;
		ReleaseTask(tid, delTask);
		if (!delTask.get())
		{
			//already release
			return true;
		}
		delTasks.insert(delTask);
		LOG_TRACE("Task fini:jid="<<jid<<",tid="<<tid<<",slot="<<delTask->GetSlot()<<",split="<<delTask->GetSplit()->GetId());
		//tell job task finish
		jIter->second->TaskFini(tid, thb);
		if (jIter->second->CheckFini())
		{
			//if job is finish, insert 
			m_FiniJob.insert(jid);
		}
		
		if (jIter->second->GetStatus() == CJob::JOB_STATUS_ERR
			|| jIter->second->GetStatus() == CJob::JOB_STATUS_PART)
		{
			m_RedoJob.insert(jid);
		}

		modJob = jIter->second;
	}
	else if (CTask::TASK_STATUS_FAILED == ts 
		||	CTask::TASK_STATUS_ERR == ts)
	{
		//release task
		TaskPTR delTask;
		ReleaseTask(tid, delTask);
		if (!delTask.get())
		{
			return false;
		}
		//HUMMER_ASSERT(delTask.get());
		delTasks.insert(delTask);

		//tell job task err
		jIter->second->TaskErr(tid);
		if (jIter->second->GetStatus() == CJob::JOB_STATUS_ERR
			|| jIter->second->GetStatus() == CJob::JOB_STATUS_PART)
		{
			m_RedoJob.insert(jid);
		}
		else
		{
			HUMMER_ASSERT(false);
		}
		modJob = jIter->second;
	}
	else
	{
		HUMMER_ASSERT(false);
	}

	return true;
}

bool hummer::Cluster::DelJob(const string& jobdes, JobPTR& delJob, set<TaskPTR>& delTasks)
{
	DESJOBS::iterator iter = m_des2job.find(jobdes);
	if (iter == m_des2job.end())
	{
		LOG_ERROR("Del job failed,"<<jobdes);
		return false;
	}

	JID jid = iter->second->GetJobId();
	//release all task of job;
	set<TID> tids;
	iter->second->GetAllTaskID(tids);
	set<TID>::iterator tIter = tids.begin();
	for (; tIter != tids.end(); ++tIter)
	{
		//release task
		TaskPTR delTask;
		ReleaseTask(*tIter, delTask);
		if (delTask.get())
		{
			delTasks.insert(delTask);
		}
	}

	//job to del
	delJob = iter->second;
	//release job
	m_des2job.erase(iter);
	m_Jobs.erase(jid);
	m_RedoJob.erase(jid);
	m_FiniJob.erase(jid);
	//HUMMER_ASSERT(m_Jobs.size() == m_des2job.size());
	return true;
}

void hummer::Cluster::GetNeedRedoJob(list<JobPTR>& jobs)
{
	if (m_RedoJob.size() > 0)
	{
		//LOG_TRACE("Redo job num="<<m_RedoJob.size());
		set<JID>::iterator iter = m_RedoJob.begin();
		for (; iter != m_RedoJob.end(); ++iter)
		{
			JOBS::iterator jIter = m_Jobs.find(*iter);
			HUMMER_ASSERT(jIter != m_Jobs.end());
			jobs.push_back(jIter->second);
		}
	}
}

bool hummer::Cluster::ReSchedule(JobPTR& job, set<TaskPTR>& assignTasks)
{
	if (!job.get())
	{
		return false;
	}

	//schedule job
	SchedulerPTR schedulerptr = ChooseScheduler(job);
	if (schedulerptr.get() && schedulerptr->Schedule(job, assignTasks, *this))
	{
		return true;
	}
	return false;
}

void hummer::Cluster::GenerateSlot(const ServerPTR& server, SlotPTR& slot)
{
	if (!server.get() || !slot.get())
	{
		LOG_ERROR("slot or server is null");
		return;
	}
	slot->Setup(server->GetServerDes(), server->CpuCore(), 0);
	return;
}

void hummer::Cluster::ReleaseTask(TID tid, TaskPTR& delTask)
{
	//delete task heartbeat
	m_CurrTaskhbs.erase(tid);
	TASKS::iterator iter = m_CurrTask.find(tid);
	if (iter == m_CurrTask.end())
	{
		return;
	}

	delTask = iter->second;
	string slot = iter->second->GetSlot();
	//release slot
	if (m_WorkSlots.end() != m_WorkSlots.find(slot))
	{
		m_FreeSlots.insert(slot);
		m_UsedSlots.erase(slot);
	}
	//delete task from task set
	m_CurrTask.erase(iter);
	return;
}

void hummer::Cluster::SetupTask(const TaskPTR& task)
{
	TID tid = task->GetTaskId();
	string slot = task->GetSlot();
	//add to task set and heartbeat set
	m_CurrTask.insert(TASKS::value_type(tid, task));
	m_CurrTaskhbs.insert(TASKHBS::value_type(tid, m_TS));
	//used task
	m_FreeSlots.erase(slot);
	m_UsedSlots.insert(slot);
	//HUMMER_ASSERT(m_CurrTaskhbs.size() == m_CurrTask.size());
}

void hummer::Cluster::ServerUpdate(ServerPTR& server)
{
	if (!server.get())
	{
		return;
	}

	string des = server->GetServerDes();
	SERVERS::iterator sIter = m_Workers.find(des);
	if (sIter != m_Workers.end())
	{
		WORKSLOTS::iterator slotIter = m_WorkSlots.find(des);
		HUMMER_ASSERT(slotIter != m_WorkSlots.end());
		slotIter->second->Setup(server->GetServerDes(), server->CpuCore(), 0);	
	}
	else
	{
		m_Workers.insert(SERVERS::value_type(des, server));
		//init slot by servers
		SlotPTR slotptr(new(std::nothrow) CWorkSlot());
		GenerateSlot(server, slotptr);
		m_WorkSlots.insert(WORKSLOTS::value_type(des, slotptr));
		m_FreeSlots.insert(slotptr->Slot());
	}

	return;
}

void hummer::Cluster::ServerDead(const string& serverdes)
{
	m_Workers.erase(serverdes);
	m_WorkSlots.erase(serverdes);
	m_FreeSlots.erase(serverdes);
	m_UsedSlots.erase(serverdes);
	return;
}

bool hummer::Cluster::GenerateJID(JID& jid)
{
	if (m_Generator && m_Generator->JobId(jid))
	{
		return true;
	}
	LOG_ERROR("Generate job id failed");
	return false;
}

bool hummer::Cluster::GenerateTID(TID& tid)
{
	if (m_Generator && m_Generator->TaskId(tid))
	{
		return true;
	}
	LOG_ERROR("Generate task id failed");
	return false;
}

size_t hummer::Cluster::FreeSlotNum()
{
	return m_FreeSlots.size();
}

bool hummer::Cluster::FreeSlot(size_t num, list<SlotPTR>& slot)
{
	size_t count = 0;
	FREE_SLOTS::iterator freeI = m_FreeSlots.begin();
	for (; freeI != m_FreeSlots.end(); ++freeI)
	{
		WORKSLOTS::iterator slotI = m_WorkSlots.find(*freeI);
		HUMMER_ASSERT(slotI != m_WorkSlots.end());
		slot.push_back(slotI->second);
		++count;
		if (count >= num)
		{
			break;
		}
	}
	return true;
}

void hummer::Cluster::Print()
{
	char buf[256] = {0};
	sprintf(buf, "Cluster_Statistic, server[Worker:%u,Slot:%u,FreeSlot:%u,UsedSlot:%u]"
		, m_Workers.size(), m_WorkSlots.size()
		, m_FreeSlots.size(), m_UsedSlots.size());
	LOG_TRACE(buf);

	memset(buf, 0, 256);
	sprintf(buf, "Cluster_Statistic, JOB[TotalJob:%u,RunningTask:%u,RedoJob:%u,FiniJob:%u]"
		, m_Jobs.size(), m_CurrTask.size()
		, m_RedoJob.size(), m_FiniJob.size());
	LOG_TRACE(buf);
	JOBS::iterator jIter = m_Jobs.begin();
	for (; jIter != m_Jobs.end(); ++jIter)
	{
		jIter->second->Statistic();
	}
}


