/*****************************************************
 * Copyright zhengyang   
 * FileName: master.hpp 
 * Description: master monitor server
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef MASTER_HPP
#define MASTER_HPP

#include "serverbase.hpp"
#include "msg.hpp"
#include "cluster.hpp"
#include "zkactor.hpp"

namespace hummer
{

class CMaster : public CServerBase
{
public:
	CMaster();
	~CMaster();

	bool ConsumeCmd(CmdPTR& cmd);
	void Timer();
	bool DoInit();
	void DoRun();
	void DoUnInit();
	void DoUnconnected();
	void DoConnected();
	ServerType GetType();
private:
	bool ProcessConn(CmdPTR& cmd);
	bool ProcessNetErr(CmdPTR& cmd);
	bool ProcessXml(CmdPTR& cmd);
	bool ProcessTaskHB(CmdPTR& cmd);
	bool ProcessServerModify(CmdPTR& cmd);

	bool RegisterServer();
	bool ProcessJobSubmit(CXmlCmd* cmd);
	bool ProcessJobDel(CXmlCmd* cmd);
	bool RecoverServer(CmdPTR& cmd);
	bool RecoverAssign(CmdPTR& job, CmdPTR& submitJob, CmdPTR& task);

	void PullDirs();
	bool ParseJobdesFromXmlname(string& jobdes, const string& xml);
	bool ParseSubmitJob(CmdContainer* container, map<string, CmdPTR>& submits);
	bool ParseTask(CMsgContainer* container, set<TaskPTR>& tasks);
	bool ParseCommitJob(CMsgContainer* container, map<string, JobPTR>& commits);

	static const char* JOB_COUNTER;
	static const char* TASK_COUNTER;
private:
	bool m_ConnFlag;
	bool m_InitFlag;
	Cluster m_Cluster;

	//subscribe the global_config.xml
	CXmlActor m_GlobalXmlSub;
	//subscribe the submit_job.xml
	CXmlActor m_JobXmlSub;
	//subscribe the data server
	CServerActor m_WorkerSub;
	//subscribe the task heartbeat
	CTaskHBActor m_HBSub;
	//assign task
	CTaskActor m_Assigner;
	//commit job
	CJobActor m_Commiter;
	CIdGenerator m_IdGenerator;
};

};

#endif

