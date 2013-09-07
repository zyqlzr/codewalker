/*****************************************************
Copyright 2010-20XX ZY 
FileName: clusterzoo.cpp
Description: the wrapper of zookeeper api 
Author: zhengyang
Date: 2013.06.03
 *****************************************************/

#include "topic.hpp"
#include "pubsub.hpp"
#include "topicwatcher.hpp"
#include "pbdef.hpp" 
#include "nsrule.hpp"
#include "topic_def.hpp"

#include "nodedes.hpp"
#include "Logger.h" 

#include <unistd.h>

pubsub::CTopicWatcher::CTopicWatcher()
: m_bConnFlag(false), m_InitFlag(false)
, m_StatusCB(NULL), m_StatusArg(NULL)
{
	PB_ASSERT(0 == pthread_cond_init(&m_ConnCond, NULL));	
}

pubsub::CTopicWatcher::~CTopicWatcher()
{
	PB_ASSERT(0 == pthread_cond_destroy(&m_ConnCond)); 
}

int pubsub::CTopicWatcher::ZooEvent(int type, const char* path)
{
	if (NULL == path)
	{
		return PB_SYSERR;
	}
	
	//LOG_TRACE(LOG_PUBSUB_HEAD<<"zoo event "<<"type="<<type<<" path="<<path);
	map<string, CTopicListener*>::iterator iter = m_Topics_.find(path);
	if (iter == m_Topics_.end())
	{
		string parent;
		string endName;
		if (-1 == pubsub::CRules::ParsePath(path, parent, endName))
		{
			LOG_TRACE(LOG_PUBSUB_HEAD<<"no find path and path is err");
			return PB_SYSERR;
		}

		map<string, CTopicListener*>::iterator findIter = m_Topics_.find(parent);
		if (findIter == m_Topics_.end())
		{
			LOG_TRACE(LOG_PUBSUB_HEAD<<"no find parent,"<<parent);
			return PB_SYSERR;
		}
		
		if (ZOO_CREATED_EVENT == type)
		{
			//LOG_TRACE(LOG_PUBSUB_HEAD<<path<<" Create");
			findIter->second->NodeCreate(endName.c_str());
		}
		else if (ZOO_DELETED_EVENT == type)
		{
			//LOG_TRACE(LOG_PUBSUB_HEAD<<path<<" Delete");
			findIter->second->NodeDelete(endName.c_str());
		}
		else if (ZOO_CHILD_EVENT == type)
		{
			//LOG_TRACE(LOG_PUBSUB_HEAD<<path<<" Child");
			findIter->second->ChildChange(endName.c_str());
		}
		else if (ZOO_CHANGED_EVENT == type)
		{
			//LOG_TRACE(LOG_PUBSUB_HEAD<<path<<" Change");
			findIter->second->DateChange(endName.c_str());
		}
		else
		{
			//maybe ZOO_SESSION_EVENT and ZOO_NOTWATCHING_EVENT,  do nothing
		}
	}
	else
	{
		if (ZOO_CREATED_EVENT == type)
		{
			//LOG_TRACE(LOG_PUBSUB_HEAD<<path<<" Create");
			iter->second->NodeCreate(path);
		}
		else if (ZOO_DELETED_EVENT == type)
		{
			//LOG_TRACE(LOG_PUBSUB_HEAD<<path<<" Delete");
			iter->second->NodeDelete(path);
		}
		else if (ZOO_CHILD_EVENT == type)
		{
			//LOG_TRACE(LOG_PUBSUB_HEAD<<path<<" Child");
			iter->second->ChildChange(path);
		}
		else if (ZOO_CHANGED_EVENT == type)
		{
			//LOG_TRACE(LOG_PUBSUB_HEAD<<path<<" Change");
			iter->second->DateChange(path);
		}
		else
		{
			//maybe ZOO_SESSION_EVENT and ZOO_NOTWATCHING_EVENT,  do nothing
		}
	}
	return PB_OK;
}

