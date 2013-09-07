#include "zkactor.hpp"
#include "Logger.h"
#include "utildef.hpp"
#include "pbdef.hpp"

#include "server.pb.h"
#include "job.pb.h"

#include "Logger.h"

/***********CTaskHBActor definition***********/
hummer::CTaskHBActor::CTaskHBActor(){}

hummer::CTaskHBActor::~CTaskHBActor(){}

int hummer::CTaskHBActor::DoSubActor(int type, const char* topic, char* buffer, size_t len)
{
	ActType actType = ACT_INVALID;
	switch(type)
	{
	case NT_TOPIC_CHILD_ADD:
		actType = ACT_ADD;
		break;
	case NT_TOPIC_CHILD_DEL:
		LOG_TRACE("*ZK_EVENT*,TaskHB,T:"<<topic<<",t:"<<type<<",l:"<<len);
		actType = ACT_DEL;
		break;
	case NT_TOPIC_CHILD_CHANGE:
		actType = ACT_MOD;
		break;
	default:
		LOG_TRACE("The type of actor is invalid");
		return -1;
	}

	CTaskHBCmd* cmd = new CTaskHBCmd();
	CmdPTR cmdptr(cmd);
	if (!cmd 
		|| !cmd->Stuff(topic, actType, buffer, len))
	{
		LOG_TRACE("new CTaskHBCmd failed");
		return -1;
	}

	if (!m_Server
		|| !m_Server->PushCmd(cmdptr))
	{
		LOG_TRACE("push command failed");
		return -1;
	}

	return 0;
}

bool hummer::CTaskHBActor::PullChilds(CmdPTR& cmd)
{
	if (!m_handle)
	{
		LOG_ERROR("pb in  is null");
		return false;
	}

	CMsgContainer* pcontainer = new(std::nothrow) CMsgContainer();
	CmdPTR containerptr(pcontainer);
	if (!pcontainer
		|| !pcontainer->Stuff(CMsgContainer::TEXT_TASKHB))
	{
		LOG_ERROR("create taskhb failed in pullChilds");
		return false;
	}

	set<string> childs;
	if (!(m_handle->PullChild(m_ns.c_str(), m_id, m_name.c_str(), childs)))
	{
		LOG_ERROR("pull childs from hbdir failed");
		return false;
	}

	BufPTR buf(new(std::nothrow) char[MAX_ZOO_DATA_LEN]);
	size_t len = MAX_ZOO_DATA_LEN;
	if (!buf.get())
	{
		LOG_ERROR("Malloc buffer failed");
		return false;
	}

	set<string>::iterator iter = childs.begin();
	for (; iter != childs.end(); ++iter)
	{
		len = MAX_ZOO_DATA_LEN;
		memset(buf.get(), 0, len);
		if (!m_handle->pull(m_ns.c_str(), m_id, (*iter).c_str()
				, buf.get(), len))
		{
			LOG_ERROR("poll "<<(*iter)<<" failed");
			continue;
		}

		hummer::taskHB* ptaskhb = new(std::nothrow) hummer::taskHB();		
		MsgPTR msgptr(ptaskhb);
		if (!ptaskhb)
		{
			LOG_ERROR("Create hb pb failed");
			continue;
		}

		if (!(ptaskhb->ParseFromArray(buf.get(), len)))
		{
			LOG_ERROR("taskhb pb parse data failed");
			continue;
		}

		LOG_TRACE("taskHB:"<<(*iter));
		pcontainer->Msgs().insert(map<string, MsgPTR>::value_type((*iter), msgptr));
	}
	LOG_TRACE("Load taskhb num = "<<pcontainer->Msgs().size());
	cmd = containerptr;
	return true;
}

bool hummer::CTaskHBActor::PullPer(const string& name, CmdPTR& data)
{
	HUMMER_NOUSE(name);
	HUMMER_NOUSE(data);
	return false;
}

bool hummer::CTaskHBActor::PullData(char* buf, size_t& max_buf_len)
{
	HUMMER_NOUSE(buf);
	HUMMER_NOUSE(max_buf_len);
	return false;
}

