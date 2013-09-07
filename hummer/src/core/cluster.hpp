/*****************************************************
 * Copyright zhengyang   
 * FileName: cluster.hpp 
 * Description: the cluster manager of hummer 
 * Author: zhengyang
 * Date: 2013.08.10
 * *****************************************************/
#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "server.hpp"
#include "taskhb.hpp"
#include "scheduler.hpp"

#include "job.hpp"
#include "task.hpp"
#include "taskhb.hpp"
#include "workslot.hpp"
#include "zkactor.hpp"

#include <string>
#include <map>
using std::string;
using std::map;

namespace hummer
{

class Cluster
{
public:
	Cluster();
	~Cluster();
	bool Init(CIdGenerator* idgenerator);
	void Clean();
	void UnInit();
	void TimeOutCheck(set<TaskPTR>& delTasks);
	bool ServerRecover(set<ServerPTR>& servers);
	bool ScheduleRecover(set<JobPTR>& jobs, set<TaskPTR>& tasks
			, set<TaskPTR>& delTasks);

	bool Schedule(JobPTR& job, set<TaskPTR>& tasks);
	void Commit(JobPTR& job, set<TaskPTR>& tasks);
	void Rollback(JobPTR& job, set<TaskPTR>& tasks);

	bool TaskHB(TaskhbPTR& thb, JobPTR& modJob, set<TaskPTR>& delTasks);
	bool TaskDel(TID tid, JobPTR& modJob, set<TaskPTR>& delTasks);
	bool DelJob(const string& jobdes, JobPTR& delJob, set<TaskPTR>& delTasks);
	void GetNeedRedoJob(list<JobPTR>& jobs);
	bool ReSchedule(JobPTR& job, set<TaskPTR>& assignTasks);

	void ServerUpdate(ServerPTR& server);
	void ServerDead(const string& serverdes);
	size_t FreeSlotNum();
	bool FreeSlot(size_t num, list<SlotPTR>& slot);	
	bool GenerateJID(JID& jid);
	bool GenerateTID(TID& tid);

	void Print();
private:
	void GenerateSlot(const ServerPTR& sever, SlotPTR& slot);
	void SetupTask(const TaskPTR& commitTask);
	void ReleaseTask(TID tid, TaskPTR& delTask);
	void CheckJob();
	void CheckPerJob(JobPTR& job);
	SchedulerPTR& ChooseScheduler(JobPTR& job);
	enum
	{
		TASK_TIMEOUT = 30000
	};
private:
	uint64_t m_TS;
	typedef map<string, ServerPTR> SERVERS;
	typedef map<JID, JobPTR> JOBS;
	typedef map<string, JobPTR> DESJOBS;
	typedef map<TID, TaskPTR> TASKS;
	typedef map<TID, uint64_t> TASKHBS;
	typedef map<string, SlotPTR> WORKSLOTS;
	typedef set<string> FREE_SLOTS;
	typedef set<string> USED_SLOTS;
	SERVERS m_Workers;
	WORKSLOTS m_WorkSlots;
	FREE_SLOTS m_FreeSlots;
	USED_SLOTS m_UsedSlots;

	JOBS m_Jobs;
	DESJOBS m_des2job;
	TASKS m_CurrTask;
	TASKHBS m_CurrTaskhbs;
	set<JID> m_RedoJob;
	set<JID> m_FiniJob;

	vector<SchedulerPTR> m_Schedulers;
	CIdGenerator* m_Generator;
};

};

#endif