int pubsub::CTopicWatcher::sub(const char* path, WatchHandler notifycb, void* arg)
{
	NodeLevel level = NODE_INVALID;
	ZooNodeType type = ZOO_TYPE_INVALID;
	SubPattern sub = SUB_INVALID;
	PubPattern pub = PUB_INVALID;
	string parentPath;
	string watchPah;
	if (!m_RuleDef_.CheckPath(path, level, type, sub, pub, parentPath, watchPah))
	{
		return PB_TOPICINVALID;
	}
	//check if the path is subscribe or not 
	map<string, CTopicListener*>::iterator iter = m_Topics_.find(watchPah);
	if (iter != m_Topics_.end())
	{
		//already subscribed
		PB_ASSERT(iter->second->GetLevel() == level);
		if (iter->second->GetFunc() != notifycb 
			|| iter->second->GetArg() != arg)
		{
			iter->second->SetHandle(notifycb, arg);
		}

		if (iter->second->GetSubPattern() == sub
			&& iter->second->GetPubPattern() == pub)
		{
			return PB_OK;
		}

		if (!Watch(watchPah.c_str(), level, sub))
		{
			return PB_SYSERR;
		}

		iter->second->SetListenMode(sub, pub, level);
	}
	else
	{
		//new subscribe
		CTopicListener* listener = CListenerFactory::GetInstance().NewListener(watchPah.c_str(), notifycb, arg, level);
		if (NULL == listener)
		{
			return PB_SYSERR;
		}

		if (!Watch(watchPah.c_str(), level, sub))
		{
			return PB_SYSERR;
		}
		listener->SetWatcher(this);
		listener->SetListenMode(sub, pub, level);
		m_Topics_.insert(map<string, CTopicListener*>::value_type(watchPah, listener));
	}
	
	LOG_TRACE(LOG_PUBSUB_HEAD<<" listener number="<<m_Topics_.size());
	return PB_OK;
}

int pubsub::CTopicWatcher::Unsub(const char* path)
{
	NodeLevel level = NODE_INVALID;
	ZooNodeType type = ZOO_TYPE_INVALID;
	pubsub::SubPattern sub = SUB_INVALID;
	string parentPath;
	string watchPath;
	if (!m_RuleDef_.CheckPathSubPattern(path, level, type, sub, parentPath, watchPath))
	{
		return PB_TOPICINVALID;
	}

	map<string, CTopicListener*>::iterator iter = m_Topics_.find(watchPath);
	if (iter != m_Topics_.end())
	{
		m_Topics_.erase(iter);
		if (!UnWatch(watchPath.c_str(), level, sub))
		{
			return PB_SYSERR;
		}
	}

	return PB_OK;
}

pubsub::WatchType pubsub::CTopicWatcher::GetWatchType(NodeLevel level, const string& path, const string& parent)
{
	if (level == NODE_LEAF)
	{
		map<string, CTopicListener*>::iterator find = m_Topics_.find(path);
		if (find != m_Topics_.end())
		{
			return WATCH;
		}

		find = m_Topics_.find(parent);
		if (find == m_Topics_.end())
		{
			return UN_WATCH;
		}
		
		if (find->second->GetSubPattern() == SUB_ALL)
		{
			return WATCH;
		}
		else
		{
			return UN_WATCH;
		}
	}
	else
	{
		map<string, CTopicListener*>::iterator find = m_Topics_.find(path);
		if (find == m_Topics_.end())
		{
			return UN_WATCH;
		}
		else
		{
			return WATCH;
		}	
	}	
}

bool pubsub::CTopicWatcher::Watch(const char* path, NodeLevel level, SubPattern& pattern)
{
	bool ret = false;
	if (level == NODE_DIR || level == NODE_ROOT)
	{
		if (pattern == SUB_ALL)
		{
			ret = (0 == m_cZApi_.WatchPChilds(path)) && (0 == m_cZApi_.Watch(path));
			LOG_TRACE(LOG_PUBSUB_HEAD<<"Watch path="<<path);
		}
		else if (pattern == SUB_SINGLE)
		{
			ret = (0 == m_cZApi_.Watch(path));
		}
	}
	else if (level == NODE_LEAF)
	{
		//LOG_TRACE(LOG_PUBSUB_HEAD<<"path="<<path<<",level="<<level<<",pat="<<pattern);
		PB_ASSERT(pattern == SUB_SINGLE);
		ret = (0 == m_cZApi_.Watch(path));
	}

	return ret;
}

