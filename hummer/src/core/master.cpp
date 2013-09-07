#include "master.hpp"
#include "utildef.hpp"

hummer::CMaster::CMaster(): m_ConnFlag(false), m_InitFlag(false){}

hummer::CMaster::~CMaster(){}

hummer::CServerBase::ServerType hummer::CMaster::GetType()
{
	return hummer::CServerBase::SERVER_MASTER;
}

bool hummer::CMaster::DoInit()
{
	m_GlobalXmlSub.SetInfo(m_Conf.GetNs(), m_Conf.GetConfTId()
		, hummer::CHummerCtx::GLOBAL_CONF_XML, &m_Zk);
	m_GlobalXmlSub.SetServer(this);

	m_JobXmlSub.SetInfo(m_Conf.GetNs(), m_Conf.GetSubmitJobTId()
		, m_Conf.GetDirSubMod(), &m_Zk);
	m_JobXmlSub.SetServer(this);

	m_WorkerSub.SetInfo(m_Conf.GetNs(), m_Conf.GetWorkerTId()
		, m_Conf.GetDirSubMod(), &m_Zk);
	m_WorkerSub.SetServer(this);

	m_HBSub.SetInfo(m_Conf.GetNs(), m_Conf.GetTaskHeartbeatTId()
		, m_Conf.GetDirSubMod(), &m_Zk);
	m_HBSub.SetServer(this);

	m_Assigner.SetInfo(m_Conf.GetNs(), m_Conf.GetTaskTId()
		, m_Conf.GetDirSubMod(), &m_Zk);
	m_Commiter.SetInfo(m_Conf.GetNs(), m_Conf.GetCommitJobTId()
		, m_Conf.GetDirSubMod(), &m_Zk);

	m_IdGenerator.SetInfo(&m_Zk, m_Conf.GetNs()
		, JOB_COUNTER, TASK_COUNTER);

	if (!m_Zk.Sub(&m_GlobalXmlSub) 
			|| !m_Zk.Sub(&m_JobXmlSub)
			|| !m_Zk.Sub(&m_WorkerSub)
			|| !m_Zk.Sub(&m_HBSub))
	{
		LOG_ERROR("reg or sub failed");
		return false;
	}
	
	if (!m_Cluster.Init(&m_IdGenerator))
	{
		return false;
	}
	return true;
}

void hummer::CMaster::DoRun()
{
	m_IdGenerator.CheckCounter();
}

void hummer::CMaster::DoUnInit()
{
	m_Cluster.UnInit();
	return;
}

bool hummer::CMaster::ConsumeCmd(CmdPTR& cmd)
{
	bool rc = false;
	switch(cmd->GetType())
	{
	case Cmd::CMD_CONN:
		rc = ProcessConn(cmd);
		break;
	case Cmd::CMD_XML:
		rc = ProcessXml(cmd);
		break;
	case Cmd::CMD_SERVER:
		rc = ProcessServerModify(cmd);
		break;
	case Cmd::CMD_TASKHB:
		rc = ProcessTaskHB(cmd);
		break;
	case Cmd::CMD_NETERR:
		rc = ProcessNetErr(cmd);
		break;
	default:
		return false;
	}
	return rc;
}

void hummer::CMaster::DoUnconnected()
{
	CmdPTR neterrptr(new(std::nothrow) CNetErrCmd());
	if (!neterrptr.get() || !PushCmd(neterrptr))
	{
		LOG_ERROR("Push conn cmd failed");
	}
}

void hummer::CMaster::DoConnected()
{
	CmdPTR connptr(new(std::nothrow) ConnCmd());
	if (!connptr.get() || !PushCmd(connptr))
	{
		LOG_ERROR("Push conn cmd failed");
	}
}

bool hummer::CMaster::ProcessConn(CmdPTR& cmd)
{
	HUMMER_NOUSE(cmd);
	m_ConnFlag = true;
	if (!RegisterServer())
	{
		LOG_ERROR("master register failed");
		return false;
	}
	else
	{
		LOG_TRACE("master connected");
	}

	PullDirs();
	//init all sub-module
	m_InitFlag = true;
	return true;
}