bool hummer::CTaskHBActor::HB(MsgPTR& heartbt)
{
	hummer::taskHB* hb = dynamic_cast<hummer::taskHB*>(heartbt.get());	
	if (!m_handle || !hb)
	{
		LOG_ERROR("handle null or hb msg err");
		return false;
	}
	
	char hbstr[128] = {0};
	sprintf(hbstr, "%u", hb->taskid());
	//report heart beat
	string hbdata;
	if (!heartbt->SerializeToString(&hbdata))
	{
		LOG_TRACE("serialize taskheartbeat failed");
		return false;
	}
	//LOG_TRACE("task heartbeat, len="<<hbdata.size()<<
	//",jid="<<hb->jobid()<<",tid="<<hb->taskid()<<",status="
	//<<hb->status()<<",des="<<hb->serverdes());

	return m_handle->push(m_ns.c_str(), m_id, hbstr, (char*)hbdata.data(), hbdata.size());		
}

void hummer::CTaskHBActor::DelHB(TID tid)
{
	char hbstr[128] = {0};
	sprintf(hbstr, "%lu", tid);

	if (m_handle)
	{
		if(!m_handle->del(m_ns.c_str(), m_id, hbstr))
		{
			LOG_ERROR("delete task hb failed,"<<tid<<","<<hbstr);
		}
		else
		{
			LOG_TRACE("delete task hd ok,"<<tid<<","<<hbstr);
		}
	}
}

/***********CServerActor definition***********/
hummer::CServerActor::CServerActor(){}

hummer::CServerActor::~CServerActor(){}

int hummer::CServerActor::DoSubActor(int type, const char* topic, char* buffer, size_t len)
{
	LOG_TRACE("*ZK_EVENT*,Server,T:"<<topic<<",t:"<<type<<",l:"<<len);
	ActType actType = ACT_INVALID;
	switch(type)
	{
	case NT_TOPIC_CHILD_ADD:
		actType = ACT_ADD;
		break;
	case NT_TOPIC_CHILD_DEL:
		actType = ACT_DEL;
		break;
	case NT_TOPIC_CHILD_CHANGE:
		actType = ACT_MOD;
		break;
	default:
		LOG_TRACE("The type of actor is invalid");
		return -1;
	}

	CServerCmd* cmd = new CServerCmd();
	if (!cmd 
		|| !cmd->Stuff(topic, actType, buffer, len))
	{
		LOG_TRACE("new CTaskHBCmd failed");
		return -1;
	}

	CmdPTR cmdptr(cmd);
	if (!m_Server
		|| !m_Server->PushCmd(cmdptr))
	{
		LOG_TRACE("push command failed");
		return -1;
	}
	else
	{
		LOG_TRACE("server change, des:"<<cmd->ServerDes());
	}

	return 0;
}

bool hummer::CServerActor::PullChilds(CmdPTR& cmd)
{
	if (!m_handle)
	{
		LOG_ERROR("pb in  is null");
		return false;
	}

	CMsgContainer* pcontainer = new(std::nothrow) CMsgContainer();
	CmdPTR containerptr(pcontainer);
	if (!pcontainer
		|| !pcontainer->Stuff(CMsgContainer::TEXT_SERVER))
	{
		LOG_ERROR("create taskhb failed in pullChilds");
		return false;
	}

	set<string> childs;
	if (!(m_handle->PullChild(m_ns.c_str(), m_id, m_name.c_str(), childs)))
	{
		LOG_ERROR("pull childs from serverdir failed,ns="<<m_ns<<",id="<<m_id<<",name"<<m_name);
		return false;
	}

	BufPTR buf(new(std::nothrow) char[MAX_ZOO_DATA_LEN]);
	size_t len = MAX_ZOO_DATA_LEN;
	if (!buf.get())
	{
		LOG_ERROR("malloc buffer failed");
		return false;
	}

	set<string>::iterator iter = childs.begin();
	for (; iter != childs.end(); ++iter)
	{
		len = MAX_ZOO_DATA_LEN;
		memset(buf.get(), 0, len);
		if (!m_handle->pull(m_ns.c_str(), m_id, (*iter).c_str()
				, buf.get(), len))
		{
			LOG_ERROR("poll "<<(*iter)<<" failed");
			continue;
		}

		MsgPTR msgptr(new(std::nothrow) hummer::server());
		if (!msgptr.get())
		{
			LOG_ERROR("Create server pb failed");
			continue;
		}
		
		if (!(msgptr->ParseFromArray(buf.get(), len)))
		{
			LOG_ERROR("server pb parse data failed");
			continue;
		}

		LOG_TRACE("Server:"<<(*iter));
		pcontainer->Msgs().insert(map<string, MsgPTR>::value_type((*iter), msgptr));	
	}
	LOG_TRACE("Load server pb num = "<<pcontainer->Msgs().size());
	cmd = containerptr;
	return true;
}