bool pubsub::CTopicWatcher::UnWatch(const char* path, NodeLevel level, SubPattern& pattern)
{
	bool ret = false;
	if (level == NODE_DIR || level == NODE_ROOT)
	{
		if (pattern == SUB_ALL)
		{
			ret = (0 == m_cZApi_.UnWatchPChilds(path) && 0 == m_cZApi_.UnWatch(path));
		}
		else if (pattern == SUB_SINGLE)
		{
			ret = (0 == m_cZApi_.UnWatch(path));
		}
	}
	else
	{
		PB_ASSERT(pattern == SUB_SINGLE);
		ret = (0 == m_cZApi_.UnWatch(path));
	}

	return ret;
}

int pubsub::CTopicWatcher::pub(const char* path, char* data, size_t len)
{
	if (NULL == path)
	{
		return PB_SYSERR;
	}

	NodeLevel level = NODE_INVALID;
	ZooNodeType type = ZOO_TYPE_INVALID;
	PubPattern pattern = PUB_INVALID;
	string parentPath;
	string pubName;
	if (!m_RuleDef_.CheckPathPubPattern(path, level, type, pattern, parentPath, pubName))
	{
		return PB_TOPICINVALID;
	}

	if (pattern == PUB_UNABLE)
	{
		return PB_TOPICFORBID;
	}

	WatchType wt = GetWatchType(level, path, parentPath);
	if (m_cZApi_.Exist(path, wt))
	{
		return m_cZApi_.Create(pubName.c_str(), type, data, len);
	}
	else
	{
		return m_cZApi_.Modify(pubName.c_str(), data, len);
	}
}

int pubsub::CTopicWatcher::push(const char* path, char* data, size_t len)
{
	return pub(path, data, len);
}

int pubsub::CTopicWatcher::pull(const char* path, char* data, size_t& len)
{
	NodeLevel level = NODE_INVALID;
	ZooNodeType type = ZOO_TYPE_INVALID;
	PubPattern pattern = PUB_INVALID;
	string parentPath;
	string pullName;
	if (!m_RuleDef_.CheckPathPubPattern(path, level, type, pattern, parentPath, pullName))
	{
		return PB_TOPICINVALID;
	}

	if (pattern == PUB_UNABLE)
	{
		return PB_TOPICFORBID;
	}

	WatchType wt = GetWatchType(level, path, parentPath);
	return m_cZApi_.Data(pullName.c_str(), data, len, wt);
}

int pubsub::CTopicWatcher::pullChild(const char* path, set<string>& childSet)
{
	NodeLevel level = NODE_INVALID;
	ZooNodeType type = ZOO_TYPE_INVALID;
	pubsub::SubPattern pattern = SUB_INVALID;
	string parentPath;
	string pullName;
	if (!m_RuleDef_.CheckPathSubPattern(path, level, type, pattern, parentPath, pullName)
			|| level == NODE_LEAF)
	{
		return PB_TOPICINVALID;
	}

	WatchType wt = GetWatchType(level, path, parentPath);
	//check the path is subscribed or not
	return m_cZApi_.Childs(pullName.c_str(), childSet, wt);
}

const time_t pubsub::CTopicWatcher::INIT_WAIT_TIME = 5;

void pubsub::CTopicWatcher::WaitConn(time_t sec)
{
	LOG_TRACE(LOG_PUBSUB_HEAD<<"wait conn");
	struct timeval waitTime;
	struct timespec timeParm;
	memset(&waitTime, 0, sizeof(waitTime));
	memset(&timeParm, 0, sizeof(timeParm));
	gettimeofday(&waitTime, NULL);
	waitTime.tv_sec += sec;
	timeParm.tv_sec = waitTime.tv_sec;	
	m_CondLock.Lock();
	pthread_cond_timedwait(&m_ConnCond, m_CondLock.GetMutex(), &timeParm);
	//pthread_cond_wait(&m_ConnCond, m_CondLock.GetMutex());
	m_CondLock.UnLock();
	LOG_TRACE(LOG_PUBSUB_HEAD<<"wait conn ok");
	return;
}

