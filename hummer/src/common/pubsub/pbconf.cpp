/*****************************************************
Copyright 2010-20XX ZY 
FileName: clusterzoo.cpp
Description: the wrapper of zookeeper api 
Author: zhengyang
Date: 2013.06.03
*****************************************************/

#include "pbconf.hpp"
#include "nsrule.hpp"
#include "pbdef.hpp"

#include "Logger.h"

#include <string.h>

const char* HUMMER_NS_STRING = "hummer";
const char* COUNTER_NS_STRING = "counter";
const char* TEST_NS_STRING = "test";
const char* CONFIG_NS_STRING = "config";

const char* CONFIG_NS_TOPIC = "/config";

const char* HUMMER_NS_TOPIC = "/hummer";
const char* HUMMER_CONFIG_TOPIC = "/hummer/config";
const char* HUMMER_WORKER_TOPIC = "/hummer/worker";
const char* HUMMER_TASK_TOPIC = "/hummer/task";
const char* HUMMER_TASK_HEARTBEAT_TOPIC = "/hummer/task_heartbeat";
const char* HUMMER_SUBMIT_JOB_TOPIC = "/hummer/submit_job";
const char* HUMMER_COMMIT_JOB_TOPIC = "/hummer/commit_job";
const char* HUMMER_MASTER_TOPIC = "/hummer/master";

const char* COUNTER_NS_TOPIC = "/counter";
const char* COUNTER_LOCK_TOPIC = "/counter/lock";
const char* COUNTER_VALUE_TOPIC = "/counter/value";
 
const char* TEST_NS_TOPIC = "/test";
const char* TEST_NOR_TOPIC = "/test/nor";
const char* TEST_TMP_TOPIC = "/test/tmp";
const char* TEST_SEQ_TOPIC = "/test/seq";
const char* TEST_SEQTMP_TOPIC = "/test/seqtmp"; 

const dir_def_t CONFIG_ROOT = 
{
	CONFIG_NS_TOPIC, NODE_ROOT, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_EPHEMERAL 
};

const dir_def_t pubsub::CPubSubConf::HUMMER_ROOT = 
{
	HUMMER_NS_TOPIC, NODE_ROOT, ZOO_TYPE_NORMAL, NODE_MIX, ZOO_TYPE_NORMAL 
};

const dir_def_t HUMMER_CONFIG = 
{
	HUMMER_CONFIG_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_NORMAL 
};

const dir_def_t HUMMER_WORKER = 
{
	HUMMER_WORKER_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_EPHEMERAL 
};

const dir_def_t HUMMER_TASK = 
{
	HUMMER_TASK_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_NORMAL
};

const dir_def_t HUMMER_TASKHEARTBEAT = 
{
	HUMMER_TASK_HEARTBEAT_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_EPHEMERAL
};

const dir_def_t HUMMER_SUBMIT_JOB = 
{
	HUMMER_SUBMIT_JOB_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_NORMAL
};

const dir_def_t HUMMER_COMMIT_JOB = 
{
	HUMMER_COMMIT_JOB_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_NORMAL
};

const dir_def_t HUMMER_MASTER = 
{
	HUMMER_MASTER_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_EPHEMERAL
};

const dir_def_t pubsub::CPubSubConf::COUNTER_ROOT =
{
	COUNTER_NS_TOPIC, NODE_ROOT, ZOO_TYPE_NORMAL, NODE_DIR, ZOO_TYPE_NORMAL
};

const dir_def_t pubsub::CPubSubConf::COUNTER_LOCK =
{
	COUNTER_LOCK_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_EPHEMERAL
};

const dir_def_t pubsub::CPubSubConf::COUNTER_VALUE =
{
	COUNTER_VALUE_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_NORMAL
};

const dir_def_t pubsub::CPubSubConf::TEST_ROOT = 
{
	TEST_NS_TOPIC, NODE_ROOT, ZOO_TYPE_NORMAL, NODE_DIR, ZOO_TYPE_NORMAL 
};

const dir_def_t pubsub::CPubSubConf::TEST_NOR_DIR = 
{
	TEST_NOR_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_NORMAL 
};

const dir_def_t pubsub::CPubSubConf::TEST_TMP_DIR = 
{
	TEST_TMP_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_EPHEMERAL
};