bool hummer::CServerActor::PullPer(const string& name, CmdPTR& data)
{
	HUMMER_NOUSE(name);
	HUMMER_NOUSE(data);
	return false;
}

bool hummer::CServerActor::PullData(char* buf, size_t& max_buf_len)
{
	HUMMER_NOUSE(buf);
	HUMMER_NOUSE(max_buf_len);
	return false;
}

bool hummer::CServerActor::Update(ServerPTR& server)
{
	MsgPTR serverreg;
	string infodata;
	if (!m_handle || !server.get()
		|| !server->ConvertToMsg(serverreg)
		|| !serverreg.get()
		|| !serverreg->SerializeToString(&infodata))
	{
		LOG_TRACE("serialize server info failed");
		return false;
	}

	return m_handle->push(m_ns.c_str(), m_id
		, server->GetServerDes().c_str()
		, (char*)infodata.data(), infodata.size());
}

bool hummer::CServerActor::Exist(const string& des)
{
	if (m_handle 
		&& m_handle->exist(m_ns.c_str(), m_id, des.c_str()))
	{
		return true;
	}
	else
	{
		return false;
	}		
}

/***********CXmlActor definition***********/
hummer::CXmlActor::CXmlActor(){}

hummer::CXmlActor::~CXmlActor(){}

int hummer::CXmlActor::DoSubActor(int type, const char* topic, char* buffer, size_t len)
{
	LOG_TRACE("*ZK_EVENT*,XML,T:"<<topic<<",t:"<<type<<",l:"<<len);
	if (!topic)
	{
		LOG_ERROR("topic is null in subactor");
		return -1;
	}

	CXmlCmd::XmlType xmlType = CXmlCmd::XML_INVALID;
	ActType actType = ACT_INVALID;
	if (m_id == TOPIC_SUBMIT_JOB)
	{
		LOG_TRACE("Submit job notify,"<<type<<","<<topic);
		xmlType = CXmlCmd::XML_JOB;
	}
	else if (m_name == hummer::CHummerCtx::GLOBAL_CONF_XML)
	{
		LOG_TRACE("Global xml notify,"<<type<<","<<topic);
		xmlType = CXmlCmd::XML_GLOBAL;
	}
	else if (m_name == hummer::CHummerCtx::WORKER_CONF)
	{
		LOG_TRACE("Worker xml notify,"<<type<<","<<topic);
		xmlType = CXmlCmd::XML_WORKER;
	}
	else
	{
		LOG_ERROR("xml topic is invalid,"<<m_id<<","<<m_name);
		return -1;
	}

	switch(type)
	{
	case NT_TOPIC_CHILD_ADD:
		actType = ACT_ADD;
		break;
	case NT_TOPIC_CREATE:
		actType = ACT_ADD;
		break;
	case NT_TOPIC_CHILD_DEL:
		actType = ACT_DEL;
		break;
	case NT_TOPIC_DEL:
		actType = ACT_DEL;
		break;
	case NT_TOPIC_UPDATE:
		actType = ACT_MOD;
		break;
	case NT_TOPIC_CHILD_CHANGE:
		actType = ACT_MOD;
		break;
	default:
		LOG_ERROR("The type of actor is invalid");
		return -1;
	}

	CXmlCmd* cmd = new CXmlCmd();
	CmdPTR xmlptr(cmd);
	if (!cmd 
		|| !cmd->Stuff(topic, xmlType 
			, actType, buffer, len))
	{
		LOG_ERROR("stuff xml failed,"<<topic);
		return -1;
	}

	if (!m_Server 
		|| !m_Server->PushCmd(xmlptr))
	{
		LOG_ERROR("push xml failed,"<<topic);
		return -1;
	}

	return 0;
}