void pubsub::CTopicWatcher::NotifyConn()
{
	//LOG_TRACE(LOG_PUBSUB_HEAD<<"notify conn");
	pthread_cond_signal(&m_ConnCond);
	//LOG_TRACE(LOG_PUBSUB_HEAD<<"notify conn ok");
}

void pubsub::CTopicWatcher::ZooConnEvent()
{
	bool firstNotify = false;
	LOG_TRACE(LOG_PUBSUB_HEAD<<"conn notify"<<"InitFlag="<<m_InitFlag);
	{
		if (!m_InitFlag)
		{
			bool successFlag = true;
			LOG_TRACE(LOG_PUBSUB_HEAD<<"init zoo dir");
			pubsub::CRules::CRuleIterator ruleIter(m_RuleDef_);
			for (; !(ruleIter.IsEnd()); ++ruleIter)
			{
				list<CDirDef> predefs;
				if (!((*ruleIter)->GetPredefineSet(predefs))
					 || !PreDefineHDir(predefs))
				{
					successFlag = false;
					continue;
				}
			} 
			m_InitFlag = successFlag;
			firstNotify = true;
		}
		m_bConnFlag = true;
	}

	NotifyConn();

	if (!firstNotify && NULL != m_StatusCB)
	{
		(*m_StatusCB)(PUBSUB_STATUS_OK, m_StatusArg);
	}

	return;
}

void pubsub::CTopicWatcher::ZooDisconnEvent()
{
	LOG_TRACE(LOG_PUBSUB_HEAD<<"disconn"<<" InitFlag="<<m_InitFlag);
	{
		m_bConnFlag = false;
	}
	NotifyConn();
	
	if (m_InitFlag && NULL != m_StatusCB)
	{
		(*m_StatusCB)(PUBSUB_STATUS_NETERR, m_StatusArg);
	}
	return;
}

bool pubsub::CTopicWatcher::IsConnected()
{
	return m_bConnFlag;
}

int pubsub::CTopicWatcher::Status()const
{
	if (m_InitFlag)
	{
		if (m_bConnFlag)
		{
			return PUBSUB_STATUS_OK;
		}
		else
		{
			return PUBSUB_STATUS_NETERR; 
		}
	}
	else
	{
		return PUBSUB_STATUS_UNINIT;
	}
}

int pubsub::CTopicWatcher::Init(const char* zooaddr, const char* confpath, StatusNotifyCB cb, void* arg)
{
	string addr = zooaddr;
	m_StatusCB = cb;
	m_StatusArg = arg;
	
	CPubSubConf::GetInstance().Init(confpath);
	m_cZApi_.Init(this);
	m_RuleDef_.InitRule(CPubSubConf::GetInstance());
	StartWatchSync(addr);

	WaitConn(INIT_WAIT_TIME);
	m_InitFlag = true;
	return PB_OK;
}

void pubsub::CTopicWatcher::UnInit()
{
	StopWatch();
	m_RuleDef_.UnInitRule();
	m_cZApi_.UnInit();

	m_InitFlag = false;
	m_Topics_.clear();
	return;
}

int pubsub::CTopicWatcher::CreateTopic(const char* topic, char* data, size_t len)
{
	NodeLevel level = NODE_INVALID;
	ZooNodeType type = ZOO_TYPE_INVALID;
	PubPattern pattern = PUB_INVALID;
	string parentPath;
	string topicName;
	if (!m_RuleDef_.CheckPathPubPattern(topic, level, type, pattern, parentPath, topicName))
	{
		return PB_TOPICINVALID;
	}

	if (pattern == PUB_UNABLE)
	{
		return PB_TOPICFORBID;
	}

	return m_cZApi_.Create(topicName.c_str(), type, data, len);
}

int pubsub::CTopicWatcher::DelTopic(const char* topic)
{
	NodeLevel level = NODE_INVALID;
	ZooNodeType type = ZOO_TYPE_INVALID;
	PubPattern pattern = PUB_INVALID;
	string parentPath;
	string topicName;
	if (!m_RuleDef_.CheckPathPubPattern(topic, level, type, pattern, parentPath, topicName))
	{
		return PB_TOPICINVALID;
	}

	if (level != NODE_LEAF)
	{
		return PB_TOPICFORBID;
	}

	return m_cZApi_.Delete(topicName.c_str());
}

