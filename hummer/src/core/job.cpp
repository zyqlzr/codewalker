#include "job.hpp"
#include "utildef.hpp"
#include "Logger.h"
#include "xmlwrapper.hpp"
#include "job.pb.h"

#define JOB_ROOT "job"
#define JOB_TYPE "type"
#define JOB_BUSI "business"
#define JOB_CORE "core"
#define JOB_SRC "src"
#define JOB_DST "dst"
#define JOB_SD_DES "des"
#define JOB_SPLIT "split"
#define JOB_SPLIT_DEF "def"
#define JOB_SPLIT_TYPE	"TYPE"
#define JOB_SPLIT_ID "ID"
#define JOB_SPLIT_DATA "DATA"

#define TEST_BUSI "test"
#define SKETCH_BUSI "sketch"

static string simJob = JOB_SIM_STRING;
static string mapJob = JOB_MAP_STRING;

static string defSplit = SPLIT_DEF_STRING;
static string preSplit = SPLIT_PRE_STRING;
static string mapSplit = SPLIT_MAP_STRING;
 
static BusinessType FetchBusiness(const string& value)
{
	if (value == TEST_BUSI)
	{
		return BUSI_TEST;
	}
	else
	{
		return BUSI_SKETCH;
	}
}

static JobType FetchJobType(const string& value)
{
	if (value == simJob)
	{
		return JOB_SIM;
	}
	else if (value == mapJob)
	{
		return JOB_MAP;
	}
	else
	{
		return JOB_SIM;
	}
}

static hummer::CSplit::SplitType FetchSplitType(const string& value)
{
	if (value == preSplit)
	{
		return hummer::CSplit::SPLIT_PRE;
	}
	else if (value == mapSplit)
	{
		return hummer::CSplit::SPLIT_MAP;
	}
	else
	{
		return hummer::CSplit::SPLIT_DEF;
	}
}

hummer::CJob::CJob()
: m_JobId(0), m_JobType(JOB_INVALID), m_Busi(BUSI_INVALID)
, m_status(JOB_STATUS_INVALID), m_ExpectCore(0){}

hummer::CJob::~CJob(){}

