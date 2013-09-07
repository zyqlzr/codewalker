#include "hummerctx.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Logger.h"

hummer::CParmSet::CParmSet(){}

hummer::CParmSet::~CParmSet(){}

bool hummer::CParmSet::SetParm(const string& name, string& value)
{
	map<string, string>::iterator findIter = m_ParmSet.find(name);
	if (findIter != m_ParmSet.end())
	{
		findIter->second = value;
	}
	else
	{
		m_ParmSet.insert(map<string, string>::value_type(name, value));
	}
	return true;
}

bool hummer::CParmSet::GetParm(const string& name, string& value)
{
	map<string, string>::iterator findIter = m_ParmSet.find(name);
	if (findIter != m_ParmSet.end())
	{
		value = findIter->second;
		return true;	
	}
	LOG_TRACE("GetSet err:"<<name);	
	return false;	
}

bool hummer::CParmSet::GetParm(const string& name, int& value)
{
	string valueString;
	if (!GetParm(name, valueString))
	{
		return false;
	}
	
	value = atoi(valueString.c_str());
	return true;
}

bool hummer::CParmSet::GetParm(const string& name, bool& value)
{
	(void)name;
	(void)value;
	return true;
}

size_t hummer::CParmSet::size()const
{
	return m_ParmSet.size();
}

void hummer::CParmSet::Copy(map<string, string>& parms)
{
	parms = m_ParmSet;
}

/****************CParmObj definition****************/
hummer::CParmObj::CParmObj(){}

hummer::CParmObj::~CParmObj(){}

bool hummer::CParmObj::SetServerParm(const string& server, const string& name, string& value)
{
	SERVER_SETS::iterator ServerIter = m_servers.find(server);
	if (ServerIter != m_servers.end())
	{
		return ServerIter->second.SetParm(name, value);			
	}
	else
	{
		pair<SERVER_SETS::iterator, bool> iret = m_servers.insert(SERVER_SETS::value_type(server, CParmSet()));
		if (!iret.second)
		{
			return false;
		}
		
		return iret.first->second.SetParm(name, value);
	}
}

bool hummer::CParmObj::GetServerParm(const string& server, const string& name, string& value)
{
	SERVER_SETS::iterator ServerIter = m_servers.find(server);
	if (ServerIter == m_servers.end())
	{
		LOG_TRACE("GetParm err,"<<server<<","<<name);
		return false;
	}

	return ServerIter->second.GetParm(name, value);				
}

bool hummer::CParmObj::CheckServer(const string& des)
{
	SERVER_SETS::iterator iter = m_servers.find(des);
	if (iter == m_servers.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool hummer::CParmObj::GetServer(const string& des
		, map<string, string>& parms)
{
	SERVER_SETS::iterator iter = m_servers.find(des);
	if (iter == m_servers.end())
	{
		return false;
	}
	else
	{
		iter->second.Copy(parms);
		return true;
	}
}

/***************CHummerCtx definition****************/
hummer::CHummerCtx::CHummerCtx(){}

hummer::CHummerCtx::~CHummerCtx(){}

bool hummer::CHummerCtx::LoadLocalConf(const char* localConfig)
{
	CXmlWrapper wrapper;
	string localFile = localConfig;
	string localroot = LOCAL_ROOT;
	if (NULL == localConfig 
		|| !wrapper.LoadFile(localFile))
	{
		return false;
	}

	multimap<string, xmlNodePtr> localParms;
	xmlNodePtr rootNode = NULL;
	if (!wrapper.CheckRoot(localroot))
	{
		return false;
	}

	rootNode = wrapper.GetRootNode();
	if (NULL == rootNode)
	{
		return false;
	}
	
	if (!wrapper.GetChilds(rootNode, localParms))
	{
		return false;
	}

	if (localParms.size() == 0)
	{
		return false;
	}
	
	multimap<string, xmlNodePtr>::iterator NodeIter = localParms.begin();
	for (; NodeIter != localParms.end(); ++NodeIter)
	{
		string value;
		if (NodeIter->first == LOCAL_DES 
			|| NodeIter->first == LOCAL_ZK)
		{
			if (!wrapper.GetText(NodeIter->second, value))
			{
				continue;
			}
			m_local.SetParm(NodeIter->first, value);
		}	
	}
		
	return true;	
}

bool hummer::CHummerCtx::LoadMemGlobalConf(const char* buf, int len)
{
	CXmlWrapper wrapper;
	string globalRoot = GLOBAL_ROOT;
	if (NULL == buf || 0 == len
		|| !wrapper.LoadMem(buf, len))
	{
		return false;
	}

	multimap<string, xmlNodePtr> globalParms;
	xmlNodePtr rootNode = NULL;
	if (!wrapper.CheckRoot(globalRoot))
	{
		return false;
	}

	rootNode = wrapper.GetRootNode();
	if (NULL == rootNode)
	{
		return false;
	}
	
	if (!wrapper.GetChilds(rootNode, globalParms))
	{
		return false;
	}

	if (globalParms.size() == 0)
	{
		return false;
	}

	multimap<string, xmlNodePtr>::iterator NodeIter = globalParms.begin();
	for (; NodeIter != globalParms.end(); ++NodeIter)
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
			m_global.SetServerParm(NodeIter->first, subIter->first, value);
		}
	}
	return true;
}