int pubsub::CTopicWatcher::TopicExist(const char* topic)
{
	NodeLevel level = NODE_INVALID;
	ZooNodeType type = ZOO_TYPE_INVALID;
	PubPattern pattern = PUB_INVALID;
	string parentPath;
	string topicName;
	if (!m_RuleDef_.CheckPathPubPattern(topic, level, type, pattern, parentPath, topicName))
	{
		return PB_TOPICINVALID;
	}
	WatchType wt = UN_WATCH;
	map<string, CTopicListener*>::iterator iter = m_Topics_.find(topicName.c_str());
	if (iter != m_Topics_.end())
	{
		wt = WATCH;
	}
	
	return m_cZApi_.Exist(topic, wt);
}

int pubsub::CTopicWatcher::TopicGet(const char* ns, TopicId appid, const char*topic_name, string& topic)
{
	return CPubSubConf::GetInstance().GetTopic(ns, appid, topic_name, topic);
}

int pubsub::CTopicWatcher::InitRootPath(const CDirDef* root)
{
	string rootPath = root->GetPath();
	string levelPath;
	string validPath;
	string::size_type pos = string::npos;
	if (rootPath.length() <= 1 || rootPath[0] != '/')
	{
		LOG_TRACE(LOG_PUBSUB_HEAD<<"invalid root path "<<rootPath.c_str());
		return PB_TOPICINVALID;
	}

	levelPath = root->GetPath().substr(1);

	do
	{
		pos = levelPath.find_first_of('/');
		if (pos == string::npos)
		{
			validPath += "/" + levelPath;
		}
		else
		{
			validPath += "/" + levelPath.substr(0, pos);
			levelPath = levelPath.substr(pos+1);
		}

		int ret = m_cZApi_.Create(validPath.c_str(), root->GetType(), NULL, 0);
		if (PB_OK != ret && PB_TOPICEXIST != ret)
		{
			LOG_TRACE(LOG_PUBSUB_HEAD<<"Create root path "<<validPath);
			continue;
		}
	}while(pos != string::npos);

	return PB_OK;
}

int pubsub::CTopicWatcher::PreDefineHDir(list<CDirDef>& predefs)
{
	int ret = PB_OK;
	const CDirDef* root = NULL;
	list<CDirDef>::iterator findIter = predefs.begin();
	for ( ; findIter != predefs.end(); ++findIter)
	{
		if (NODE_ROOT == (*findIter).GetLevel())
		{
			root = &(*findIter);
			break;
		}
	}

	if (NULL == root)
	{
		LOG_TRACE(LOG_PUBSUB_HEAD<<"cannot find root dr");	
		return PB_TOPICINVALID;
	}

	ret = InitRootPath(root);
	if (PB_OK != ret)
	{
		PB_ASSERT(false);
		return ret;
	}

	list<CDirDef>::iterator createIter = predefs.begin();
	for (; createIter != predefs.end(); ++createIter)
	{
		if (NODE_DIR != (*createIter).GetLevel())
		{
			continue;
		}

		int rc = m_cZApi_.Create((*createIter).GetPath().c_str(), (*createIter).GetType(), NULL, 0);
		if (PB_OK != rc && PB_TOPICEXIST != rc)
		{
			LOG_TRACE(LOG_PUBSUB_HEAD<<"create "<<createIter->GetPath()<<"failed");

		}
	}

	return PB_OK;
}

/************************counter api*************************/
bool pubsub::CTopicWatcher::TryLockCounter(const char* counter)
{
	if (PB_OK == TopicExist(counter))
	{
		return false;
	}
	
	if (PB_OK == CreateTopic(counter, NULL, 0))
	{
		return true;
	} 
	else
	{
		return false;
	}
}

void pubsub::CTopicWatcher::UnLockCounter(const char* counter)
{
	if (PB_OK != DelTopic(counter))
	{
		LOG_TRACE(LOG_PUBSUB_HEAD<<"delete lock="<<counter<<" failed");
	}
	return;
}

bool pubsub::CTopicWatcher::CheckAndGetCounter(const char* countername, counter_value_t& value)
{
	size_t len = sizeof(value);
	if (PB_OK == TopicExist(countername) 
		&& PB_OK == pull(countername, (char*)(&value), len))
	{
		return true;
	}
	else
	{
		return false;
	}
}

