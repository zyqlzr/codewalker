/*****************************************************
Copyright 2010-20XX ZY 
FileName: clusterzoo.cpp
Description: the wrapper of zookeeper api 
Author: zhengyang
Date: 2013.06.03
*****************************************************/

#include "listenerfactory.hpp"
#include "pubsub.hpp"
#include "topicwatcher.hpp"
#include "pbdef.hpp"

#include "Logger.h"

pubsub::CNodeListener::CNodeListener()
: m_len(MAX_ZOO_DATA_LEN)
{
	memset(m_data, 0, MAX_ZOO_DATA_LEN);
}

pubsub::CNodeListener::~CNodeListener(){}

int pubsub::CNodeListener::NodeCreate(const char* path)
{
	LOG_TRACE(LOG_PUBSUB_HEAD<<"node listener create "<<path);
	if (NULL == m_pWatcher_ 
		|| !(m_pWatcher_->IsConnected())
		|| NULL == m_fHandler_)
	{
		return -1;
	}
	
	if (PUB_ABLE == m_ePub_)
	{
		//get the data of node and notify
		int ret = PB_OK;
		m_len = MAX_ZOO_DATA_LEN;

		ret = m_pWatcher_->pull(path, m_data, m_len);
		if (PB_OK != ret)
		{
			PB_ASSERT(false);
			return ret;
		}

		(*m_fHandler_)(NT_TOPIC_CREATE, path, m_data, m_len, m_pArg_);
	}
	else
	{
		// only notify the name of node
		(*m_fHandler_)(NT_TOPIC_CREATE, path, NULL, 0, m_pArg_);
	}

	return 0;
}

int pubsub::CNodeListener::NodeDelete(const char* path)
{
	if (NULL == m_pWatcher_ 
		|| !(m_pWatcher_->IsConnected())
		|| NULL == m_fHandler_)
	{
		return -1;
	}

	m_pWatcher_->TopicExist(path);
	(*m_fHandler_)(NT_TOPIC_DEL, path, NULL, 0, m_pArg_);
	return 0;
}

int pubsub::CNodeListener::DateChange(const char* path)
{
	if (NULL == m_pWatcher_ 
		|| !(m_pWatcher_->IsConnected())
		|| NULL == m_fHandler_)
	{
		return -1;
	}

	if (PUB_ABLE == m_ePub_)
	{
		//get the data of node and notify
		int ret = PB_OK;
		m_len = MAX_ZOO_DATA_LEN;

		ret = m_pWatcher_->pull(path, m_data, m_len);
		if (PB_OK != ret)
		{
			return ret;
		}
		
		(*m_fHandler_)(NT_TOPIC_UPDATE, path, m_data, m_len, m_pArg_);
	}
	else
	{
		// only notify the name of node
		(*m_fHandler_)(NT_TOPIC_UPDATE, path, NULL, 0, m_pArg_);
	}
	
	return 0;
}

int pubsub::CNodeListener::ChildChange(const char* path)
{
	PB_NOUSE(path);
	PB_ASSERT(false);
	return 0;
}

/**************CDirListener definition*************/
pubsub::CDirListener::CDirListener(const string& path)
: m_Path(path), m_len(MAX_ZOO_DATA_LEN)
{
	memset(m_data, 0, MAX_ZOO_DATA_LEN);
}

pubsub::CDirListener::~CDirListener(){}

int pubsub::CDirListener::NodeCreate(const char* path)
{	
	if (NULL == m_pWatcher_ || !(m_pWatcher_->IsConnected()))
	{
		return -1;
	}

	string zkPath;	
	int type = NT_TOPIC_CREATE;
	if (path != m_Path)
	{
		type = NT_TOPIC_CHILD_ADD;
		zkPath = m_Path + "/" + path;
		AddChild(path);
	}
	else
	{
		zkPath = path;
	}

	if (PUB_ABLE == m_ePub_
		|| type == NT_TOPIC_CHILD_ADD)
	{
		int ret = PB_OK;
		m_len = MAX_ZOO_DATA_LEN;

		ret = m_pWatcher_->pull(path, m_data, m_len);
		if (PB_OK != ret)
		{
			return ret;
		}
		
		(*m_fHandler_)(type, path, m_data, m_len, m_pArg_);
	}
	else
	{
		(*m_fHandler_)(NT_TOPIC_CREATE, path, NULL, 0, m_pArg_);
	}

	return 0;
}

int pubsub::CDirListener::NodeDelete(const char* path)
{
	if (NULL == m_pWatcher_ || !(m_pWatcher_->IsConnected()))
	{
		return -1;
	}
	
	if (m_Path == path)
	{
		(*m_fHandler_)(NT_TOPIC_DEL, path, NULL, 0, m_pArg_);
	}
	else 
	{
		DelChild(path);
		(*m_fHandler_)(NT_TOPIC_CHILD_DEL, path, NULL, 0, m_pArg_);
	}
	return 0;
}