bool hummer::CHummerCtx::LoadMemWorkerConf(const char* buf, int len)
{
	CXmlWrapper wrapper;
	string workerRoot = WORKER_ROOT;
	if (NULL == buf || 0 == len
		|| !wrapper.LoadMem(buf, len))
	{
		return false;
	}

	multimap<string, xmlNodePtr> workerParms;
	xmlNodePtr rootNode = NULL;
	if (!wrapper.CheckRoot(workerRoot))
	{
		return false;
	}

	rootNode = wrapper.GetRootNode();
	if (NULL == rootNode)
	{
		return false;
	}
	
	if (!wrapper.GetChilds(rootNode, workerParms))
	{
		return false;
	}

	if (workerParms.size() == 0)
	{
		return false;
	}

	multimap<string, xmlNodePtr>::iterator NodeIter = workerParms.begin();
	for (; NodeIter != workerParms.end(); ++NodeIter)
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
			m_workers.SetServerParm(NodeIter->first, subIter->first, value);
		}
	}

	return true;
}

bool hummer::CHummerCtx::GetGlobalParm(const string& des
			, const string& attr, string& value)
{
	return m_global.GetServerParm(des, attr, value);	
}

bool hummer::CHummerCtx::GetGlobalParm(const string& des
			, const string& attr, int& value)
{
	string serverValue;
	if (!m_global.GetServerParm(des, attr, serverValue))
	{
		return false;
	}
	else
	{
		value = atoi(serverValue.c_str());
		return true;
	}
}

