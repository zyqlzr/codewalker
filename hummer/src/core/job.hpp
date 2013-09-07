/*****************************************************
 * Copyright zhengyang   
 * FileName: job.hpp 
 * Description: the definition of job
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef JOB_HPP
#define JOB_HPP

#include "msg.hpp"
#include "split.hpp"
#include "task.hpp"
#include "assignment.hpp"
#include "taskhb.hpp"

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <set>
#include <map>
#include <list>
using std::set;
using std::string;
using std::list;
using std::map;

namespace hummer
{

class CJob
{
public:
	CJob();
	~CJob();

	enum JobStatus
	{
		JOB_STATUS_INVALID = -1,
		JOB_STATUS_UNINIT = 0,
		JOB_STATUS_SUBMIT,
		JOB_STATUS_RUNNING,
		JOB_STATUS_PART,
		JOB_STATUS_FINI,
		JOB_STATUS_ERR,
		JOB_STATUS_MAX
	};

	bool LoadXml(const char* des, const char* xml, int len);
	void Assign(JID jid);
	void Recover();
	bool ConvertByMsg(MsgPTR& jobmsg);
	bool ConvertToMsg(MsgPTR& jobmsg);

	void Submit(JID jid, JobType jt
				, BusinessType busi, const string& jobDes
				, set<SplitPTR>& splits, set<string>& srcs
				, set<string>& dsts, int corenum);

	void Commit(set<TaskPTR>& tasks);
	void Rollback(set<TaskPTR>& tasks);

	JID GetJobId()const;
	JobType GetJobType()const;
	BusinessType GetBusi()const;
	JobStatus GetStatus()const;
	const string& GetDes()const;
	int CoreNum()const;
	const set<string>& GetSrc()const;
	const set<string>& GetDst()const;

	void AllSplit(list<SplitPTR>& all);
	void UnAssignSplit(list<SplitPTR>& unassigns);

	void GetRunningTaskID(set<TID>& ids)const;
	void GetAllTaskID(set<TID>& ids)const;
	void TaskErr(TID tid);
	void TaskFini(TID tid, TaskhbPTR& hbInfo);
	void TaskRun(const TaskPTR& task);
	bool CheckFini();
	void Print();
	void Statistic();
private:
	void DoSplitPre(const string& split, TaskhbPTR& hbInfo);
private:
	JID m_JobId;
	string m_JobDes;
	JobType m_JobType;
	BusinessType m_Busi;

	JobStatus m_status;
	set<string> m_SrcDes;
	set<string> m_DstDes;
	typedef map<string, SplitPTR> SPLITS;
	typedef map<TID, CAssignment> ASSIGNMENTS;
	SPLITS m_Splits;
	ASSIGNMENTS m_Assignments;
	set<string> m_RedoSplits;
	set<string> m_RunningSplits;
	int m_ExpectCore;
};

};

typedef boost::shared_ptr<hummer::CJob> JobPTR;

#endif