extern const char* COUNTER_VALUE_TOPIC;
extern const char* COUNTER_LOCK_TOPIC;

int pubsub::CTopicWatcher::CounterCreate(const char* counter, int64_t& initcount)
{
	int ret = PB_OK;
	counter_value_t value;
	memset(&value, 0, sizeof(value));
	string counterValue = COUNTER_VALUE_TOPIC;
	counterValue +="/";
	counterValue += counter;

	string counterLock = COUNTER_LOCK_TOPIC;
	counterLock +="/";
	counterLock +=counter;
	//if the counter is already existed
	if (CheckAndGetCounter(counterValue.c_str(), value))
	{
		initcount = value.iValue;
		return PB_OK;
	}
	
	//lock and create counter and set
	int tryFetchLockNum = 10;
	do
	{	
		if (!TryLockCounter(counterLock.c_str()))
		{
			--tryFetchLockNum;
			LOG_TRACE(LOG_PUBSUB_HEAD<<"Fetch distribute lock failed, try count="<<tryFetchLockNum);
			usleep(10000);
			continue;
		}

		if (!CheckAndGetCounter(counterValue.c_str(), value))
		{
			ret = CreateTopic(counterValue.c_str(), (char*)(&initcount), sizeof(initcount));
		}
		else
		{
			initcount = value.iValue;
		}

		UnLockCounter(counterLock.c_str());
		break;
	}while(tryFetchLockNum > 0);
	return ret;
}
 
int pubsub::CTopicWatcher::CounterExist(const char* counterName)
{
	string counterValue = COUNTER_VALUE_TOPIC;
	counterValue +="/";
	counterValue += counterName;
	
	return TopicExist(counterValue.c_str());
}

int pubsub::CTopicWatcher::CounterGetNext(const char* counterName, int64_t& count)
{
	int ret = PB_OK;
	counter_value_t value;
	memset(&value, 0, sizeof(value));
	string counterValue = COUNTER_VALUE_TOPIC;
	counterValue +="/";
	counterValue += counterName;

	string counterLock = COUNTER_LOCK_TOPIC;
	counterLock +="/";
	counterLock +=counterName;
	if (PB_NOTOPIC == TopicExist(counterValue.c_str()))
	{
		return PB_NOTOPIC;
	}
	
	//lock and create counter and set
	int tryFetchLockNum = 10;
	size_t len = sizeof(value);
	do
	{
		if (!TryLockCounter(counterLock.c_str()))
		{
			--tryFetchLockNum;
			LOG_TRACE(LOG_PUBSUB_HEAD<<"Fetch lock failed in GetNext,count="<<tryFetchLockNum);
			usleep(10000);
			continue;
		}

		ret = pull(counterValue.c_str(), (char*)(&value), len);
		if (PB_OK == ret && len == sizeof(value))
		{
			++(value.iValue);
			ret = push(counterValue.c_str(), (char*)(&value), len);
		}	
		UnLockCounter(counterLock.c_str());
		break;
	}while(tryFetchLockNum > 0);
	
	if (ret == PB_OK)
	{
		count = value.iValue;
	}
	return ret;
}

int pubsub::CTopicWatcher::CounterGetCurrent(const char* counterName, int64_t& count)
{
	counter_value_t value;
	memset(&value, 0, sizeof(value));
	string counterValue = COUNTER_VALUE_TOPIC;
	counterValue +="/";
	counterValue += counterName;

	if (CheckAndGetCounter(counterValue.c_str(), value))
	{
		count = value.iValue;
		return PB_OK;
	} 
	else
	{
		return PB_SYSERR;
	}
}

int pubsub::CTopicWatcher::CounterDelete(const char* counterName)
{
	string counterValue = COUNTER_VALUE_TOPIC;
	counterValue +="/";
	counterValue += counterName;
	if (PB_NOTOPIC == TopicExist(counterValue.c_str()))
	{
		return PB_OK;
	}
	
	return DelTopic(counterValue.c_str());
}

pubsub::CZooUtil& pubsub::CTopicWatcher::ZooApi()
{
	return m_cZApi_;
}


