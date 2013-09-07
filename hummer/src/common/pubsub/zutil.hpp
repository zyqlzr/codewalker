/*****************************************************
 * Copyright zhengyang   
 * FileName: zutil.hpp 
 * Description: the wrapper of zookeeper api 
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_ZNODE_H
#define PUBSUB_ZNODE_H

#include "zookeeper.jute.h"
#include "zookeeper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <set>
#include <string>
using std::set;
using std::string;

namespace pubsub
{

class CZooWatcher;

enum WatchType
{
	UN_WATCH = 0,
	WATCH,
};

typedef void (*WatchCB)(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);

class CZooUtil
{
public:
	CZooUtil();
	virtual ~CZooUtil();

	int ParentPath(const string& path, string& parentPath);
	int Name(const string& path, string& name);

	int Create(const char* path, int flag, const char * data, size_t len, char* outbuf = NULL, size_t outbuflen = 0);
	int Data(const char* path, char* dataBuf, size_t& dataLen, WatchType wt);
	int Modify(const char* path, char * date, size_t len);
	int Delete(const char* path);
	int Childs(const char* path, set<string>& childset, WatchType wt);
	int Watch(const char* path);
	int UnWatch(const char* path);
	int WatchPChilds(const char* path);
	int WatchPChilds(const char* path, set<string>& childs);
	int UnWatchPChilds(const char* path);
	bool Exist(const char* path, WatchType wt);

	int Init(CZooWatcher* watcher);
	void UnInit();
protected:	
	int GetVersion(const char* path, int32_t& version);
	int GetZooStat(const char* path, struct Stat& nodezstat);
	int ParsePath(const string& path, string& parent, string& name);
	void freeZookeeperVector(struct String_vector* strings);
	int ErrorCodeTran(int zooerr);
protected:
	CZooWatcher* m_cWatcher_;
};


}

#endif