bool hummer::CMaster::ProcessNetErr(CmdPTR& cmd)
{
	HUMMER_NOUSE(cmd);
	m_Cluster.Clean();
	m_InitFlag = false;
	m_ConnFlag = false;
	return true;
}

bool hummer::CMaster::RegisterServer()
{
	LOG_TRACE("Master des:"<<m_ServerDes);
	if (m_Zk.exist(m_Conf.GetNs()
			, TOPIC_MASTER 
			, m_ServerDes.c_str()))
	{
		LOG_ERROR("master is alreayd exist");
		return false;
	}
	else
	{
		LOG_TRACE("master is not exist,push{"<<TOPIC_MASTER<<","<<m_ServerDes<<"}");
	}

	return m_Zk.push(m_Conf.GetNs(), TOPIC_MASTER, m_ServerDes.c_str(), NULL, 0);
}

bool hummer::CMaster::ProcessXml(CmdPTR& cmd)
{
	CXmlCmd* xmlcmd = dynamic_cast<CXmlCmd*>(cmd.get());
	if (!xmlcmd)
	{
		LOG_ERROR("convert to xml cmd failed");
		return false;
	}
	
	if (CXmlCmd::XML_GLOBAL == xmlcmd->GetXmlType())
	{
		if (ACT_ADD == xmlcmd->GetActType()
			|| ACT_MOD == xmlcmd->GetActType())
		{
			return m_Conf.LoadMemGlobalConf((const char*)(xmlcmd->GetBuf()), xmlcmd->GetLen());
		}
		else
		{
			LOG_TRACE("global configure file is del");
		}
	}
	else if (CXmlCmd::XML_JOB == xmlcmd->GetXmlType())
	{
		if (ACT_ADD == xmlcmd->GetActType())
		{
			return ProcessJobSubmit(xmlcmd);
		}
		else if (ACT_DEL == xmlcmd->GetActType())
		{
			return ProcessJobDel(xmlcmd);
		}
		else
		{
			LOG_TRACE("ignore submit_job modify");
		}
	}
	else
	{
		LOG_ERROR("xml file type is invalid");
		return false;
	}

	return true;
}

bool hummer::CMaster::ProcessTaskHB(CmdPTR& cmd)
{
	CTaskHBCmd* taskhb = dynamic_cast<CTaskHBCmd*>(cmd.get());
	if (!m_InitFlag || !taskhb)
	{
		LOG_ERROR("uninit or convert to hb cmd failed");
		return false;
	}

	JobPTR modJob;
	set<TaskPTR> delTasks;	
	ActType type = taskhb->GetActType();
	if (ACT_ADD == type 
		|| ACT_MOD == type)
	{
		TaskhbPTR hbptr(new(std::nothrow) CTaskHB());
		if (!hbptr.get() 
			|| !hbptr->ConvertByMsg(taskhb->GetMsg()))
		{
			LOG_ERROR("convert to taskhb failed");
			return false;
		}
		m_Cluster.TaskHB(hbptr, modJob, delTasks);	
	}
	else if (ACT_DEL == type)
	{
		int tid = atoi(taskhb->TaskHBName().c_str());
		LOG_TRACE("heartbeat del, tid="<<tid);
		m_Cluster.TaskDel(tid, modJob, delTasks);
	}
	else
	{
		LOG_ERROR("Act type err");
		return false;
	}

	if (delTasks.size() > 0)
	{
		m_Assigner.StopTask(delTasks);
	}

	if (modJob.get())
	{
		m_Commiter.CommitJob(modJob);	
	}

	return true;
}

