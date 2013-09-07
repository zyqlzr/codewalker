#ifndef PUBSUB_LISTENER_FACTORY_H
#define PUBSUB_LISTENER_FACTORY_H

#include "topic_def.hpp"
#include "listener.hpp"
#include "handler.hpp"
#include "pbconf.hpp"

namespace pubsub
{

class CTopicWatcher;

#define DATA_BUFFER_NUM  10

class CTopicListener : public pubsub::CListener, public pubsub::CHandler
{
public:
	CTopicListener():m_eSub_(SUB_INVALID), m_ePub_(PUB_INVALID)
		, m_eLevel_(NODE_INVALID), m_pWatcher_(NULL){}
	
	virtual ~CTopicListener(){}

	virtual void SetListenMode(pubsub::SubPattern sub, pubsub::PubPattern pub, NodeLevel level)
	{
		m_eSub_ = sub;
		m_ePub_ = pub;
		m_eLevel_ = level;
	}

	void SetWatcher(CTopicWatcher* watcher)
	{
		m_pWatcher_ = watcher;
	}
	
	pubsub::SubPattern GetSubPattern()const
	{
		return m_eSub_;
	}

	pubsub::PubPattern GetPubPattern()const
	{
		return m_ePub_;
	}
	
	NodeLevel GetLevel()const
	{
		return m_eLevel_;
	}
protected:
	pubsub::SubPattern m_eSub_;
	pubsub::PubPattern m_ePub_;
	NodeLevel m_eLevel_;
	CTopicWatcher* m_pWatcher_;
};

class CNodeListener : public pubsub::CTopicListener
{
public:
	CNodeListener();
	~CNodeListener();

	int NodeCreate(const char* path);
	int NodeDelete(const char* path);
	int DateChange(const char* path);
	int ChildChange(const char* path);
private:
	char m_data[MAX_ZOO_DATA_LEN];
	size_t m_len;
};

class CDirListener : public pubsub::CTopicListener
{
public:
	CDirListener(const string& path);
	~CDirListener();

	int NodeCreate(const char* path);
	int NodeDelete(const char* path);
	int DateChange(const char* path);
	int ChildChange(const char* path);
private:
	void ChildCmp(set<string>& childs
			, set<string>& addChild);
	void AddChild(const string& child);
	void DelChild(const string& child);
	CDirListener();
	pubsub::CDirListener& operator=(const pubsub::CDirListener& listener);
private:
	string m_Path;
	char m_data[MAX_ZOO_DATA_LEN];
	size_t m_len;
	set<string> m_Childs;
};

class CListenerFactory
{
public:
	CListenerFactory();
	~CListenerFactory();
	
	CTopicListener* NewListener(const char* path, WatchHandler notifycb, void* arg, NodeLevel level);
	void ReleaseListener(CTopicListener*);

	static CListenerFactory& GetInstance();
};

}

#endif
