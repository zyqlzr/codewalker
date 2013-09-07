/*****************************************************
Copyright 2010-20XX ZY 
FileName: clusterzoo.cpp
Description: the wrapper of zookeeper api 
Author: zhengyang
Date: 2013.06.03
*****************************************************/
#include "pubsub.hpp"
#include "topic.hpp"
#include "pbdef.hpp"

#include "mutex.hpp"

#include "Logger.h"

using namespace pubsub;

pbHandle pubsub_init(PubSubType type, const char* addr, StatusNotify statusCB, void* arg)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = CTopicFactory::GetInstance().CreateWrapper(type, addr, NULL, statusCB, arg);
	if (NULL != wrapper)
	{ 
		return (pbHandle)wrapper;
	}
	else
	{
		LOG_TRACE(LOG_PUBSUB_HEAD<<"conn failed");
		return NULL;
	}
}

PubsubStatus pubsub_status(pbHandle pbh)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper)
	{
		return PUBSUB_STATUS_ERR;
	}

	return (PubsubStatus)wrapper->Status();
}

int create_topic(pbHandle pbh, const char* topic, char* data, size_t len)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == topic)
	{
		return PB_SYSERR;
	}

	return wrapper->CreateTopic(topic, data, len);
}

int delete_topic(pbHandle pbh, const char* topic)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == topic)
	{
		return PB_SYSERR;
	}

	return wrapper->DelTopic(topic);
}

int exist_topic(pbHandle pbh, const char* topic)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == topic)
	{
		return PB_SYSERR;
	}

	return wrapper->TopicExist(topic);
}

int get_topic(pbHandle pbh, const char* ns, TopicId appid, const char*topic_name, char* buf, size_t& len)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper)
	{
		return PB_SYSERR;
	}
	string topic;
	if(PB_OK != wrapper->TopicGet(ns, appid, topic_name, topic))
	{
		return PB_SYSERR;
	}
	
	if (len < topic.length() || 0 == topic.length())
	{
		return PB_DATAOVER; 
	}
	memcpy(buf, topic.c_str(), topic.length());
	return PB_OK;
}

int pub_topic(pbHandle pbh, const char* topic, char* data, size_t len)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == topic)
	{
		return PB_SYSERR;
	}

	return wrapper->pub(topic, data, len);
}

int sub_topic(pbHandle pbh, const char* topic, DataSubNotify notifycb, void* arg)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == topic)
	{
		return PB_SYSERR;
	}

	return wrapper->sub(topic, notifycb, arg);
}

int unsub_topic(pbHandle pbh, const char* topic)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == topic)
	{
		return PB_SYSERR;
	}

	return wrapper->Unsub(topic);
}

int pubsub_topic(pbHandle pbh, const char* topic, char* data, size_t len, DataSubNotify notifycb, void* arg)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == topic)
	{
		return PB_SYSERR;
	}

	int ret = PB_OK;
	ret = wrapper->pub(topic, data, len);
	if (ret != PB_OK)
	{
		return ret;
	}
	
	return wrapper->sub(topic, notifycb, arg);
}

void pubsub_uninit(pbHandle pbh)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper)
	{
		return;
	}
	
	CTopicFactory::GetInstance().DestroyWrapper(wrapper);
	return;
}

int push_topic(pbHandle pbh, const char* topic, char* data, size_t len)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == topic)
	{
		return PB_SYSERR;
	}

	return wrapper->push(topic, data, len);
}

int pull_topic(pbHandle pbh, const char* topic, char* data, size_t& len)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == topic)
	{
		return PB_SYSERR;
	}

	return wrapper->pull(topic, data, len);
}


int pull_subtopic(pbHandle pbh, const char* topic, set<string>& subtopics)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == topic)
	{
		return PB_SYSERR;
	}
	
	return wrapper->pullChild(topic, subtopics);
}

int create_counter(pbHandle pbh, const char* counterName, int64_t& initcount)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == counterName)
	{
		return PB_SYSERR;
	}
	return wrapper->CounterCreate(counterName, initcount);	
}

int exist_counter(pbHandle pbh, const char* counterName)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == counterName)
	{
		PB_ASSERT(false);
		return PB_SYSERR;
	}
	return wrapper->CounterExist(counterName);
}

int get_next_count(pbHandle pbh, const char* counterName, int64_t& count)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == counterName)
	{
		return PB_SYSERR;
	}

	return wrapper->CounterGetNext(counterName, count);
}

int get_current_count(pbHandle pbh, const char* counterName, int64_t& count)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == counterName)
	{
		return PB_SYSERR;
	}

	return wrapper->CounterGetCurrent(counterName, count);
}

int delete_counter(pbHandle pbh, const char* counterName)
{
	CGuard guard(CTopicFactory::GetInstance().GetLock());
	CTopicsWrapper* wrapper = (CTopicsWrapper*)pbh;
	if (NULL == wrapper || NULL == counterName)
	{
		return PB_SYSERR;
	}
	
	return wrapper->CounterDelete(counterName);
}

