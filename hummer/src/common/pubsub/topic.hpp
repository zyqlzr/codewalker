/*****************************************************
 * Copyright zhengyang   
 * FileName: topic.hpp 
 * Description: the parent topic subscriber
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_TOPIC_H
#define PUBSUB_TOPIC_H

#include <set>
#include <string>
using std::set;
using std::string;

#include "handler.hpp"
#include "mutex.hpp"
#include "topic_def.hpp"

namespace pubsub
{

typedef struct 
{
	int64_t iValue;
}counter_value_t;

typedef int (*StatusNotifyCB)(int status, void* arg);

class CTopicsWrapper
{
public:
	CTopicsWrapper(){}
	virtual ~CTopicsWrapper(){}
	virtual int Init(const char* addr, const char* confpath
			, StatusNotifyCB cb, void* arg) = 0;
	virtual void UnInit() = 0;

	virtual int Status()const = 0;  
	virtual int CreateTopic(const char* topic, char* data, size_t len) = 0;
	virtual int DelTopic(const char* topic) = 0;
	virtual int TopicExist(const char* topic) = 0;
	virtual int TopicGet(const char* ns, TopicId appid, const char*topic_name, string& topic) = 0;
	
	virtual int pub(const char* path, char* data, size_t len) = 0;
	virtual int sub(const char* path, WatchHandler notifycb, void* arg) = 0;
	virtual int Unsub(const char* path) = 0;
	virtual int push(const char* path, char* data, size_t len) = 0;
	virtual int pull(const char* path, char* data, size_t& len) = 0;
	virtual int pullChild(const char* path, set<string>& newChild) = 0;

	virtual int CounterCreate(const char* counter, int64_t& initcount) = 0; 
	virtual int CounterExist(const char* counterName) = 0;
	virtual int CounterGetNext(const char* counterName, int64_t& count) = 0;
	virtual int CounterGetCurrent(const char* counterName, int64_t& count) = 0;
	virtual int CounterDelete(const char* counterName) = 0;

};

class CTopicFactory
{
public:
	CTopicFactory();
	~CTopicFactory();
	CTopicsWrapper* CreateWrapper(int type, const char* addr
		, const char* confpath, StatusNotifyCB cb, void* arg);
	void DestroyWrapper(CTopicsWrapper* wrapper);
	CMutex* GetLock();
	static CTopicFactory& GetInstance();
private:
	CMutex m_FactoryLock;
};

}

#endif
