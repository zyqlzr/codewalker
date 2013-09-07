#ifndef FILE_NOTIFIER_H
#define FILE_NOTIFIER_H

#include "event.hpp"
#include "thread.hpp"
#include "iopoller.hpp"

#include <string>
#include <map>
#include <set>
#include <list>
using std::string;
using std::map;
using std::set;
using std::list;

class CNotify
{
public:
	CNotify();
	virtual ~CNotify();
	void NotifyEvent(uint32_t mask);
	void NotifyEvents(set<uint32_t>& mask);
	bool Match(uint32_t mask);
	virtual bool Notify(uint32_t mask, const char* dir, const char* filename) = 0;
private:
	bool m_allFlag;
	set<uint32_t> m_events;
};

class CFileNotifier : public CIoEvent
{
public:
	CFileNotifier();
	~CFileNotifier();
	bool Init(CNotify* notify);
	void UnInit();
	bool WatchPath(string& path);
	void UnwatchPath(string& path);
	int IoRead(int fd);
	int IoWrite(int fd);
	int NotifyFd()const;
	void NotifyDirs(list<string>& dirs);
private:
	void PrintErrCode(int err);
	void PrintNotifyEvent(struct inotify_event* event);
private: 
	static const int BUFFER_LEN;
private:
	bool m_InitFlag;
	//set<string> m_paths;
	typedef map<int, string> WATCH_MAP;
	WATCH_MAP m_WatchToPath;
	set<string> m_WatchFaileds;
	int m_WatchFd;
	char* m_buf;
	int m_bufused;
	CNotify* m_notify;	
};

class CNotifierMonitor : public CTimeEvent
{
public:
	CNotifierMonitor();
	~CNotifierMonitor();
	bool Init(CNotify* notify, int time, list<string>& dir);
	void UnInit();
	int Time();
	static void* NotifyLoop(void* arg);
private:
	bool m_exit;
	CThread m_thread;
	CFileNotifier m_notifier;
	CIoPoller m_poller;
};

#endif