bool hummer::CMaster::ProcessServerModify(CmdPTR& cmd)
{
	CServerCmd* servercmd = dynamic_cast<CServerCmd*>(cmd.get());
	if (!m_InitFlag)
	{
		LOG_ERROR("uninit or server is null");
		return false;
	}

	if (ACT_ADD == servercmd->GetActType()
		|| ACT_MOD == servercmd->GetActType())
	{
		ServerPTR serverptr(new(std::nothrow) CServer());
		if (!serverptr.get()
			|| !serverptr->ConvertByMsg(servercmd->GetMsg()))
		{
			LOG_ERROR("init server from server cmd failed");
			return false;
		}

		m_Cluster.ServerUpdate(serverptr);
		return true;
	}
	else if (ACT_DEL == servercmd->GetActType())
	{
		LOG_TRACE("ServerDel: "<<servercmd->ServerDes());
		m_Cluster.ServerDead(servercmd->ServerDes());
		return true;
	}
	else
	{
		LOG_ERROR("the act type of server is err");
		return false;
	}
}

bool hummer::CMaster::ProcessJobSubmit(CXmlCmd* cmd)
{
	LOG_TRACE("Process job submit");
	HUMMER_ASSERT(cmd);

	JobPTR jobptr(new(std::nothrow) CJob);
	JID jid = 0;
	string jobdes;
	if (jobptr.get() 
		&& ParseJobdesFromXmlname(jobdes, cmd->XmlName())
		&& m_Cluster.GenerateJID(jid)
		&& jobptr->LoadXml(jobdes.c_str(), (const char*)cmd->GetBuf(), cmd->GetLen()))
	{
		jobptr->Assign(jid);
	}
	else
	{
		LOG_ERROR("Convert jobxml to job failed");
		return false;
	}

	set<TaskPTR> tasks;
	if (!m_Cluster.Schedule(jobptr, tasks))
	{
		return false;	
	}

	if (!m_Assigner.AssignTask(tasks)
		|| !m_Commiter.CommitJob(jobptr))
	{
		m_Cluster.Rollback(jobptr, tasks);
		return false;
	}
	else
	{
		m_Cluster.Commit(jobptr, tasks);
		return true;
	}
}

bool hummer::CMaster::ProcessJobDel(CXmlCmd* cmd)
{
	LOG_TRACE("Process JOB Del");
	HUMMER_ASSERT(cmd);
	string jobdes;
	if (!ParseJobdesFromXmlname(jobdes, cmd->XmlName()))
	{
		LOG_ERROR("Parse jobdes failed");
		return false;
	}

	JobPTR job;
	set<TaskPTR> tasks;
	if (!m_Cluster.DelJob(jobdes, job, tasks))
	{
		LOG_ERROR("Deljob failed,"<<jobdes);
		return false;
	}

	if (tasks.size() > 0)
	{
		m_Assigner.StopTask(tasks);
	}

	if (job.get())
	{
		m_Commiter.RollbackJob(job);
	}

	return true;
}

bool hummer::CMaster::ParseJobdesFromXmlname(string& jobdes, const string& xml)
{
	string::size_type pos = xml.find(".xml");
	if (string::npos == pos)
	{
		return false;
	}
	
	jobdes = xml.substr(0, pos);
	LOG_TRACE("parse xml:"<<xml<<",jobdes="<<jobdes);
	return true;
}

void hummer::CMaster::PullDirs()
{
	CmdPTR globalCmd;
	if (!m_GlobalXmlSub.PullPer(m_Conf.GetGlobalConf(), globalCmd))
	{
		LOG_TRACE("pull global xml failed");
	}
	else
	{
		ProcessXml(globalCmd);
	}

	CmdPTR submitJobsCmd;
	CmdPTR commitJobsCmd;
	CmdPTR tasksCmd;
	if (!m_JobXmlSub.PullChilds(submitJobsCmd))
	{
		LOG_TRACE("no submit_job");
	}
	if (!m_Commiter.PullChilds(commitJobsCmd))
	{
		LOG_TRACE("no commit_job");
	}
	if 	(!m_Assigner.PullChilds(tasksCmd))
	{
		LOG_TRACE("no task");
	}

	RecoverAssign(commitJobsCmd, submitJobsCmd, tasksCmd);	

	CmdPTR workersCmd;
	if (!m_WorkerSub.PullChilds(workersCmd))
	{
		LOG_TRACE("pull computers failed");
	}
	else
	{
		RecoverServer(workersCmd);	
	}

	return;
}