bool hummer::CJob::LoadXml(const char* des, const char* xml, int len)
{
	CXmlWrapper wrapper;
	string jobroot = JOB_ROOT;
	if (!xml || 0 == len
		|| !wrapper.LoadMem(xml, len)
		|| !wrapper.CheckRoot(jobroot))
	{
		return false;
	}

	xmlNodePtr rootNode = wrapper.GetRootNode();
	if (!rootNode)
	{
		return false;
	}

	multimap<string, xmlNodePtr> jobParms;
	if (!wrapper.GetChilds(rootNode, jobParms)
		|| jobParms.size() == 0)
	{
		return false;
	}
	m_JobDes = des;
	LOG_TRACE("Load JOB,des="<<des);
	multimap<string, xmlNodePtr>::iterator NodeIter = jobParms.begin();
	for (; NodeIter != jobParms.end(); ++NodeIter)
	{
		string value;
		if (JOB_CORE == NodeIter->first)
		{
			if (wrapper.GetText(NodeIter->second, value))
			{
				m_ExpectCore = atoi(value.c_str());
				LOG_TRACE("Load JOB,cpu="<<m_ExpectCore);
			}
		}
		else if (JOB_BUSI == NodeIter->first)
		{
			if (wrapper.GetText(NodeIter->second, value))
			{
				m_Busi = FetchBusiness(value);
				LOG_TRACE("Load JOB,busi="<<m_Busi<<","<<value);
			}
		}
		else if (JOB_TYPE == NodeIter->first)
		{
			if (wrapper.GetText(NodeIter->second, value))
			{
				m_JobType = FetchJobType(value);
			}
			LOG_TRACE("Load JOB,type="<<value<<","<<m_JobType);
		}
		else if (JOB_SRC == NodeIter->first
				|| JOB_DST == NodeIter->first)
		{
			multimap<string, xmlNodePtr> subParms;
			if (!wrapper.GetChilds(NodeIter->second, subParms)
				|| subParms.size() == 0)
			{
				continue;
			}

			multimap<string, xmlNodePtr>::iterator subIter = subParms.begin();
			for (; subIter != subParms.end(); ++subIter)
			{
				string value;
				if (!wrapper.GetText(subIter->second, value)
					|| 0 == value.length())
				{
					continue;
				}
	
				if (JOB_SRC == NodeIter->first){
					LOG_TRACE("Load JOB, SRC="<<value);
					m_SrcDes.insert(value);
				}
				else{
					LOG_TRACE("Load JOB, DST="<<value);
					m_DstDes.insert(value);
				}
			}
		}
		else if (JOB_SPLIT == NodeIter->first)
		{
			string TypeAttr = JOB_SPLIT_TYPE;
			string IdAttr = JOB_SPLIT_ID;
			string DataAttr = JOB_SPLIT_DATA;
			multimap<string, xmlNodePtr> subParms;
			if (!wrapper.GetChilds(NodeIter->second, subParms)
				|| subParms.size() == 0)
			{
				continue;
			}

			multimap<string, xmlNodePtr>::iterator subIter = subParms.begin();
			for (; subIter != subParms.end(); ++subIter)
			{
				string typeValue;
				string idValue;
				string dataValue;
				wrapper.GetAttr(subIter->second, TypeAttr, typeValue);
				if (wrapper.GetAttr(subIter->second, IdAttr, idValue)
					&& wrapper.GetAttr(subIter->second, DataAttr, dataValue))
				{
					SplitPTR splitptr(new(std::nothrow) CSplit());
					splitptr->Init(FetchSplitType(typeValue), idValue
						, dataValue);
					m_Splits.insert(SPLITS::value_type(idValue, splitptr));
				}
				LOG_TRACE("Load JOB, type="<<typeValue<<",id="<<idValue<<",data="<<dataValue);
			}
		}
		else
		{
			//do nothing
		}
	}
	m_status = JOB_STATUS_UNINIT;
	return true;
}

void hummer::CJob::Assign(JID jid)
{
	m_JobId = jid;
	m_status = JOB_STATUS_SUBMIT;
	SPLITS::iterator sIter = m_Splits.begin();
	for (; sIter != m_Splits.end(); ++sIter)
	{
		m_RedoSplits.insert(sIter->first);
	}
}

bool hummer::CJob::ConvertByMsg(MsgPTR& jobmsg)
{
	hummer::job* jmsg = dynamic_cast<hummer::job*>(jobmsg.get());
	if(!jmsg)
	{
		return false;
	}

	m_JobId = jmsg->jobid();
	m_JobDes = jmsg->jobdes();
	m_JobType = (JobType)jmsg->jobtype();
	m_status = (CJob::JobStatus)(jmsg->jobstatus());
	m_Busi = (BusinessType)jmsg->business();

	int srcnum = jmsg->srcdes_size();
	for (int i = 0; i < srcnum; ++i)
	{
		m_SrcDes.insert(jmsg->srcdes(i));	
	}
	HUMMER_ASSERT((size_t)srcnum == m_SrcDes.size());

	int dstnum = jmsg->dstdes_size();
	for (int j = 0; j < dstnum; ++j)
	{
		m_DstDes.insert(jmsg->dstdes(j));
	}
	HUMMER_ASSERT((size_t)dstnum == m_DstDes.size());

	int splitnum = jmsg->splits_size();
	for (int m = 0; m < splitnum; ++m)
	{
		SplitPTR splitptr(new(std::nothrow) hummer::CSplit());
		splitptr->Init((CSplit::SplitType)(jmsg->splits(m).type())
			, jmsg->splits(m).sid(), jmsg->splits(m).svalue());
		m_Splits.insert(SPLITS::value_type(splitptr->GetId(), splitptr));
	}
	HUMMER_ASSERT((size_t)splitnum == m_Splits.size());

	int assignnum = jmsg->assigns_size();
	for (int n = 0; n < assignnum; ++n)
	{
		CAssignment assign;
		assign.Assign(jmsg->assigns(n).tid(), jmsg->assigns(n).split());
		if (jmsg->assigns(n).finiflag())
		{
			assign.Finish();
		}
		m_Assignments.insert(ASSIGNMENTS::value_type(assign.GetTid(), assign));
	}
	HUMMER_ASSERT((size_t)assignnum == m_Assignments.size());
	return true;
}

