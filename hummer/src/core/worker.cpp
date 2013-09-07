#include "worker.hpp"
#include "hummerctx.hpp"
#include "utildef.hpp"
#include "taskcontainer.hpp"
#include "tasker.hpp"
#include "taskhb.hpp"

hummer::CWorker::CWorker()
: m_FirstInitFlag(false), m_InitFlag(false), m_GetConfFlag(false){}

hummer::CWorker::~CWorker(){}

bool hummer::CWorker::ConsumeCmd(CmdPTR& cmd)
{
	bool rc = false;
	switch(cmd->GetType())
	{
	case Cmd::CMD_XML:
		rc = ProcessXml(cmd);
		break;
	case Cmd::CMD_TASK:
		rc = ProcessTask(cmd);
		break;
	case Cmd::CMD_TASKHB:
		rc = ProcessTaskhb(cmd);
		break; 
	case Cmd::CMD_NETERR:
		rc = ProcessNetErr(cmd);
		break;
	case Cmd::CMD_CONN:
		rc = ProcessConn(cmd);
		break;
	default:
		return -1;
	}
	return rc;
}

void hummer::CWorker::Timer()
{
	if (!m_InitFlag || !m_GetConfFlag)
	{
		RegisterServer();
	}
	LOG_TRACE("worker time:Running task num="<<m_Tasks.size());
}

bool hummer::CWorker::DoInit()
{
	m_XmlSub.SetInfo(m_Conf.GetNs(), m_Conf.GetConfTId()
		, hummer::CHummerCtx::WORKER_CONF, &m_Zk);
	m_XmlSub.SetServer(this);
	m_TaskSub.SetInfo(m_Conf.GetNs(), m_Conf.GetTaskTId()
		, m_ServerDes.c_str(), &m_Zk);
	m_TaskSub.SetServer(this);

	m_ThbPusher.SetInfo(m_Conf.GetNs()
		, m_Conf.GetTaskHeartbeatTId(), "", &m_Zk);
	m_ServerPusher.SetInfo(m_Conf.GetNs()
		, m_Conf.GetWorkerTId(), "", &m_Zk);

	if (!m_Zk.Sub(&m_XmlSub)
		|| !m_Zk.Sub(&m_TaskSub))
	{
		return false;
	}

	return true;
}

void hummer::CWorker::DoRun()
{
	//do nothing
}

void hummer::CWorker::DoUnInit()
{
	//do nothing
}

void hummer::CWorker::DoUnconnected()
{
	CmdPTR neterrptr(new(std::nothrow) CNetErrCmd());
	if (!neterrptr.get() || !PushCmd(neterrptr))
	{
		LOG_ERROR("Push unconn cmd failed");
	}
}

void hummer::CWorker::DoConnected()
{
	CmdPTR connptr(new(std::nothrow) ConnCmd());
	if (!connptr.get() || !PushCmd(connptr))
	{
		LOG_ERROR("Push conn cmd failed");
	}
}

bool hummer::CWorker::ProcessConn(CmdPTR& cmd)
{
	HUMMER_NOUSE(cmd);
	HUMMER_ASSERT(m_Tasks.size() == 0);
	if (!RegisterServer())
	{
		LOG_ERROR("register failed");
		return false;
	}

	CmdPTR taskptr;
	if (!m_TaskSub.PullPer(m_ServerDes, taskptr)
		|| !ProcessTask(taskptr))
	{
		LOG_TRACE("no task while connected");
	}

	return true;
}

bool hummer::CWorker::ProcessNetErr(CmdPTR& cmd)
{
	HUMMER_NOUSE(cmd);
	m_GetConfFlag = false;
	m_InitFlag = false;
	ReleaseAllTasker();
	return true;
}

bool hummer::CWorker::RegisterServer()
{
	CmdPTR workXml;
	CmdPTR globalXml;
	if (!m_GetConfFlag)
	{
		if (!m_XmlSub.PullPer(hummer::CHummerCtx::WORKER_CONF,workXml)
			|| !ProcessXml(workXml)
			|| !m_XmlSub.PullPer(hummer::CHummerCtx::GLOBAL_CONF_XML, globalXml)
			|| !ProcessXml(globalXml))
		{
			return false;
		}	
		m_GetConfFlag = true;		
	}

	if (CheckExist())
	{
		if (!m_FirstInitFlag)
		{
			LOG_TRACE("worker describer is repeated");
			return false;
		}
	}
	else
	{
		if (ServerUpdate())
		{
			m_FirstInitFlag = true;
		}
	}

	m_InitFlag = true;
	return true;	
}

void hummer::CWorker::ReleaseAllTasker()
{
	TASK_SLOTS::iterator tIter = m_Tasks.begin();
	for (; tIter != m_Tasks.end(); ++tIter)
	{
		if (!tIter->second)
		{
			continue;
		}

		tIter->second->Clean();
		delete tIter->second;
	}
}