int pubsub::CDirListener::DateChange(const char* path)
{
	if (NULL == m_pWatcher_ || !(m_pWatcher_->IsConnected()))
	{
		return -1;
	}

	string zkPath;
	int type = NT_TOPIC_UPDATE;
	if (path != m_Path)
	{
		type = NT_TOPIC_CHILD_CHANGE;
		zkPath = m_Path + "/" + path;
		AddChild(path);
	}
	else
	{
		zkPath = path;
	}
	
	if (PUB_ABLE == m_ePub_ 
		|| type == NT_TOPIC_CHILD_CHANGE)
	{
		int ret = PB_OK;
		m_len = MAX_ZOO_DATA_LEN;

		ret = m_pWatcher_->pull(zkPath.c_str(), m_data, m_len);
		if (PB_OK != ret)
		{
			LOG_TRACE(LOG_PUBSUB_HEAD<<"pull child changed failed");
			return ret;
		}
		//LOG_TRACE(LOG_PUBSUB_HEAD<<"Data change len:"<< m_len<<",max="<<MAX_ZOO_DATA_LEN);
		(*m_fHandler_)(type, path, m_data, m_len, m_pArg_);
	}
	else
	{
		(*m_fHandler_)(type, path, NULL, 0, m_pArg_);
	}

	return PB_OK;
}

void pubsub::CDirListener::AddChild(const string& child)
{
	set<string>::iterator findIter = m_Childs.find(child);
	if (findIter == m_Childs.end())
	{
		m_Childs.insert(child);
	}
	return;
}

void pubsub::CDirListener::DelChild(const string& child)
{
	set<string>::iterator findIter = m_Childs.find(child);
	if (findIter != m_Childs.end())
	{
		m_Childs.erase(findIter);
	}

	return;
}

void pubsub::CDirListener::ChildCmp(set<string>& childs
			, set<string>& addChild)
{
	set<string>::iterator iter = childs.begin();
	for (; iter != childs.end(); ++iter)
	{
		set<string>::iterator findIter = m_Childs.find(*iter);
		if (findIter == m_Childs.end())
		{
			addChild.insert(*iter);
			m_Childs.insert(*iter);
		}
	}

	return;
}

int pubsub::CDirListener::ChildChange(const char* path)
{
	//LOG_TRACE(LOG_PUBSUB_HEAD<<"dir child change,"<<path);
	if (NULL == m_pWatcher_ || !(m_pWatcher_->IsConnected()))
	{
		return -1;
	}

	set<string> childs;
	//get the new child of the directory
	if (-1 == m_pWatcher_->pullChild(path, childs))
	{
		return -1;
	}

	if (SUB_ALL == m_eSub_)
	{
		set<string> addChilds;
		ChildCmp(childs, addChilds);
		//get the data from all of children nodes and notify 
		set<string>::iterator iter = addChilds.begin();
		for (;iter != addChilds.end(); ++iter)
		{
			string fullPath = m_Path + "/" + (*iter);
			//LOG_TRACE(LOG_PUBSUB_HEAD<<"Child="<<(*iter)<<",path="<<fullPath);
			//watch and pull added child data
			m_len = MAX_ZOO_DATA_LEN;
			if (PB_OK != m_pWatcher_->ZooApi().Data(fullPath.c_str(), m_data, m_len, WATCH) 
				|| NULL == m_fHandler_)
			{
				continue;
			}

			(*m_fHandler_)(NT_TOPIC_CHILD_ADD, (*iter).c_str(), m_data, m_len, m_pArg_);
		}
	}
	else
	{
		//only notify the change msg
		if (NULL == m_fHandler_)
		{
			LOG_TRACE(LOG_PUBSUB_HEAD<<"handle is null");
			return -1;
		}

		(*m_fHandler_)(NT_TOPIC_CHILD, m_Path.c_str(), NULL, 0, m_pArg_);
	}
	
	return 0;
}

/****************CListenerFactory definition******************/
pubsub::CListenerFactory::CListenerFactory(){}

pubsub::CListenerFactory::~CListenerFactory(){}
	
pubsub::CTopicListener* pubsub::CListenerFactory::NewListener(const char* path
	, DataSubNotify notifycb, void* arg, NodeLevel level)
{
	PB_NOUSE(path);
	pubsub::CTopicListener* listener = NULL;
	if (level == NODE_ROOT || level == NODE_DIR)
	{
		LOG_TRACE(LOG_PUBSUB_HEAD<<"Dir listener,path="<<path);
		listener = new pubsub::CDirListener(path);
	}
	else if (level == NODE_LEAF)
	{
		LOG_TRACE(LOG_PUBSUB_HEAD<<"Node listener,path="<<path);
		listener = new pubsub::CNodeListener();
	}
	else
	{
		return NULL;
	}
	listener->SetHandle(notifycb, arg);
	return listener;
}

void pubsub::CListenerFactory::ReleaseListener(CTopicListener*)
{
	return;
}

pubsub::CListenerFactory lfactory;

pubsub::CListenerFactory& pubsub::CListenerFactory::GetInstance()
{
	return lfactory;
}

