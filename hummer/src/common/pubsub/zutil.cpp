/*****************************************************
Copyright 2010-20XX ZY 
FileName: clusterzoo.cpp
Description: the wrapper of zookeeper api 
Author: zhengyang
Date: 2013.06.03
 *****************************************************/

#include "zookeeper.h"
#include "zookeeper.jute.h"

#include "zutil.hpp"
#include "zoowatcher.hpp"
#include "pbdef.hpp"
#include "pubsub.hpp"
#include "pbconf.hpp"

#include "Logger.h"

int pubsub::CZooUtil::Init(CZooWatcher* watcher)
{
	if (NULL == watcher)
	{
		return PB_SYSERR;
	}
	
	m_cWatcher_ = watcher;
	return PB_OK;
}

void pubsub::CZooUtil::UnInit()
{
	m_cWatcher_ = NULL;
	return;
}

pubsub::CZooUtil::CZooUtil(): m_cWatcher_(NULL){}

pubsub::CZooUtil::~CZooUtil(){}

int pubsub::CZooUtil::GetZooStat(const char* path, struct Stat& nodezstat)
{	
	int ret = PB_OK;
	if (NULL == m_cWatcher_ 
		|| NULL == m_cWatcher_->GetHandle()
		/*|| !(m_cWatcher_->IsConnected())*/)
	{
		return PB_NETERR;
	}
	
	ret = zoo_exists(m_cWatcher_->GetHandle(), path, UN_WATCH, &nodezstat);
	return ErrorCodeTran(ret);
}

int pubsub::CZooUtil::GetVersion(const char* path, int32_t& version)
{
	int ret = PB_OK;
	struct Stat newStat;
		
	ret = GetZooStat(path, newStat);
	if (ret != PB_OK)
	{
		return ret;
	}
	
	version = newStat.version;
	return PB_OK;
}

int pubsub::CZooUtil::ParentPath(const string& path, string& parentPath)
{
	string name;
	return ParsePath(path, parentPath, name);
}

int pubsub::CZooUtil::Name(const string& path, string& name)
{
	string parentPath;
	return ParsePath(path, parentPath, name);
}

int pubsub::CZooUtil::Create(const char* path
		, int flag, const char * data, size_t len
		, char* outbuf, size_t outbuflen)
{
	int zooflag = 0;
	int ret = PB_OK;
	if (NULL == m_cWatcher_ 
		|| NULL == m_cWatcher_->GetHandle()
		/*|| !(m_cWatcher_->IsConnected())*/)
	{
		return PB_NETERR;
	}
	
	switch(flag)
	{
	case ZOO_TYPE_NORMAL:
		{
			zooflag = 0;
		}
		break;
	case ZOO_TYPE_EPHEMERAL:
		{ 
			zooflag = ZOO_EPHEMERAL;
		}
		break;
	case ZOO_TYPE_SEQUNCE:
		{
			zooflag = ZOO_SEQUENCE;
		}
		break;
	case ZOO_TYPE_EPHSEQ:
		{
			zooflag = (ZOO_SEQUENCE | ZOO_SEQUENCE);
		}
		break;
	default:
		{
			return PB_SYSERR;
		}
	}
	
	ret = zoo_create(m_cWatcher_->GetHandle(), path, data
		, len, &ZOO_OPEN_ACL_UNSAFE, flag, outbuf, outbuflen);
	return ErrorCodeTran(ret);
}

int pubsub::CZooUtil::Delete(const char* path)
{
	int ret = PB_OK;
	int32_t iver = 0;
	if (NULL == m_cWatcher_ 
		|| NULL == m_cWatcher_->GetHandle()
		/*|| !(m_cWatcher_->IsConnected())*/)
	{
		return PB_NETERR;
	}
	
	ret = GetVersion(path, iver);
	if (PB_OK != ret)
	{
		return ret;
	}
	
	ret = zoo_delete(m_cWatcher_->GetHandle(), path, iver);
	return ErrorCodeTran(ret);
}

int pubsub::CZooUtil::Modify(const char* path, char* dataBuf, size_t dataLen)
{
	int ret = PB_OK;
	int32_t iver = 0;
	if (NULL == m_cWatcher_ 
		|| NULL == m_cWatcher_->GetHandle()
		/*|| !(m_cWatcher_->IsConnected())*/)
	{
		return PB_NETERR;
	}
	
	ret = GetVersion(path, iver);
	if (PB_OK != ret)
	{
		return ret;
	}
	
	ret = zoo_set(m_cWatcher_->GetHandle(), path, dataBuf, dataLen, iver);
	return ErrorCodeTran(ret);
}

int pubsub::CZooUtil::Data(const char* path, char* dataBuf, size_t& dataLen, WatchType wt)
{
	int ret = ZOK;
	struct Stat newStat;
	if (NULL == m_cWatcher_ 
		|| NULL == m_cWatcher_->GetHandle()
		/*|| !(m_cWatcher_->IsConnected())*/)
	{
		return PB_NETERR;
	}
	
	ret = zoo_get(m_cWatcher_->GetHandle(), path, wt, dataBuf, (int*)&dataLen, &newStat);
	return ErrorCodeTran(ret);
}

int pubsub::CZooUtil::Watch(const char* path)
{
	int ret = ZOK;
	struct Stat newStat;
	
	if (NULL == m_cWatcher_ 
		|| NULL == m_cWatcher_->GetHandle()
		/*|| !(m_cWatcher_->IsConnected())*/)
	{
		return PB_NETERR;
	}
	
	ret = zoo_wexists(m_cWatcher_->GetHandle(), path,
			CZooWatcher::WatchNotify, m_cWatcher_, &newStat);
	if (ret != ZNONODE && ret != ZOK)
	{
		return ErrorCodeTran(ret);
	}
	return PB_OK;
}