bool hummer::CHummerCtx::GetGlobalParm(const string& des
			, const string& attr, bool& value)
{
	string serverValue;
	if (!m_global.GetServerParm(des, attr, serverValue))
	{
		return false;
	}
	else
	{
		if (serverValue == "false")
		{
			value = false;
		}
		else if (serverValue == "true")
		{
			value = true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool hummer::CHummerCtx::GlobalHasDes(const string& des)
{
	return m_global.CheckServer(des);
}

bool hummer::CHummerCtx::GlobalServer(const string& des
		, map<string, string>& parms)
{
	return m_global.GetServer(des, parms);	
}

bool hummer::CHummerCtx::GetWorkerParm(const string& des
			, const string& attr, string& value)
{
	return m_workers.GetServerParm(des, attr, value);	
}

bool hummer::CHummerCtx::GetWorkerParm(const string& des
			, const string& attr, int& value)
{
	string serverValue;
	if (!m_workers.GetServerParm(des, attr, serverValue))
	{
		return false;
	}
	else
	{
		value = atoi(serverValue.c_str());
		return true;
	}
}

bool hummer::CHummerCtx::GetWorkerParm(const string& des
			, const string& attr, bool& value)
{
	string serverValue;
	if (!m_workers.GetServerParm(des, attr, serverValue))
	{
		return false;
	}
	else
	{
		if (serverValue == "false")
		{
			value = false;
		}
		else if (serverValue == "true")
		{
			value = true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool hummer::CHummerCtx::WorkerHasDes(const string& des)
{
	return m_workers.CheckServer(des);
}

bool hummer::CHummerCtx::GetZKAddr(string& zk)
{
	return m_local.GetParm(LOCAL_ZK, zk);
}

bool hummer::CHummerCtx::GetServerDes(string& des)
{
	return m_local.GetParm(LOCAL_DES, des);
}

const char* hummer::CHummerCtx::GetNs()const
{
	return HUMMER_NS_STRING;	
}

TopicId hummer::CHummerCtx::GetWorkerTId()const
{
	return TOPIC_WORKER;
}

TopicId hummer::CHummerCtx::GetTaskTId()const
{
	return TOPIC_TASK;
}

TopicId hummer::CHummerCtx::GetTaskHeartbeatTId()const
{
	return TOPIC_TASK_HEARTBEAT;
}

TopicId hummer::CHummerCtx::GetConfTId()const
{
	return TOPIC_CONF;
}

TopicId hummer::CHummerCtx::GetSubmitJobTId()const
{
	return TOPIC_SUBMIT_JOB;
}

TopicId hummer::CHummerCtx::GetCommitJobTId()const
{
	return TOPIC_COMMIT_JOB;
}

const char* hummer::CHummerCtx::GetDirSubMod()const
{
	return "*";
}

const char* hummer::CHummerCtx::GetGlobalConf()const
{
	return GLOBAL_CONF_XML;
}

hummer::CParmSet& hummer::CHummerCtx::GetLocalParms()
{
	return m_local;
}

const char* hummer::CHummerCtx::GetTaskHBName(uint64_t taskId)const
{
	static char taskstring[256];
	memset(taskstring, 0, 256);
	sprintf(taskstring, "Task-%ld", taskId);
	return taskstring;
}

bool hummer::CHummerCtx::GetTaskIDFromHBName(const string& hb
		, uint64_t& id)
{
	if (0 == hb.length())
	{
		return false;
	}

	string::size_type pos = hb.find("-");
	if (string::npos == pos)
	{
		return false;
	}

	id = atoi(hb.substr(pos + 1).c_str());
	return true;
}

const char* hummer::CHummerCtx::LOCAL_ROOT = "local";
const char* hummer::CHummerCtx::LOCAL_DES = "des";
const char* hummer::CHummerCtx::LOCAL_ZK = "zk";
const char* hummer::CHummerCtx::GLOBAL_ROOT = "mcs";
const char* hummer::CHummerCtx::GLOBAL_CONF_XML = "global_conf.xml";

const char* hummer::CHummerCtx::MYSQL_TYPE = "mysql";
const char* hummer::CHummerCtx::AMQ_TYPE = "amq";
const char* hummer::CHummerCtx::HBASE_TYPE = "hbase";
const char* hummer::CHummerCtx::LOCALPATH_TYPE = "localpath";

const char* hummer::CHummerCtx::SRC_DST_Type = "type";
const char* hummer::CHummerCtx::DB_NAME = "dbname";
const char* hummer::CHummerCtx::DB_HOST = "host";
const char* hummer::CHummerCtx::DB_USR = "user";
const char* hummer::CHummerCtx::DB_PW = "pw";
const char* hummer::CHummerCtx::DB_PORT = "port";
const char* hummer::CHummerCtx::DB_TABLE = "table";
const char* hummer::CHummerCtx::DB_UNIXSOCK = "unixsock";

const char* hummer::CHummerCtx::AMQ_SRCURI = "BrokerURI";
const char* hummer::CHummerCtx::AMQ_DSTURI = "DestURI";
const char* hummer::CHummerCtx::AMQ_TQFLAG = "TQFlag";
const char* hummer::CHummerCtx::AMQ_ACK = "ACK";

const char* hummer::CHummerCtx::WORKER_CONF = "worker.xml";
const char* hummer::CHummerCtx::WORKER_ROOT = "worker";
const char* hummer::CHummerCtx::WORKER_CPU = "cpu_core";
const char* hummer::CHummerCtx::WORKER_PORTUPPER = "portupper";
const char* hummer::CHummerCtx::WORKER_PORTDOWN = "portdown";
const char* hummer::CHummerCtx::WORKER_FDFSCONF = "fdfsconf";

const char* hummer::CHummerCtx::LOCAL_PATH = "LocalPath";
const char* hummer::CHummerCtx::ABSOLUTE_PATH = "AbsolutePath";
const char* hummer::CHummerCtx::RELATIVE_PATH = "RelativePath";