bool hummer::CXmlActor::PullChilds(CmdPTR& cmd)
{
	if (!m_handle)
	{
		LOG_ERROR("pb in is null");
		return false;
	}

	CmdContainer* pcontainer = new(std::nothrow) CmdContainer();
	CmdPTR containerptr(pcontainer);
	if (!pcontainer
		|| !pcontainer->Stuff(CmdContainer::CMD_TYPE_SUBMITXML))
	{
		LOG_ERROR("create cmdcontainer failed in pullChilds");
		return false;
	}

	set<string> childs;
	if (!(m_handle->PullChild(m_ns.c_str(), m_id, m_name.c_str(), childs)))
	{
		LOG_ERROR("pull childs from serverdir failed,ns="<<m_ns<<",id="<<m_id<<",name="<<m_name);
		return false;
	}

	BufPTR buf(new(std::nothrow) char[MAX_ZOO_DATA_LEN]);
	size_t len = MAX_ZOO_DATA_LEN;
	if (!buf.get())
	{
		LOG_ERROR("Malloc buffer failed");
		return false;
	}

	set<string>::iterator iter = childs.begin();
	for (; iter != childs.end(); ++iter)
	{
		len = MAX_ZOO_DATA_LEN;
		memset(buf.get(), 0, len);
		if (!m_handle->pull(m_ns.c_str(), m_id, (*iter).c_str()
				, buf.get(), len))
		{
			LOG_ERROR("poll "<<(*iter)<<" failed");
			continue;
		}
		else
		{
			LOG_TRACE("submit_job:"<<buf.get());
		}

		CXmlCmd* xmlcmd = new(std::nothrow) CXmlCmd();	
		CmdPTR cmdptr(xmlcmd);
		if (!xmlcmd 
			|| !cmdptr.get())
		{
			LOG_ERROR("Create job xml cmd failed");
			continue;
		}
		
		if (!xmlcmd->Stuff((*iter).c_str()
				, CXmlCmd::XML_JOB, ACT_ADD, buf.get(), len))
		{
			LOG_ERROR("Stuff job xml cmd failed");
			continue;
		}

		LOG_TRACE("submit_job:"<<(*iter));
		pcontainer->Cmds().insert(map<string, CmdPTR>::value_type((*iter), cmdptr));	
	}
	LOG_TRACE("Load job pb num = "<<pcontainer->Cmds().size());
	cmd = containerptr;
	return true;
}

bool hummer::CXmlActor::PullPer(const string& name, CmdPTR& data)
{
	if (!m_handle)
	{
		return false;
	}

	BufPTR buf(new(std::nothrow) char[MAX_ZOO_DATA_LEN]);
	size_t len = MAX_ZOO_DATA_LEN;
	if (!buf.get())
	{
		LOG_ERROR("malloc buffer failed");
		return false;
	}

	memset(buf.get(), 0, len);
	if (!m_handle->pull(m_ns.c_str(), m_id, name.c_str()
				, buf.get(), len))
	{
		LOG_ERROR("poll "<<name<<" failed");
		return false;
	}

	CXmlCmd::XmlType xmlType = CXmlCmd::XML_INVALID;
	if (name == hummer::CHummerCtx::GLOBAL_CONF_XML)
	{
		xmlType = CXmlCmd::XML_GLOBAL;
	}
	else if (name == hummer::CHummerCtx::WORKER_CONF)
	{
		xmlType = CXmlCmd::XML_WORKER;
	}
	else
	{
		//do nothing
	}
	
	CXmlCmd* pxml = new(std::nothrow) CXmlCmd();
	CmdPTR xmlptr(pxml);
	if (!pxml
		|| !pxml->Stuff(name.c_str(), xmlType 
			, ACT_ADD, buf.get(), len))
	{
		LOG_ERROR("stuff xml cmd failed");
		return false;
	}
	else
	{
		LOG_TRACE("xml:"<<buf.get());
	}

	data = xmlptr;
	return true;
}

bool hummer::CXmlActor::PullData(char* buf, size_t& max_buf_len)
{
	HUMMER_NOUSE(buf);
	HUMMER_NOUSE(max_buf_len);
	return false;
}

