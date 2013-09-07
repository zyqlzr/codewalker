/*****************************************************
 * Copyright zhengyang   
 * FileName: topicwatcher.hpp 
 * Description: the implication of topic watcher 
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_TOPIC_WATCHER_H
#define PUBSUB_TOPIC_WATCHER_H

#include "zoowatcher.hpp"
#include "topic.hpp"
#include "pbconf.hpp"
#include "pbdef.hpp"
#include "mutex.hpp"
#include "rule.hpp"
#include "listenerfactory.hpp"
#include "zutil.hpp" 
#include <map>
#include <string>
#include <list>
using std::list;
using std::map;
using std::string;

namespace pubsub
{

class CTopicWatcher : public CZooWatcher, public CTopicsWrapper
{
public:
	CTopicWatcher();
	~CTopicWatcher();
	int Init(const char* zooaddr, const char* confpath, StatusNotifyCB cb, void* arg);
	void UnInit();
	int Status()const; 
	bool IsConnected();

	int CreateTopic(const char* topic, char* data, size_t len);
	int DelTopic(const char* topic);
	int TopicExist(const char* topic);
	int TopicGet(const char* ns, TopicId appid, const char*topic_name, string& topic);

	int pub(const char* path, char* data, size_t len);
	int sub(const char* path, WatchHandler notifycb, void* arg);
	int Unsub(const char* path);
	int push(const char* path, char* data, size_t len);
	int pull(const char* path, char* data, size_t& len);
	int pullChild(const char* path, set<string>& newChild);

	int ZooEvent(int type, const char* path);
	void ZooConnEvent();
	void ZooDisconnEvent();

	int CounterCreate(const char* counter, int64_t& initcount); 
	int CounterExist(const char* counterName);
	int CounterGetNext(const char* counterName, int64_t& count);
	int CounterGetCurrent(const char* counterName, int64_t& count);
	int CounterDelete(const char* counterName);

	pubsub::CZooUtil& ZooApi();	
private:
	pubsub::WatchType GetWatchType(NodeLevel level, const string& path, const string& parent);
	bool TryLockCounter(const char* countername);
	void UnLockCounter(const char* countername);
	bool CheckAndGetCounter(const char* countername, counter_value_t& value);
private:
	static const time_t INIT_WAIT_TIME; 
	void WaitConn(time_t sec);
	void NotifyConn();
	bool Watch(const char* path, NodeLevel level, SubPattern& pattern);
	bool UnWatch(const char* path, NodeLevel level, SubPattern& pattern);
	int PreDefineHDir(list<CDirDef>& predefs);
	int InitRootPath(const CDirDef* root);
private:
	bool m_bConnFlag;
	map<string, CTopicListener*> m_Topics_;
	CRules m_RuleDef_;
	CZooUtil m_cZApi_;
	bool m_InitFlag;
	pthread_cond_t m_ConnCond;
	CMutex m_CondLock;
	StatusNotifyCB m_StatusCB;
	void* m_StatusArg;
};

}

#endif
