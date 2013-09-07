/*****************************************************
 * Copyright zhengyang   
 * FileName: zoowatcher.hpp 
 * Description: the zookeeper event watcher 
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_ZOO_WATCHER_H
#define PUBSUB_ZOO_WATCHER_H

#include "thread.hpp"

#include "zookeeper.jute.h"
#include "zookeeper.h"

#include <stdio.h>
#include <stdlib.h>

#include <string>
using std::string;

namespace pubsub
{

enum ZooWatchState
{
    ZOO_WATCH_UNINIT = 0,
    ZOO_WATCH_INIT,
    ZOO_WATCH_CONN,
    ZOO_WATCH_NETERR,
    ROOT_MAX
};

enum WATCHER_FLAG
{
	KEEP_WATCH = 0,
	EXIT_WATCH,
};

class CZooWatcher
{
public:
    CZooWatcher();
    virtual ~CZooWatcher();
    
    static void WatchNotify(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx);
    static void* WatcherRoutine(void* ptr);

	int StartWatchSync(string& zooaddr);
    int StartWatchAsync(string& zooaddr);
    void StopWatch();
 
   	zhandle_t* GetHandle()const;
	pubsub::ZooWatchState GetZooWatchState()const;

	virtual int ZooEvent(int type, const char* path);
	virtual void ZooConnEvent();
	virtual void ZooDisconnEvent();
	static const int ZOO_TIMEOUT;
protected:
   	int ConnState(int type, int state);
protected:
  	ZooWatchState m_eState_;
	WATCHER_FLAG m_eFlag_;
	zhandle_t* m_ZooHandle;
	size_t m_uThreadSleepTime;
   	CThread m_cThread_;
   	string m_zooaddr;
};

}

#endif