void hummer::CJob::Recover()
{
	HUMMER_ASSERT(m_Assignments.size() <= m_Splits.size());
	set<string> keysets;
	SPLITS::iterator splitIter = m_Splits.begin();
	for (; splitIter != m_Splits.end(); ++splitIter)
	{
		keysets.insert(splitIter->first);
	}

	ASSIGNMENTS::iterator assignIter = m_Assignments.begin();
	for (; assignIter != m_Assignments.end(); ++assignIter)
	{
		string split = assignIter->second.Split();
		if (!assignIter->second.IsFini())
		{
			m_RunningSplits.insert(split);
		}
		keysets.erase(split);
	}
	
	m_RedoSplits = keysets;
	LOG_TRACE("Job recover: split="<<m_Splits.size()<<",redo="<<m_RedoSplits.size()<<",running="<<m_RunningSplits.size());
	return;
}

bool hummer::CJob::ConvertToMsg(MsgPTR& jobmsg)
{
	hummer::job* pjob = new(std::nothrow) hummer::job();
	MsgPTR newJobMsg(pjob);
	if (!newJobMsg.get())
	{
		return false;
	}
	pjob->set_jobid(m_JobId);
	pjob->set_jobtype(m_JobType);
	pjob->set_jobdes(m_JobDes);
	pjob->set_jobstatus(m_status);
	pjob->set_business(m_Busi);
	pjob->set_cpucore(m_ExpectCore);
	set<string>::iterator srcIter = m_SrcDes.begin();
	for (; srcIter != m_SrcDes.end(); ++srcIter)
	{
		pjob->add_srcdes(*srcIter);
	}
	set<string>::iterator dstIter = m_DstDes.begin();
	for (; dstIter != m_DstDes.end(); ++dstIter)
	{
		pjob->add_dstdes(*dstIter);
	}

	SPLITS::iterator splitIter = m_Splits.begin();
	for (; splitIter != m_Splits.end(); ++splitIter)
	{
		hummer::split* psplit = pjob->add_splits();
		psplit->set_sid(splitIter->second->GetId());
		psplit->set_svalue(splitIter->second->GetText());
		psplit->set_type(splitIter->second->GetType());
	}

	ASSIGNMENTS::iterator assignIter = m_Assignments.begin();
	for (; assignIter != m_Assignments.end(); ++assignIter)
	{
		hummer::assignment* passign = pjob->add_assigns();
		passign->set_tid(assignIter->second.GetTid());
		passign->set_split(assignIter->second.Split());
		passign->set_finiflag(assignIter->second.IsFini());
	}

	jobmsg = newJobMsg;
	return true;
}

void hummer::CJob::Submit(JID jid, JobType jt
				, BusinessType busi, const string& jobDes
				, set<SplitPTR>& splits, set<string>& srcs
				, set<string>& dsts, int corenum)
{
	m_JobId = jid;
	m_JobType = jt;
	m_Busi = busi;
	m_JobDes = jobDes;

	m_SrcDes = srcs;
	m_DstDes = dsts;
	set<SplitPTR>::iterator sIter = splits.begin();
	for (; sIter != splits.end(); ++sIter)
	{
		m_Splits.insert(SPLITS::value_type((*sIter)->GetId(), *sIter));
	}
	m_ExpectCore = corenum;
	m_status = JOB_STATUS_SUBMIT;	
	return;
}