/*************CTaskActor definition**************/
hummer::CTaskActor::CTaskActor(){}

hummer::CTaskActor::~CTaskActor(){}

int hummer::CTaskActor::DoSubActor(int type, const char* topic, char* buffer, size_t len)
{
	LOG_TRACE("**zk event** task="<<topic<<",type="<<type);
	if (!m_handle || !topic)
	{
		return -1;
	}

	ActType acttype = ACT_INVALID;
	switch(type)
	{
	case NT_TOPIC_CREATE:
		acttype = ACT_ADD;
		break;
	case NT_TOPIC_DEL:
		acttype = ACT_DEL;
		break;
	case NT_TOPIC_UPDATE:
		acttype = ACT_MOD;
		break;
	default:
		LOG_ERROR("Zk type is err,"<<type);
		return -1;
	}

	CTaskCmd* cmd = new CTaskCmd();
	CmdPTR topologyptr(cmd);
	if (!cmd
		|| !cmd->Stuff(acttype, topic, buffer, len))
	{
		LOG_ERROR("create or stuff task failed");
		return -1;
	}

	if (!m_Server
		|| !m_Server->PushCmd(topologyptr))
	{
		LOG_ERROR("push task failed,"<<topic);
		return -1;
	}

	return 0;
}

bool hummer::CTaskActor::PullChilds(CmdPTR& cmd)
{
	if (!m_handle)
	{
		LOG_ERROR("pb in task assigner is null");
		return false;
	}

	CMsgContainer* pcontainer = new(std::nothrow) CMsgContainer();
	CmdPTR msgsptr(pcontainer);
	if (!pcontainer
		|| !(pcontainer->Stuff(CMsgContainer::TEXT_TASK)))
	{
		LOG_ERROR("create container failed in pullChilds");
		return false;
	}	

	set<string> childs;
	if (!(m_handle->PullChild(m_ns.c_str(), m_id, m_name.c_str(), childs)))
	{
		LOG_ERROR("pull childs of task dir failed,id="<<m_id<<",name="<<m_name<<",ns="<<m_ns);
		return false;
	}

	BufPTR buf(new(std::nothrow) char[MAX_ZOO_DATA_LEN]);
	size_t len = MAX_ZOO_DATA_LEN;
	if (!buf.get())
	{
		LOG_ERROR("Malloc buffer failed");
		return false;
	}

	set<string>::iterator iter = childs.begin();
	for (; iter != childs.end(); ++iter)
	{
		len = MAX_ZOO_DATA_LEN;
		memset(buf.get(), 0, len);	
		if (!m_handle->pull(m_ns.c_str(), m_id, (*iter).c_str()
				, buf.get(), len))
		{
			LOG_ERROR("poll "<<(*iter)<<" failed");
			continue;
		}
		
		hummer::task* pTaskData = new(std::nothrow) hummer::task();
		if (!pTaskData)
		{
			LOG_ERROR("Create task pb failed");
			continue;
		}
		
		MsgPTR msgptr(pTaskData);
		if (!pTaskData->ParseFromArray(buf.get(), len))
		{
			LOG_ERROR("task pb parse data failed,"<<(*iter)<<",len="<<len);
			continue;
		}

		LOG_TRACE("task:"<<(*iter));
		pcontainer->Msgs().insert(map<string, MsgPTR>::value_type((*iter), msgptr));	
	}
	LOG_TRACE("Load task pb num = "<<pcontainer->Msgs().size());
	cmd = msgsptr;
	return true;
}

bool hummer::CTaskActor::PullPer(const string& name, CmdPTR& data)
{
	BufPTR buf(new(std::nothrow) char[MAX_ZOO_DATA_LEN]);
	size_t len = MAX_ZOO_DATA_LEN;
	if (!buf.get())
	{
		LOG_ERROR("Malloc buffer failed");
		return false;
	}

	if (!m_handle->pull(m_ns.c_str(), m_id, name.c_str() 
			, buf.get(), len))
	{
		LOG_ERROR("pull task failed");	
		return false;
	}

	hummer::CTaskCmd* pTask = new(std::nothrow) hummer::CTaskCmd();
	CmdPTR cmdptr(pTask);
	if (!cmdptr.get()
		|| !pTask->Stuff(ACT_ADD, m_name.c_str(), buf.get(), len))
	{
		LOG_ERROR("stuff task failed");
		return false;
	}
	
	data = cmdptr;
	return true;
}