bool hummer::CMaster::RecoverAssign(CmdPTR& job, CmdPTR& submitJob, CmdPTR& task)
{
	LOG_TRACE("******Master Recover*******");
	CMsgContainer* pjob = dynamic_cast<CMsgContainer*>(job.get());
	CmdContainer* pSubJob = dynamic_cast<CmdContainer*>(submitJob.get());
	CMsgContainer* ptask = dynamic_cast<CMsgContainer*>(task.get());
	//map taskid to CMcsTask
	set<TaskPTR> tasks;
	map<string, CmdPTR> SubmitJobs;
	map<string, JobPTR> commitJobs;
	if (!ptask 
		|| CMsgContainer::TEXT_TASK != ptask->GetTextType() 
		|| !ParseTask(ptask, tasks))
	{
		LOG_TRACE("NO TASK");
	}

	if (!pSubJob 
		|| CmdContainer::CMD_TYPE_SUBMITXML != pSubJob->GetTextType()
		|| !ParseSubmitJob(pSubJob, SubmitJobs))
	{
		LOG_TRACE("NO submit job");
	}

	if (!pjob 
		|| CMsgContainer::TEXT_COMMIT_JOB != pjob->GetTextType()
		|| !ParseCommitJob(pjob, commitJobs))
	{
		LOG_TRACE("NO commit job");
	}

	set<JobPTR> liveJobs;
	set<JobPTR> deadJobs;
	map<string, JobPTR>::iterator jIter = commitJobs.begin();
	for (; jIter != commitJobs.end(); )
	{
		map<string, CmdPTR>::iterator fIter =	SubmitJobs.find(jIter->first);
		if (fIter != SubmitJobs.end())
		{
			liveJobs.insert(jIter->second);
			SubmitJobs.erase(fIter);
		}
		else
		{
			deadJobs.insert(jIter->second);
		}
		++jIter;
	}

	map<string, CmdPTR>::iterator subIter = SubmitJobs.begin();
	for (; subIter != SubmitJobs.end(); ++subIter)
	{
		CXmlCmd* xml = dynamic_cast<CXmlCmd*>(subIter->second.get());
		JobPTR jobptr(new(std::nothrow) CJob);
		JID jid = 0;
		if (jobptr.get() && xml && m_Cluster.GenerateJID(jid)
			&& jobptr->LoadXml(subIter->first.c_str(), (const char*)(xml->GetBuf()), xml->GetLen()))
		{
			jobptr->Assign(jid);
			liveJobs.insert(jobptr);
		}
		else
		{
			LOG_ERROR("Submit jobxml convert to job failed");
		}
	}

	set<TaskPTR> deadTask;
	if (!m_Cluster.ScheduleRecover(liveJobs, tasks, deadTask))
	{
		LOG_ERROR("recover schedule failed");
		return false;
	}

	LOG_TRACE("Invalid task num="<<deadTask.size());
	LOG_TRACE("Invalid job num="<<deadJobs.size());
	LOG_TRACE("valid comm_job num="<<liveJobs.size());
	if (deadJobs.size() > 0)
	{
		set<JobPTR>::iterator dIter = deadJobs.begin();
		for (; dIter != deadJobs.end(); ++dIter)
		{
			m_Commiter.RollbackJob(*dIter);
		}
	}

	if (deadTask.size() > 0)
	{
		m_Assigner.StopTask(deadTask);
	}

	return true;	
}

