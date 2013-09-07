#include "filenotifier.hpp"
#include "Logger.h"
#include "pbdef.hpp"

#include <sys/inotify.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

/***********CNotify definition****************/
CNotify::CNotify(): m_allFlag(false){}

CNotify::~CNotify(){}

void CNotify::NotifyEvent(uint32_t mask)
{
	if (mask == IN_ALL_EVENTS)
	{
		m_allFlag = true;
	}
	else
	{
		m_events.insert(mask);
	}
}

void CNotify::NotifyEvents(set<uint32_t>& masks)
{
	if (0 == masks.size())
	{
		return;
	}
	
	set<uint32_t>::iterator iter = masks.begin();
	for (;iter != masks.end(); ++iter)
	{
		if (*iter == IN_ALL_EVENTS)
		{
			m_allFlag = true;
			return;
		}	
	}

	m_events = masks;
	return;
}

bool CNotify::Match(uint32_t mask)
{
	if (m_allFlag)
	{
		return true;
	}
	
	set<uint32_t>::iterator iter = m_events.find(mask);
	if (iter != m_events.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

extern const char* LOG_CONFCHECK_HEAD;
/*****************CFileNotifier definition******************/
const int CFileNotifier::BUFFER_LEN = 2048;

CFileNotifier::CFileNotifier()
: m_InitFlag(false), m_WatchFd(-1)
, m_buf(NULL), m_bufused(0), m_notify(NULL){}

CFileNotifier::~CFileNotifier(){}

bool CFileNotifier::Init(CNotify* notify)
{
	m_WatchFd = inotify_init();
	if (m_WatchFd == -1 || NULL == notify)
	{
		return false;
	}
	
	m_buf = new char[BUFFER_LEN];
	if (NULL == m_buf)
	{
		close(m_WatchFd);
		m_WatchFd = -1;
		return false;
	}

	m_InitFlag = true;
	m_notify = notify;
	return true;
}

void CFileNotifier::UnInit()
{
	if (-1 != m_WatchFd)
	{
		close(m_WatchFd);
	}

	if (NULL != m_buf)
	{
		delete [] m_buf;
	}
}

bool CFileNotifier::WatchPath(string& path)
{
	int wd = inotify_add_watch(m_WatchFd, path.c_str(), IN_ALL_EVENTS);
	if (-1 == wd)
	{
		PrintErrCode(errno);
		m_WatchFaileds.insert(path);	
		return false;
	}
	
	m_WatchToPath.insert(WATCH_MAP::value_type(wd, path));
	return true;
}

void CFileNotifier::UnwatchPath(string& path)
{
	int wd = -1;
	WATCH_MAP::iterator iter = m_WatchToPath.begin();
	for (; iter != m_WatchToPath.end(); ++iter)
	{
		if (path == iter->second)
		{
			wd = iter->first;
		}		
	}

	if (-1 == wd)
	{
		return;
	}

	if (-1 == inotify_rm_watch(m_WatchFd, wd))
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"remove watch failed");
	}
	return;
}

int CFileNotifier::IoRead(int fd)
{
	int index = 0;
	int len = 0;
	if (-1 == fd || m_WatchFd != fd)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"notify fd is err");
		return -1;
	}

	do
	{
		len = read(fd, m_buf, BUFFER_LEN); 
	}while(len < 0 && errno == EINTR);
	
	if (len <= 0)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"read err in notifier");
		return -1;
	}
	
	struct inotify_event* event = NULL;
	do
	{
		event = (struct inotify_event*)(m_buf + index);
		index += sizeof(struct inotify_event) + event->len;
		WATCH_MAP::iterator iter = m_WatchToPath.find(event->wd);
		if (iter == m_WatchToPath.end())
		{
			LOG_TRACE(LOG_CONFCHECK_HEAD<<"event can find path,file="<<event->name);
			continue;
		}

		PrintNotifyEvent(event);
		if (NULL != m_notify 
			&& m_notify->Match(event->mask)
			&& m_notify->Notify(event->mask, iter->second.c_str(), event->name))
		{
			LOG_TRACE(LOG_CONFCHECK_HEAD<<"notify ok,mask="<<event->mask<<" name="<<event->name);
		}
	}
	while(index < len);
	return 0;
}