const dir_def_t pubsub::CPubSubConf::TEST_SEQ_DIR = 
{
	TEST_SEQ_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_SEQUNCE
};

const dir_def_t pubsub::CPubSubConf::TEST_SEQTMP_DIR = 
{
	TEST_SEQTMP_TOPIC, NODE_DIR, ZOO_TYPE_NORMAL, NODE_LEAF, ZOO_TYPE_EPHSEQ
};



pubsub::CPubSubConf confInstance;

pubsub::CPubSubConf& pubsub::CPubSubConf::GetInstance()
{
	return confInstance;
}

pubsub::CPubSubConf::CPubSubConf(){}

pubsub::CPubSubConf::~CPubSubConf(){}

const int pubsub::CPubSubConf::HUMMER_NS = 0;

const int pubsub::CPubSubConf::CONFIG_NS = 2;

const int pubsub::CPubSubConf::COUNTER_NS = 3;

const int pubsub::CPubSubConf::TEST_NS = 4;

void pubsub::CPubSubConf::Init(const char* confpath)
{
	if (NULL == confpath || !LoadConf(confpath))
	{
		LoadDefault();	 
	}
	return;
}

bool pubsub::CPubSubConf::LoadConf(const char* confpath)
{
	PB_NOUSE(confpath);
	return 0;
}

void pubsub::CPubSubConf::LoadDefault()
{
	m_TestNs.push_back(TEST_ROOT);
	m_TestNs.push_back(TEST_NOR_DIR);
	m_TestNs.push_back(TEST_TMP_DIR);
	m_TestNs.push_back(TEST_SEQ_DIR);
	m_TestNs.push_back(TEST_SEQTMP_DIR);

	m_CounterNs.push_back(COUNTER_ROOT);
	m_CounterNs.push_back(COUNTER_LOCK);
	m_CounterNs.push_back(COUNTER_VALUE);

	m_HummerNs.push_back(HUMMER_ROOT);
	m_HummerNs.push_back(HUMMER_CONFIG);
	m_HummerNs.push_back(HUMMER_WORKER);
	m_HummerNs.push_back(HUMMER_TASK);
	m_HummerNs.push_back(HUMMER_TASKHEARTBEAT);
	m_HummerNs.push_back(HUMMER_SUBMIT_JOB);
	m_HummerNs.push_back(HUMMER_COMMIT_JOB);
	m_HummerNs.push_back(HUMMER_TASKHEARTBEAT);
	m_HummerNs.push_back(HUMMER_MASTER);

	m_ConfNs.push_back(CONFIG_ROOT);	
	return;
}

bool pubsub::CPubSubConf::GetNSRoot(int nstype, CDirDef& dir)
{
	if (nstype == HUMMER_NS)
	{
		dir = HUMMER_ROOT;
	}
	else if (nstype == COUNTER_NS)
	{
		dir = COUNTER_ROOT;
	}
	else if (nstype == TEST_NS)
	{
		dir = TEST_ROOT;
	}
	else if (nstype == CONFIG_NS)
	{
		dir = CONFIG_ROOT;
	}
	else
	{
		return false;
	}
	return true;	
}

bool pubsub::CPubSubConf::GetNSDirs(int nstype, list<CDirDef>& ns)
{
	if (nstype == HUMMER_NS)
	{
		ns = m_HummerNs;
	}
	else if (nstype == COUNTER_NS)
	{
		ns = m_CounterNs;
	}
	else if (nstype == TEST_NS)
	{
		ns = m_TestNs;
	}
	else if (nstype == CONFIG_NS)
	{
		ns = m_ConfNs;
	}
	else
	{
		return false;
	}
	return true;	
}

bool pubsub::CPubSubConf::GetNSList(list<int>& nslist)
{
	nslist.push_back(CPubSubConf::HUMMER_NS);
	nslist.push_back(CPubSubConf::CONFIG_NS);
	nslist.push_back(CPubSubConf::COUNTER_NS);
	nslist.push_back(CPubSubConf::TEST_NS);

	return true;
}

size_t pubsub::CPubSubConf::GetNsTopicNum(int nstype)
{
	if (nstype == HUMMER_NS)
	{
		return m_HummerNs.size();
	}
	else if (nstype == COUNTER_NS)
	{
		return m_CounterNs.size();
	}
	else if (nstype == TEST_NS)
	{
		return m_TestNs.size();
	}
	else
	{
		return 0;
	}

	return 5;
}