int pubsub::CZooUtil::UnWatch(const char* path)
{
	int ret = ZOK;
	struct Stat newStat;
	
	if (NULL == m_cWatcher_ 
		|| NULL == m_cWatcher_->GetHandle()
		/*|| !(m_cWatcher_->IsConnected())*/)
	{
		return PB_NETERR;
	}
	
	ret = zoo_exists(m_cWatcher_->GetHandle(), path, 0, &newStat);
	if (ret != ZNONODE && ret != ZOK)
	{
		return ErrorCodeTran(ret);
	}
	return PB_OK;
}

int pubsub::CZooUtil::WatchPChilds(const char* path, set<string>& childSet)
{
	if (-1 == Childs(path, childSet, WATCH))
	{
		return PB_NETERR;
	}
	
	set<string>::iterator iter = childSet.begin();
	for (; iter != childSet.end(); ++iter)
	{
		struct Stat tmpStat;
		string zkPath = path;
		zkPath += "/";
		zkPath += (*iter);
		zoo_wexists(m_cWatcher_->GetHandle(), zkPath.c_str() 
			, CZooWatcher::WatchNotify, m_cWatcher_, &tmpStat);
	}
	
	return PB_OK;
}

int pubsub::CZooUtil::WatchPChilds(const char* path)
{
	set<string> childset;
	if (-1 == Childs(path, childset, WATCH))
	{
		return PB_NETERR;
	}
	
	set<string>::iterator iter = childset.begin();
	for (; iter != childset.end(); ++iter)
	{
		struct Stat tmpStat;
		string zkPath = path;
		zkPath += "/";
		zkPath += (*iter);
		zoo_wexists(m_cWatcher_->GetHandle(), zkPath.c_str() 
			, CZooWatcher::WatchNotify, m_cWatcher_, &tmpStat);
	}
	
	return PB_OK;
}

int pubsub::CZooUtil::UnWatchPChilds(const char* path)
{
	set<string> childset;
	if (-1 == Childs(path, childset, WATCH))
	{
		return PB_NETERR;
	}
	
	set<string>::iterator iter = childset.begin();
	for (; iter != childset.end(); ++iter)
	{
		struct Stat tmpStat;
		string zkPath = path;
		zkPath += "/";
		zkPath += (*iter);
		zoo_exists(m_cWatcher_->GetHandle(), iter->c_str(), 0, &tmpStat);
	}
	return PB_OK;
}

bool pubsub::CZooUtil::Exist(const char* path, WatchType wt)
{
	int ret = ZOK;
	struct Stat newStat;
	
	if (NULL == m_cWatcher_ 
		|| NULL == m_cWatcher_->GetHandle()
		/*|| !(m_cWatcher_->IsConnected())*/)
	{
		return PB_NETERR;
	}
	
	ret = zoo_exists(m_cWatcher_->GetHandle(), path, wt, &newStat);
	return ErrorCodeTran(ret);
}

int pubsub::CZooUtil::Childs(const char* path, set<string>& childset, WatchType wt)
{
	struct String_vector strings;
	int ret = PB_OK;
	if (NULL == m_cWatcher_ 
		|| NULL == m_cWatcher_->GetHandle())
	{
		return PB_NETERR;
	}
	
	memset(&strings, 0, sizeof(struct String_vector));
	ret = zoo_get_children(m_cWatcher_->GetHandle(), path, wt, &strings);
	if (ZOK != ret)
	{
		freeZookeeperVector(&strings);
		return ErrorCodeTran(ret);
	}
	
	for (int i = 0; i < strings.count; ++i)
	{
		childset.insert(strings.data[i]);
	}
	freeZookeeperVector(&strings);
	return PB_OK;
}

void pubsub::CZooUtil::freeZookeeperVector(struct String_vector* strings)
{
	if (NULL != strings 
		&& NULL != strings->data 
		&& (strings->count > 0))
	{
		for (int i = 0; i < strings->count; ++i)
		{
			if (NULL == strings->data[i])
			{
				continue;
			}
			free(strings->data[i]);
		}
		free(strings->data);
	}
	return;
}

int pubsub::CZooUtil::ParsePath(const string& path, string& parent, string& name)
{
	if (path.length() <= 1)
	{
		return PB_TOPICINVALID;
	}
	
	string::size_type pos = path.find_last_of('/');
	if (string::npos == pos)
	{
		return PB_TOPICINVALID;
	}
	
	name = path.substr(pos + 1);
	parent = path.substr(0, pos);
	return PB_OK;
}

int pubsub::CZooUtil::ErrorCodeTran(int zooerr)
{
	int pbcode = PB_OK;
	switch(zooerr)
	{
	case ZOK:
		return pbcode;
	case ZBADARGUMENTS:
		{
			pbcode = PB_SYSERR;
		}
		break;
	case ZNONODE:
		{
			pbcode = PB_NOTOPIC;
		}
		break;
	case ZNOAUTH:
	case ZINVALIDSTATE:
		{
			pbcode = PB_NETERR;
		}
		break;
	case ZBADVERSION:
		{	
			pbcode = PB_WRITECONFLICT;
		}
		break;
	case ZMARSHALLINGERROR:
		{
			pbcode = PB_DATAOVER;
		}
		break;
	case ZNODEEXISTS:
		{
			pbcode = PB_TOPICEXIST;
		}
		return pbcode;	
	default:
		{
			pbcode = PB_SYSERR;
		}
		break;
	}
	LOG_TRACE(LOG_PUBSUB_HEAD<<"zoo ops errorcode="<<zooerr);
	return pbcode;
}