void hummer::CJob::GetAllTaskID(set<TID>& ids)const
{
	map<TID, CAssignment>::const_iterator iter = m_Assignments.begin();
	for (; iter != m_Assignments.end(); ++iter)
	{
		ids.insert(iter->first);
	}
}

void hummer::CJob::GetRunningTaskID(set<TID>& ids)const
{
	map<TID, CAssignment>::const_iterator iter = m_Assignments.begin();
	for (; iter != m_Assignments.end(); ++iter)
	{
		if (!iter->second.IsFini())
		{
			ids.insert(iter->first);
		}
	}
}

JID hummer::CJob::GetJobId()const
{
	return m_JobId;
}

const set<string>& hummer::CJob::GetSrc()const
{
	return m_SrcDes;
}

const set<string>& hummer::CJob::GetDst()const
{
	return m_DstDes;
}

BusinessType hummer::CJob::GetBusi()const
{
	return m_Busi;
}

hummer::CJob::JobStatus hummer::CJob::GetStatus()const
{
	return m_status;
}

const string& hummer::CJob::GetDes()const
{
	return m_JobDes;
}

int hummer::CJob::CoreNum()const
{
	return m_ExpectCore;
}

JobType hummer::CJob::GetJobType()const
{
	return m_JobType;
}

void hummer::CJob::Commit(set<TaskPTR>& tasks)
{
	HUMMER_ASSERT(JOB_STATUS_RUNNING != m_status);
	HUMMER_ASSERT(JOB_STATUS_FINI != m_status);
	set<TaskPTR>::iterator tIter = tasks.begin();
	for (; tIter != tasks.end(); ++tIter)
	{
		TaskRun(*tIter);	
	}
}

void hummer::CJob::Rollback(set<TaskPTR>& tasks)
{
	HUMMER_NOUSE(tasks);
	//do nothing
	if (m_RedoSplits.size() > 0
		&& m_status != CJob::JOB_STATUS_ERR
		&& m_status != CJob::JOB_STATUS_PART)
	{
		m_status = CJob::JOB_STATUS_PART;
	}

	HUMMER_ASSERT(m_Assignments.size() + m_RedoSplits.size() == m_Splits.size());
}

void hummer::CJob::AllSplit(list<SplitPTR>& all)
{
	SPLITS::iterator sIter = m_Splits.begin();
	for (; sIter != m_Splits.end(); ++sIter)
	{
		all.push_back(sIter->second);
	}
}

void hummer::CJob::UnAssignSplit(list<SplitPTR>& unassign)
{
	set<string>::iterator redoIter = m_RedoSplits.begin();
	for (; redoIter != m_RedoSplits.end(); ++redoIter)
	{
		SPLITS::iterator sIter = m_Splits.find(*redoIter);
		HUMMER_ASSERT(sIter != m_Splits.end());
		unassign.push_back(sIter->second);
	}
}

void hummer::CJob::TaskErr(TID tid)
{
	ASSIGNMENTS::iterator afind = m_Assignments.find(tid);
	if (afind == m_Assignments.end())
	{
		HUMMER_ASSERT(false);
		return;
	}

	HUMMER_ASSERT(!afind->second.IsFini());
	string split = afind->second.Split();
	m_RunningSplits.erase(split);
	m_RedoSplits.insert(split);
	m_Assignments.erase(afind);

	if (m_status != JOB_STATUS_ERR)
	{
		m_status = JOB_STATUS_ERR;
	}
	HUMMER_ASSERT(m_Assignments.size() + m_RedoSplits.size() == m_Splits.size());
	return;
}

void hummer::CJob::TaskFini(TID tid, TaskhbPTR& hbInfo)
{
	HUMMER_NOUSE(hbInfo);
	ASSIGNMENTS::iterator afind = m_Assignments.find(tid);
	if (afind == m_Assignments.end())
	{
		HUMMER_ASSERT(false);
		return;
	}

	string split = afind->second.Split();
	afind->second.Finish();
	m_RunningSplits.erase(split);
	DoSplitPre(split, hbInfo);
	if (CheckFini())
	{
		m_status = JOB_STATUS_FINI;
	}

	if (m_RedoSplits.size() > 0)
	{
		m_status = JOB_STATUS_PART;
	}
	else
	{
		m_status = JOB_STATUS_RUNNING;
	}
	HUMMER_ASSERT(m_Assignments.size() + m_RedoSplits.size() == m_Splits.size());
	return;	
}