bool hummer::CWorker::ProcessXml(CmdPTR& cmd)
{
	CXmlCmd* xml = dynamic_cast<CXmlCmd*>(cmd.get());
	if (NULL == xml)
	{
		LOG_ERROR("convert to xml failed");
		return false;
	}

	if (ACT_DEL == xml->GetActType())
	{
		return true;
	}

	bool rc = false;
	if (CXmlCmd::XML_GLOBAL ==  xml->GetXmlType())
	{
		rc = m_Conf.LoadMemGlobalConf((const char*)(xml->GetBuf())
				, xml->GetLen());
		LOG_TRACE("Load global xml,"<<rc);
	}
	else if (CXmlCmd::XML_WORKER == xml->GetXmlType())
	{
		rc = m_Conf.LoadMemWorkerConf((const char*)(xml->GetBuf())
				, xml->GetLen());	
		if (!rc || !m_Conf.WorkerHasDes(m_ServerDes))
		{
			LOG_ERROR("load xml err, or no find me in global conf,"<<rc);
			return false;
		}
		LOG_TRACE("Load worker xml,"<<rc);
	}
	else
	{
		LOG_ERROR("xml type is err");
	}
	return rc;
}

bool hummer::CWorker::ProcessTask(CmdPTR& cmd)
{
	CTaskCmd* ptask = dynamic_cast<CTaskCmd*>(cmd.get());
	if (!m_InitFlag || !ptask)
	{
		LOG_TRACE("Worker uninit");
		return false;
	}

	ActType at = ptask->GetActType();
	if (ACT_DEL == at)
	{
		LOG_TRACE("Del task");
		//delete the taskcontainer
		TASK_SLOTS::iterator tIter = m_Tasks.begin();
		for (; tIter != m_Tasks.end(); ++tIter)
		{
			tIter->second->Clean();
			ProcessTaskhbDel(tIter->first);
			delete tIter->second;
		}
		m_Tasks.clear();
		return true;
	}
	else if (ACT_ADD == at)
	{
		TaskPTR task(new(std::nothrow) hummer::CTask());
		if (!task.get() || !task->ConvertByMsg(ptask->GetMsg()))
		{
			LOG_ERROR("get task from msg err");
			return false;
		}
		TID tid = task->GetTaskId();
		LOG_TRACE("assign task, task="<<tid);
		//create taskcontainer
		HUMMER_ASSERT(m_Tasks.end() == m_Tasks.find(tid));
		CTaskContainer* container = new(std::nothrow) CTaskContainer();
		if (!container)
		{
			LOG_ERROR("lack memory while create container");
			return false;
		}

		if (!container->PlugTask(task, this)
			|| !container->Excute())
		{
			container->Clean();
			delete container;
			container = NULL;
			LOG_ERROR("Setup tasker failed");
			return false;
		}

		m_Tasks.insert(TASK_SLOTS::value_type(tid, container));	
	}
	else
	{
		//ACT_MOD do not support
		return false;
	}

	return true;
}

bool hummer::CWorker::CheckExist()
{
	return m_ServerPusher.Exist(m_ServerDes);	
}

bool hummer::CWorker::ServerUpdate()
{
	int portUpper = 0;
	int portDown = 0;
	int cpu = 0;
	if (!m_Conf.GetWorkerParm(m_ServerDes, CHummerCtx::WORKER_PORTUPPER, portUpper)
		|| !m_Conf.GetWorkerParm(m_ServerDes, CHummerCtx::WORKER_PORTDOWN, portDown)
		|| !m_Conf.GetWorkerParm(m_ServerDes, CHummerCtx::WORKER_CPU, cpu))
	{
		LOG_ERROR("load worker parm failed");
		return false;
	}

	ServerPTR serverptr(new(std::nothrow) CServer());
	if (!serverptr.get())
	{
		LOG_ERROR("lack memory");
		return false;
	}
	set<BusinessType> busis;
	CTasker::GetSupportBusiness(busis);

	serverptr->SetServerDes(m_ServerDes);
	serverptr->SetCpuCore(cpu);
	LOG_TRACE("PortUpper:"<<portUpper<<",Down:"<<portDown);
	serverptr->SetPortRange(portUpper, portDown);
	serverptr->SetBusiness(busis);
	return m_ServerPusher.Update(serverptr);	
}

bool hummer::CWorker::ProcessTaskhb(CmdPTR& cmd)
{
	CTaskHBCmd* taskCmd = dynamic_cast<CTaskHBCmd*>(cmd.get());
	if (cmd->GetType() != Cmd::CMD_TASKHB
		|| !taskCmd)
	{
		LOG_ERROR("heartbeat cmd is err");
		return false;
	}

	TaskhbPTR taskhb(new(std::nothrow) CTaskHB());
	if (!taskhb->ConvertByMsg(taskCmd->GetMsg()))
	{
		HUMMER_ASSERT(false);
	}

	return m_ThbPusher.HB(taskCmd->GetMsg());
}

void hummer::CWorker::ProcessTaskhbDel(TID tid)
{
	m_ThbPusher.DelHB(tid);	
}

hummer::CServerBase::ServerType hummer::CWorker::GetType()
{
	return hummer::CServerBase::SERVER_WORKER;
}


