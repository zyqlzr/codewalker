/*****************************************************
  Copyright 2010-20XX ZY 
FileName: clusterzoo.cpp
Description: the wrapper of zookeeper api 
Author: zhengyang
Date: 2013.06.03
 *****************************************************/

#include "topic.hpp"
#include "topicwatcher.hpp"
#include "pubsub.hpp"

pubsub::CTopicFactory::CTopicFactory(){}

pubsub::CTopicFactory::~CTopicFactory(){}

pubsub::CTopicsWrapper* pubsub::CTopicFactory::CreateWrapper(int type
		, const char* addr, const char* confpath
		, StatusNotifyCB cb, void* arg)
{
	CTopicsWrapper* wrapper = NULL;
	if (type == PUBSUB_ZOO)
	{
		wrapper = new(std::nothrow) CTopicWatcher();
		if (NULL != wrapper && PB_OK == wrapper->Init(addr, confpath, cb, arg))
		{
			return wrapper;	
		}
	}
	return NULL;
}

void pubsub::CTopicFactory::DestroyWrapper(CTopicsWrapper* wrapper)
{
	if (NULL != wrapper)
	{
		wrapper->UnInit();
		delete wrapper;
		wrapper = NULL;
	}
}

pubsub::CTopicFactory topicFactory;

pubsub::CTopicFactory& pubsub::CTopicFactory::GetInstance()
{
	return topicFactory;
}

CMutex* pubsub::CTopicFactory::GetLock()
{
	return &m_FactoryLock;
}