bool hummer::CTaskActor::PullData(char* buf, size_t& max_buf_len)
{
	HUMMER_NOUSE(buf);
	HUMMER_NOUSE(max_buf_len);
	return false;
}

bool hummer::CTaskActor::AssignTask(const set<TaskPTR>& tasks)
{
	if (!m_handle
		|| tasks.size() == 0)
	{
		LOG_ERROR("task assigner input err");
		return false;
	}

	set<TaskPTR>::iterator iter = tasks.begin();
	for (; iter != tasks.end(); ++iter)
	{
		MsgPTR taskpb;
		string msgData;
		if (!(*iter)->ConvertToMsg(taskpb)
			|| !taskpb->SerializeToString(&msgData))
		{
			LOG_ERROR("Serialize to string failed");
			return false;
		}

		if (!m_handle->push(m_ns.c_str(), m_id, (*iter)->GetServerDes().c_str(), (char*)(msgData.data()), msgData.size()))
		{
			LOG_ERROR("*ZK*, Task assign failed,"<<(*iter)->GetServerDes());
			return false;
		}
		else
		{
			LOG_TRACE("*ZK*, Task assign,"<<(*iter)->GetServerDes());
		}
	}
	return true;
}

void hummer::CTaskActor::StopTask(const set<TaskPTR>& tasks)
{
	if (!m_handle
		|| tasks.size() == 0)
	{
		LOG_ERROR("task assigner input err");
		return;
	}

	set<TaskPTR>::iterator iter = tasks.begin();
	for (; iter != tasks.end(); ++iter)
	{
		LOG_TRACE("*ZK*, Task del,"<<(*iter)->GetServerDes());
		m_handle->del(m_ns.c_str(), m_id, (*iter)->GetServerDes().c_str());
	}
}

/*************CJobActor definition*************/
hummer::CJobActor::CJobActor(){}

hummer::CJobActor::~CJobActor(){}

bool hummer::CJobActor::PullChilds(CmdPTR& cmd)
{
	if (!m_handle)
	{
		LOG_ERROR("pb in job committer is null");
		return false;
	}

	CMsgContainer* pcontainer = new(std::nothrow) CMsgContainer();
	CmdPTR containerptr(pcontainer);
	if (!pcontainer 
		|| !(pcontainer->Stuff(CMsgContainer::TEXT_COMMIT_JOB)))
	{
		LOG_ERROR("create container failed in pullChilds");
		return false;
	}

	set<string> childs;
	if (!(m_handle->PullChild(m_ns.c_str(), m_id, m_name.c_str(), childs)))
	{
		LOG_ERROR("pull childs of job dir failed,ns="<<m_ns<<",id="<<m_id<<",name="<<m_name);
		return false;
	}

	BufPTR buf(new(std::nothrow) char[MAX_ZOO_DATA_LEN]);
	size_t len = MAX_ZOO_DATA_LEN;
	if (!buf.get())
	{
		LOG_ERROR("Malloc buffer failed");
		return false;
	}

	set<string>::iterator iter = childs.begin();
	for (; iter != childs.end(); ++iter)
	{
		len = MAX_ZOO_DATA_LEN;
		memset(buf.get(), 0, len);	
		if (!m_handle->pull(m_ns.c_str(), m_id, (*iter).c_str()
				, buf.get(), len))
		{
			LOG_ERROR("poll "<<(*iter)<<" failed");
			continue;
		}
		
		MsgPTR msgptr(new(std::nothrow) hummer::job());
		if (!msgptr.get()
			|| !(msgptr->ParseFromArray(buf.get(), len)))
		{
			LOG_ERROR("job pb parse data failed");
			continue;
		}

		LOG_TRACE("insert:"<<(*iter));
		pcontainer->Msgs().insert(map<string, MsgPTR>::value_type((*iter), msgptr));	
	}
	LOG_TRACE("Load job pb num = "<<pcontainer->Msgs().size());
	cmd = containerptr;
	return true;
}