int pubsub::CPubSubConf::GetTopic(const char* ns, TopicId appid, const char* name, string& topic)
{
	if (0 == strcmp(ns, HUMMER_NS_STRING))
	{
		return GetHummerTopic(appid, name, topic);	
	}
	else if (0 == strcmp(ns, COUNTER_NS_STRING))
	{
		return GetCounterTopic(appid, name, topic);
	}
	else if (0 == strcmp(ns, TEST_NS_STRING))
	{
		return GetTestTopic(appid, name, topic);
	}
	else if (0 == strcmp(ns, CONFIG_NS_STRING))
	{
		return GetConfigTopic(appid, name, topic);
	}
	else
	{
		LOG_TRACE(LOG_PUBSUB_HEAD<<"Get Topic,can not find namespace");
		return -1;
	}
}

int pubsub::CPubSubConf::GetHummerTopic(TopicId appid, const char* name, string& topic)
{
	switch(appid)
	{
	case TOPIC_ROOT:
		topic = HUMMER_NS_TOPIC;
		break;
	case TOPIC_CONF:
		topic = HUMMER_CONFIG_TOPIC;
		topic += "/";
		topic += name;
		break;
	case TOPIC_WORKER:
		topic = HUMMER_WORKER_TOPIC;
		topic += "/";
		topic += name;
		break;
	case TOPIC_TASK:
		topic = HUMMER_TASK_TOPIC;
		topic += "/";
		topic += name;
		break;
	case TOPIC_TASK_HEARTBEAT:
		topic = HUMMER_TASK_HEARTBEAT_TOPIC;
		topic += "/";
		topic += name;
		break;
	case TOPIC_SUBMIT_JOB:
		topic = HUMMER_SUBMIT_JOB_TOPIC;
		topic += "/";
		topic += name;
		break;
	case TOPIC_COMMIT_JOB:
		topic = HUMMER_COMMIT_JOB_TOPIC;
		topic += "/";
		topic += name;
		break;
	case TOPIC_MASTER:
		topic = HUMMER_MASTER_TOPIC;
		topic += "/";
		topic += name;
		break;
	default:
		LOG_TRACE(LOG_PUBSUB_HEAD<<"hummer topic invalid");
		return -1;
	}
	PB_NOUSE(name);
	return 0;
}

int pubsub::CPubSubConf::GetTestTopic(TopicId appid, const char* name, string& topic)
{
	switch(appid)
	{
	case TOPIC_ROOT:
		topic = TEST_NS_TOPIC; 
		break;
	case TOPIC_TEST_NOR:
		topic = TEST_NOR_TOPIC;
		topic += "/";
		topic += name;
		break;
	case TOPIC_TEST_TMP:
		topic = TEST_TMP_TOPIC;
		topic += "/";
		topic += name;
		break;
	case TOPIC_TEST_SEQ:
		topic = TEST_SEQ_TOPIC;
		topic += "/";
		topic += name;
		break;
	case TOPIC_TEST_SEQTMP:
		topic = TEST_SEQTMP_TOPIC;
		topic += "/";
		topic += name;
		break;
	default:
		return -1;
	}
	return 0;
}

int pubsub::CPubSubConf::GetCounterTopic(TopicId appid, const char* name, string& topic)
{
	switch(appid)
	{
	case TOPIC_ROOT:
		topic = COUNTER_NS_TOPIC;
		break;
	case TOPIC_COUNTER:
		topic = COUNTER_LOCK_TOPIC;
		topic += "/";
		topic += name;
		break;
	default:
		LOG_TRACE(LOG_PUBSUB_HEAD<<"counter topic invalid");
		return -1;
	}
	return 0; 
}

int pubsub::CPubSubConf::GetConfigTopic(TopicId appid, const char* name, string& topic)
{
	switch(appid)
	{
	case TOPIC_CONFIG_SERVER:
		topic = CONFIG_NS_TOPIC;
		topic += "/";
		topic += name;
		break;
	case TOPIC_ROOT:
		topic = CONFIG_NS_TOPIC;
		break;
	default:
		LOG_TRACE(LOG_PUBSUB_HEAD<<"config topic invalid");
		return -1;
	}
	return 0;
}

