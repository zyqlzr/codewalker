/*****************************************************
  Copyright 2010-20XX ZY 
FileName: clusterzoo.cpp
Description: the wrapper of zookeeper api 
Author: zhengyang
Date: 2013.06.03
 *****************************************************/

#include "zoowatcher.hpp"
#include "pbdef.hpp"
#include "pubsub.hpp"
#include "Logger.h"

#include <unistd.h>

#define SLEEP_GAP 5

/************************CZooWatcher define**************************/
pubsub::CZooWatcher::CZooWatcher()
:m_eState_(ZOO_WATCH_UNINIT), m_eFlag_(KEEP_WATCH), m_ZooHandle(NULL), m_uThreadSleepTime(SLEEP_GAP)
{}

pubsub::CZooWatcher::~CZooWatcher()
{
	if (NULL != m_ZooHandle)
	{
		m_ZooHandle = NULL;
	}
}

pubsub::ZooWatchState pubsub::CZooWatcher::GetZooWatchState()const
{
	ZooWatchState state = m_eState_;
	return state;
}

zhandle_t* pubsub::CZooWatcher::GetHandle()const
{
	return m_ZooHandle;
}

void pubsub::CZooWatcher::WatchNotify(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx)
{
	CZooWatcher* watcher = (CZooWatcher*)watcherCtx;
	if (NULL == zh || NULL == watcher)
	{
		return;
	}

	//LOG_TRACE("ZOO Notify:type="<<type<<",status="<<state<<",path"<<path);	
	watcher->ConnState(type, state);
	watcher->ZooEvent(type, path);
	return;
}

const int pubsub::CZooWatcher::ZOO_TIMEOUT = 6000;

void* pubsub::CZooWatcher::WatcherRoutine(void* ptr)
{
	CZooWatcher* watcher = (CZooWatcher*)ptr;
	if (NULL == watcher)
	{
		return NULL;
	}
	
	watcher->m_eState_ = ZOO_WATCH_INIT;
	LOG_TRACE(LOG_PUBSUB_HEAD<<"zoo watch thread"); 
	while(1)
	{
		if (watcher->m_eFlag_ == EXIT_WATCH)
		{
			break;
		}
		
		if (ZOO_WATCH_INIT == watcher->m_eState_ 
			|| ZOO_WATCH_NETERR == watcher->m_eState_)
		{
			LOG_TRACE(LOG_PUBSUB_HEAD<<"conn zoo");
			if (NULL != watcher->m_ZooHandle)
			{
				zookeeper_close(watcher->m_ZooHandle);
				watcher->m_ZooHandle = NULL;
			}
			watcher->m_ZooHandle = zookeeper_init(watcher->m_zooaddr.c_str(), CZooWatcher::WatchNotify, ZOO_TIMEOUT, NULL, (void *)watcher, 0);
			if (NULL == watcher->m_ZooHandle)
			{
				LOG_TRACE(LOG_PUBSUB_HEAD<<"conn zoo failed");
				watcher->m_eState_ = ZOO_WATCH_NETERR;
			}
			else
			{
				LOG_TRACE(LOG_PUBSUB_HEAD<<"conn zoo success");
				watcher->m_eState_ = ZOO_WATCH_CONN;
			}
		}
		
		sleep(watcher->m_uThreadSleepTime);
		//LOG_TRACE(LOG_PUBSUB_HEAD<<"zoo watcher loop, timegap="<<watcher->m_uThreadSleepTime<<" state="<<watcher->m_eState_);
	}
	return NULL;
}

void pubsub::CZooWatcher::ZooConnEvent()
{
	return;
}

void pubsub::CZooWatcher::ZooDisconnEvent()
{
	return;
}

int pubsub::CZooWatcher::ConnState(int type, int state)
{
	if (type == ZOO_SESSION_EVENT 
		&& state == ZOO_CONNECTED_STATE)
	{
		ZooConnEvent();
	}
	else if (type == ZOO_SESSION_EVENT 
		&& (state == ZOO_EXPIRED_SESSION_STATE
		|| state == ZOO_CONNECTING_STATE))
	{
		ZooDisconnEvent();
		m_eState_ = ZOO_WATCH_INIT;
	}
	else
	{
		//state == ZOO_ASSOCIATING_STATE
		//state == ZOO_CONNECTING_STATE

		//ZOO_CONNECTING_STATE, ZOO_AUTH_FAILED_STATE
	}
	return PB_OK;
}


int pubsub::CZooWatcher::ZooEvent(int type, const char* path)
{
	PB_NOUSE(type);
	PB_NOUSE(path);
	return PB_OK;
}

int pubsub::CZooWatcher::StartWatchAsync(string& zooaddr)
{
	m_zooaddr = zooaddr;
	if (KEEP_WATCH != m_eFlag_)
	{
		m_eFlag_ = KEEP_WATCH;
	}

	if (m_cThread_.Start(pubsub::CZooWatcher::WatcherRoutine, (void*)this))
	{
		return PB_OK;
	}
	else
	{
		return PB_SYSERR;
	}
}

int pubsub::CZooWatcher::StartWatchSync(string& zooaddr)
{
	m_zooaddr = zooaddr;
	if (KEEP_WATCH != m_eFlag_)
	{
		m_eFlag_ = KEEP_WATCH;
	}

	LOG_TRACE(LOG_PUBSUB_HEAD<<"conn zoo, addr="<<zooaddr);
	m_ZooHandle = zookeeper_init(m_zooaddr.c_str(), CZooWatcher::WatchNotify, ZOO_TIMEOUT, NULL, (void *)this, 0);
	if (NULL == m_ZooHandle)
	{
		LOG_TRACE(LOG_PUBSUB_HEAD<<"conn zoo failed");
		m_eState_ = ZOO_WATCH_NETERR;
	}
	else
	{
		LOG_TRACE(LOG_PUBSUB_HEAD<<"conn zoo success");
		m_eState_ = ZOO_WATCH_CONN;
	}

	if (m_cThread_.Start(pubsub::CZooWatcher::WatcherRoutine, (void*)this))
	{
		return PB_OK;
	}
	else
	{
		return PB_SYSERR;
	}
}

void pubsub::CZooWatcher::StopWatch()
{
	m_eState_ = ZOO_WATCH_UNINIT;
	m_eFlag_ =  EXIT_WATCH;  
	m_cThread_.Stop();
	if (NULL != m_ZooHandle)
	{
		zookeeper_close(m_ZooHandle);
	}

	m_ZooHandle = NULL;
	return;
}