bool hummer::CJobActor::PullPer(const string& name, CmdPTR& data)
{
	HUMMER_NOUSE(name);
	HUMMER_NOUSE(data);
	return false;
}

bool hummer::CJobActor::PullData(char* buf, size_t& max_buf_len)
{
	HUMMER_NOUSE(buf);
	HUMMER_NOUSE(max_buf_len);
	return false;
}

bool hummer::CJobActor::CommitJob(const JobPTR& job)
{
	MsgPTR jobmsg;
	string msgData;
	CJob* pjob = dynamic_cast<CJob*>(job.get());
	if (!m_handle || !pjob)
	{
		LOG_ERROR("handle or job is null");
		return false;
	}

	if (!pjob->ConvertToMsg(jobmsg)
	 	|| !jobmsg->SerializeToString(&msgData))
	{
		LOG_ERROR("Serialize to string failed");
		return false;
	}
	//just test
	{
		MsgPTR msgptr(new(std::nothrow) hummer::job());
		if (!msgptr.get()
			|| !(msgptr->ParseFromArray(msgData.data(), msgData.size())))
		{
			LOG_ERROR("job pb parse data failed while commit");
		}
	}

	if (!m_handle->push(m_ns.c_str(), m_id, pjob->GetDes().c_str(), (char*)(msgData.data()), msgData.size()))
	{
		LOG_ERROR("*ZK*, job Commit failed,"<<pjob->GetDes());
		return false;
	}
	LOG_TRACE("*ZK*, job Commit,"<<pjob->GetDes());
	return true;
}

void hummer::CJobActor::RollbackJob(const JobPTR& job)
{
	if (!m_handle
		|| !job.get())
	{
		LOG_ERROR("job committer input err");
		return;
	}

	m_handle->del(m_ns.c_str(), m_id, job->GetDes().c_str());
	LOG_TRACE("*ZK*, job UnCommit,"<<job->GetDes());
}

int hummer::CJobActor::DoSubActor(int type, const char* topic, char* buffer, size_t len)
{
	HUMMER_NOUSE(type);
	HUMMER_NOUSE(topic);
	HUMMER_NOUSE(buffer);
	HUMMER_NOUSE(len);
	HUMMER_ASSERT(false);
	return 0;
}

/***********CIdGenerator definition************/
static uint64_t testcounter = 0;

hummer::CIdGenerator::CIdGenerator()
: m_TestFlag(false), m_pb(NULL){}

hummer::CIdGenerator::~CIdGenerator(){}

void hummer::CIdGenerator::SetInfo(CZkServer* pb, const char* ns
		, const char* jobCounter, const char* taskCounter)
{
	m_pb = pb;
	m_ns = ns;
	m_JobIdCounter = jobCounter;
	m_TaskIdCounter = taskCounter;
}

void hummer::CIdGenerator::CheckCounter()
{
	if (m_pb)
	{
		int64_t initvalue = 0;
		if (!m_pb->CreateCounter(m_JobIdCounter.c_str(), initvalue))
		{
			LOG_ERROR("create job counter err,"<<m_JobIdCounter);
		}

		if (!m_pb->CreateCounter(m_TaskIdCounter.c_str(), initvalue))
		{
			LOG_ERROR("create task counter err,"<<m_TaskIdCounter);
		}
	}
	return;
}

bool hummer::CIdGenerator::JobId(uint64_t& jobId)
{
	if (m_pb)
	{
		int64_t jobid = 0;
		if(m_pb->NextCounter(m_JobIdCounter.c_str(), jobid))
		{
			jobId = (int64_t)jobid;
			return true;
		}
	}
	return false;
}

bool hummer::CIdGenerator::TaskId(uint64_t& taskId)
{
	if (m_pb)
	{
		int64_t taskid = 0;
		if(m_pb->NextCounter(m_TaskIdCounter.c_str(), taskid)
			&& taskid > 0)
		{
			taskId = (uint64_t)taskid;
			return true;
		}
		return false;
	}
	else if (m_TestFlag)
	{
		testcounter++;
		taskId = testcounter;
		return true;
	}
	else
	{
		return false;
	}
}

void hummer::CIdGenerator::JustTest()
{
	m_TestFlag = true;
}