void CFileNotifier::PrintNotifyEvent(struct inotify_event* event)
{
	if (NULL == event)
	{
		return;
	}

	string eventdes;
	switch(event->mask)
	{
	case IN_ACCESS:
		eventdes = "access event";
		break;
	case IN_ATTRIB:
		eventdes = "meta data changed event";
		break;
	case IN_CLOSE_WRITE:
		eventdes = "file opened for writing is closed";
		break;
	case IN_CLOSE_NOWRITE:
		eventdes = "file not opened for writing is closed";
		break;
	case IN_CREATE:
		eventdes = "file/directory is create";
		break;
	case IN_DELETE:
		eventdes = "file/directory is delete";
		break;
	case IN_DELETE_SELF:
		eventdes = "watched file/directory is deleted self";
		break;
	case IN_MODIFY:
		eventdes = "file is modified";
		break;
	case IN_MOVE_SELF:
		eventdes = "watched file/dir is moved self";
		break;
	case IN_MOVED_TO:
		eventdes = "file is moved to watched dir";
		break;
	case IN_MOVED_FROM:
		eventdes = "file is moved from watched dir";
		break;
	case IN_OPEN:
		eventdes = "file is opened";
		break;
	default:
		eventdes = "other event";
		break;
	}
	LOG_TRACE(LOG_CONFCHECK_HEAD<<"mask="<<event->mask<<" des="<<eventdes<<" file="<<event->name);
	return;
}

int CFileNotifier::IoWrite(int fd)
{
	PB_NOUSE(fd);
	PB_ASSERT(false);
	return -1;
}

int CFileNotifier::NotifyFd()const
{
	return m_WatchFd;
}

void CFileNotifier::PrintErrCode(int err)
{
	LOG_TRACE(LOG_CONFCHECK_HEAD<<"file notify err, "<<strerror(err));
}

void CFileNotifier::NotifyDirs(list<string>& dirs)
{
	WATCH_MAP::iterator iter = m_WatchToPath.begin();
	for (; iter != m_WatchToPath.end(); ++iter)
	{
		dirs.push_back(iter->second);
	}
	return;
}

/**************CNotifierMonitor definition****************/
CNotifierMonitor::CNotifierMonitor(): m_exit(false){}

CNotifierMonitor::~CNotifierMonitor(){}

bool CNotifierMonitor::Init(CNotify* notify, int time, list<string>& dirs)
{
	if (NULL == notify || time <= 0)
	{
		PB_ASSERT(false);
		return false;
	}

	SetTimerSec(time);
	if (!m_notifier.Init(notify) 
		|| -1 == m_poller.Init(IO_TYPE_EPOLL))
	{
		PB_ASSERT(false);
		return false;
	}
	
	if (-1 == m_poller.AddRead(m_notifier.NotifyFd(), &m_notifier) 
		|| -1 == m_poller.AddTime(this))
	{
		PB_ASSERT(false);
		return false;
	}
	
	list<string>::iterator iter = dirs.begin();
	for (; iter != dirs.end(); ++iter)
	{
		if (!m_notifier.WatchPath(*iter))
		{
			LOG_TRACE(LOG_CONFCHECK_HEAD<<"watch failed,"<<*iter);
		}
		else
		{
			LOG_TRACE(LOG_CONFCHECK_HEAD<<"watch "<<*iter);
		}
	}	
	
	return m_thread.Start(CNotifierMonitor::NotifyLoop,(void*)this);	
}

void CNotifierMonitor::UnInit()
{
	m_exit = true;
	m_thread.Stop();
	m_notifier.UnInit();
	m_poller.UnInit();
}

static int loopPrint = 0;

int CNotifierMonitor::Time()
{
	list<string> dirs;
	m_notifier.NotifyDirs(dirs);
	if (loopPrint % 20 == 0)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"file monitor timer,dirs="<<dirs.size());
		list<string>::iterator iter = dirs.begin();
		for (; iter != dirs.end(); ++iter)
		{
			LOG_TRACE(LOG_CONFCHECK_HEAD<<"Timer, dir="<<*iter);
		}
		loopPrint = 0;
	}

	++loopPrint;
	return 0;
}

void* CNotifierMonitor::NotifyLoop(void* arg)
{
	CNotifierMonitor* monitor = (CNotifierMonitor*)arg;
	if (NULL == monitor)
	{
		return NULL;
	}

	while (!monitor->m_exit)
	{
		monitor->m_poller.Event();
	}
	return NULL;
}