bool hummer::CMaster::RecoverServer(CmdPTR& cmd)
{
	CMsgContainer* containerCmd = dynamic_cast<CMsgContainer*>(cmd.get());
	if (!containerCmd
		|| CMsgContainer::TEXT_SERVER != containerCmd->GetTextType())
	{
		LOG_ERROR("master uninit");	
		return false;
	}

	map<string, MsgPTR>::iterator msgIter = containerCmd->Msgs().begin();	
	for (; msgIter != containerCmd->Msgs().end(); ++msgIter)
	{
		ServerPTR serverptr(new(std::nothrow) CServer());
		if (serverptr->ConvertByMsg(msgIter->second))
		{
			m_Cluster.ServerUpdate(serverptr);
		}
		else
		{
			LOG_ERROR("init server failed");
		}
	}
	
	return true;
}

bool hummer::CMaster::ParseSubmitJob(CmdContainer* container
			, map<string, CmdPTR>& submits)
{
	 map<string, CmdPTR>::iterator cIter = container->Cmds().begin();	
	for (; cIter != container->Cmds().end(); ++cIter)
	{
		string jobdes;
		CXmlCmd* xml = dynamic_cast<CXmlCmd*>(cIter->second.get());
		if (xml && ParseJobdesFromXmlname(jobdes, xml->XmlName()))
		{
			submits.insert(map<string, CmdPTR>::value_type(jobdes, cIter->second));	
		}
	}

	return true;
}

bool hummer::CMaster::ParseTask(CMsgContainer* container
		, set<TaskPTR>& tasks)
{
	map<string, MsgPTR>::iterator msgIter = container->Msgs().begin();	
	for (; msgIter != container->Msgs().end(); ++msgIter)
	{
		TaskPTR taskptr(new(std::nothrow) CTask());
		if (!msgIter->second.get()
			|| !taskptr.get()
			|| !taskptr->ConvertByMsg(msgIter->second))
		{
			LOG_ERROR("Convert to mcs::task faild");
			continue;
		}

		tasks.insert(taskptr);
	}

	return true;
}

bool hummer::CMaster::ParseCommitJob(CMsgContainer* container
		, map<string, JobPTR>& commits)
{
	map<string, MsgPTR>::iterator msgIter = container->Msgs().begin();
	for (; msgIter != container->Msgs().end(); ++msgIter)
	{
		JobPTR jobptr(new(std::nothrow) CJob());
		msgIter->second.get();
		if (!msgIter->second.get()
			|| !jobptr.get()
			|| !jobptr->ConvertByMsg(msgIter->second))
		{
			LOG_ERROR("Convert to job failed");
			continue;
		}
		jobptr->Recover();
		commits.insert(map<string, JobPTR>::value_type(msgIter->first, jobptr));
	}
	return true;
}

void hummer::CMaster::Timer()
{
	if (!m_ConnFlag)
	{
		LOG_TRACE("master unconnected with zk");
		return;
	}

	if (!m_InitFlag)
	{
		RegisterServer();
		return;	
	}

	set<TaskPTR> delTasks;
	m_Cluster.TimeOutCheck(delTasks);
	if (delTasks.size() > 0)
	{
		m_Assigner.StopTask(delTasks);	
	}

	list<JobPTR> needRedoJobs;
	m_Cluster.GetNeedRedoJob(needRedoJobs);
	if (needRedoJobs.size() > 0 
		&& m_Cluster.FreeSlotNum() > 0)
	{
		size_t reScheduleOk = 0;
		LOG_TRACE("re-schedule num="<<needRedoJobs.size());
		list<JobPTR>::iterator jIter = needRedoJobs.begin();
		for (; jIter != needRedoJobs.end(); ++jIter)
		{
			set<TaskPTR> tasks;
			if (m_Cluster.ReSchedule(*jIter, tasks))
			{	
				if (!m_Assigner.AssignTask(tasks)
					|| !m_Commiter.CommitJob(*jIter))
				{
					m_Cluster.Rollback(*jIter, tasks);
				}
				else
				{
					m_Cluster.Commit(*jIter, tasks);
					++reScheduleOk;
				}
			}
		}
		LOG_TRACE("re-schedule ok num="<<reScheduleOk);
	}

	m_Cluster.Print();
}

const char* hummer::CMaster::JOB_COUNTER = "jobcounter";
const char* hummer::CMaster::TASK_COUNTER = "taskcounter";