void hummer::CJob::DoSplitPre(const string& split, TaskhbPTR& hbInfo)
{
	LOG_TRACE("Job do split pre,split="<<split);
	hbInfo->Print();
	list<SplitPTR>& splits = hbInfo->GetSplitResult();
	
	if (!hbInfo.get() 
		|| 0 == splits.size())
	{
		return;
	}

	SPLITS::iterator fIter = m_Splits.find(split);
	if (fIter == m_Splits.end())
	{
		return;
	}

	if (fIter->second->GetType() == CSplit::SPLIT_PRE)
	{
		list<SplitPTR>::iterator sIter = splits.begin();
		for (; sIter != splits.end(); ++sIter)
		{
			LOG_TRACE("Split:id="<<(*sIter)->GetId()<<",type="<<(*sIter)->GetType()<<",text="<<(*sIter)->GetText());
			m_Splits.insert(SPLITS::value_type((*sIter)->GetId()
				, (*sIter)));
			m_RedoSplits.insert((*sIter)->GetId());	
		} 
	}
}

void hummer::CJob::TaskRun(const TaskPTR& task)
{
	HUMMER_ASSERT(task.get() && task->GetSplit().get());
	string splitId = task->GetSplit()->GetId();
	TID tid = task->GetTaskId();

	SPLITS::iterator sfind = m_Splits.find(splitId);
	if (m_Splits.end() == sfind)
	{
		HUMMER_ASSERT(false);
		return;
	}

	ASSIGNMENTS::iterator afind = m_Assignments.find(tid);
	if (m_Assignments.end() != afind)
	{
		HUMMER_ASSERT(false);
		return;
	}

	hummer::CAssignment assign;
	assign.Assign(tid, splitId);
	m_Assignments.insert(map<TID, hummer::CAssignment>::value_type(tid, assign));
	m_RedoSplits.erase(splitId);
	m_RunningSplits.insert(splitId);

	if (m_RedoSplits.size() > 0)
	{
		m_status = JOB_STATUS_PART;
	}
	else
	{
		m_status = JOB_STATUS_RUNNING;
	}
	HUMMER_ASSERT(m_Assignments.size() + m_RedoSplits.size() == m_Splits.size());
}

bool hummer::CJob::CheckFini()
{
	if (m_RedoSplits.size() != 0 
		|| m_RunningSplits.size() != 0
		|| m_Assignments.size() != m_Splits.size())
	{
		HUMMER_ASSERT(m_Assignments.size() + m_RedoSplits.size() == m_Splits.size());
		return false;
	}

	ASSIGNMENTS::iterator aIter = m_Assignments.begin();
	for (; aIter != m_Assignments.end(); ++aIter)
	{
		HUMMER_ASSERT(aIter->second.IsFini());
	}
	return true;
}

void hummer::CJob::Print()
{
	LOG_TRACE("JOB id:"<<m_JobId<<",S:"<<m_status<<",T:"<<m_JobType<<",B:"<<m_Busi<<",Split:"<<m_Splits.size());
	LOG_TRACE("JOB Statistic Assign:"<<m_Assignments.size()<<",Running:"<<m_RunningSplits.size()<<",Redo="<<m_RedoSplits.size());
}

void hummer::CJob::Statistic()
{
	char buf[256] = {0};
	sprintf(buf, "JOB id:%lu,Total:%u,Running:%u,Wait:%u,Fini:%u"
		, m_JobId, m_Splits.size()
		, m_RunningSplits.size()
		, m_RedoSplits.size()
		, m_Splits.size() - m_RedoSplits.size() - m_RunningSplits.size());
	LOG_TRACE(buf);
}


