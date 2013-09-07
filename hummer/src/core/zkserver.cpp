#include "zkserver.hpp"

#include "Logger.h"
#include "pbdef.hpp"

hummer::CZkServer::CZkServer(): m_handle(NULL)
, m_status(PUBSUB_STATUS_UNINIT)
, m_code(PB_OK), m_topicbuf(NULL){}

hummer::CZkServer::~CZkServer(){}

bool hummer::CZkServer::Init(const char* addr)
{
	m_topicbuf = new(std::nothrow) char[TOPIC_LENGTH];
	if (NULL == m_topicbuf)
	{
		LOG_TRACE("malloc failed");
		return false;
	}

	m_handle = pubsub_init(PUBSUB_ZOO, addr, hummer::CZkServer::RegisterZK, (void*)this);
	if (NULL == m_handle)
	{
		LOG_TRACE("Init pubsub failed,addr="<<addr);
		delete [] m_topicbuf;
		m_topicbuf = NULL;
		return false;
	}

	memset(m_topicbuf, 0, TOPIC_LENGTH);
	m_status = pubsub_status(m_handle);
	if (PUBSUB_STATUS_OK == m_status)
	{
		RegisterZK(PUBSUB_STATUS_OK, (void*)this);
	}
	return true;
}

void hummer::CZkServer::Unit()
{
	pubsub_uninit(m_handle);
	m_status = PUBSUB_STATUS_UNINIT;
	if (NULL != m_topicbuf)
	{
		delete [] m_topicbuf;
		m_topicbuf = NULL;
	}
}

PubsubStatus hummer::CZkServer::Status()const
{
	return m_status;
}

PubsubErrCode hummer::CZkServer::Code()const
{
	return (PubsubErrCode)m_code;
} 

bool hummer::CZkServer::exist(const char* ns, TopicId id, const char* name)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	m_code = get_topic(m_handle, ns, id, name, m_topicbuf, tlen);
	if(PB_OK != m_code)
	{
		LOG_ERROR("get topic err,"<<ns<<","<<id<<","<<name);
		return false;
	}

	return (PB_OK == exist_topic(m_handle, m_topicbuf)) ? true : false;
}

bool hummer::CZkServer::del(const char* ns, TopicId id, const char* name)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	m_code = get_topic(m_handle, ns, id, name, m_topicbuf, tlen);
	if(PB_OK != m_code)
	{
		LOG_ERROR("get topic err,"<<ns<<","<<id<<","<<name);	
		return false;
	}

	return (PB_OK == delete_topic(m_handle, m_topicbuf)) ? true : false;
}

bool hummer::CZkServer::push(const char* ns, TopicId id, const char* name, char* data, size_t len)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	m_code = get_topic(m_handle, ns, id, name, m_topicbuf, tlen);
	if(PB_OK != m_code)
	{
		LOG_ERROR("get topic err,"<<ns<<","<<id<<","<<name);
		return false;
	}

	m_code = pub_topic(m_handle, m_topicbuf, data, len);
	if (PB_OK != m_code)
	{
		LOG_ERROR("pub topic err,"<<ns<<","<<id<<","<<name);
		m_status = pubsub_status(m_handle);
		return false;
	}

	return true;
}

bool hummer::CZkServer::pull(const char* ns, TopicId id, const char* name, char* data, size_t& len)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	if(PB_OK != get_topic(m_handle, ns, id, name, m_topicbuf, tlen))
	{
		return false;
	}

	m_code = pull_topic(m_handle, m_topicbuf, data, len);
	if (PB_OK != m_code)
	{
		m_status = pubsub_status(m_handle);
		return false;
	}
	
	return true;
}  

bool hummer::CZkServer::PullChild(const char* ns, TopicId id, const char* name, set<string>& childs)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	if(PB_OK != get_topic(m_handle, ns, id, name, m_topicbuf, tlen))
	{
		return false;
	}

	m_code = pull_subtopic(m_handle, m_topicbuf, childs);
	if (PB_OK != m_code)
	{
		m_status = pubsub_status(m_handle);
		return false;
	}
	
	return true;
}

int hummer::CZkServer::RegisterZK(int status, void*arg)
{
	CZkServer* server = (CZkServer*)arg;
	if (NULL == server 
		|| 0 == server->m_registers.size())
	{
		return -1;
	}

	server->m_status = (PubsubStatus)status;
	list<CZkRegister*>::iterator iter = server->m_registers.begin();
	for (; iter != server->m_registers.end(); ++iter)
	{
		if (NULL == *iter)
		{
			continue;
		}

		if (status == PUBSUB_STATUS_OK)
		{
			(*iter)->DoConnected();
		}
		else
		{
			(*iter)->DoUnconnected();
		}
	}

	if (server->m_actors.size() > 0 
		&& status == PUBSUB_STATUS_OK)
	{
		list<CZkActor*>::iterator iter = server->m_actors.begin();
		for (; iter != server->m_actors.end(); ++iter)
		{
			server->DoSub(*iter);
		}
	}

	return 0;
}

bool hummer::CZkServer::Sub(CZkActor* actor)
{
	if (NULL == actor)
	{
		return false;
	}

	m_actors.push_back(actor);
	if (m_status == PUBSUB_STATUS_OK)
	{
		DoSub(actor);
	}

	return true;
}

bool hummer::CZkServer::Register(CZkRegister* reg)
{
	if (NULL == reg)
	{
		return false;
	}

	m_registers.push_back(reg);
	return true;
}

bool hummer::CZkServer::DoSub(CZkActor* actor)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	if(PB_OK != get_topic(m_handle, actor->GetNs().c_str(), actor->GetTopic(), actor->GetName().c_str(), m_topicbuf, tlen))
	{
		return false;
	}

	if (PB_OK == sub_topic(m_handle, m_topicbuf, hummer::CZkActor::SubAct, actor))
	{
		return true;
	}
	return false;
}

bool hummer::CZkServer::CreateCounter(const char* cname, int64_t& init)
{
	if (PB_OK == create_counter(m_handle, cname, init))
	{
		return true;
	}

	return false;
}

bool hummer::CZkServer::NextCounter(const char* cname, int64_t& cvalue)
{
	if (PB_OK == get_next_count(m_handle, cname, cvalue))
	{
		return true;
	}	
	return false;
}

bool hummer::CZkServer::DelCounter(const char* cname)
{
	if (PB_OK == delete_counter(m_handle, cname))
	{
		return true;
	}
	return false;
}

const int hummer::CZkServer::TOPIC_LENGTH = 256;

/***************CZkRegister definition****************/
hummer::CZkRegister::CZkRegister()
:m_handle(NULL){}

hummer::CZkRegister::~CZkRegister(){}

hummer::CZkServer* hummer::CZkRegister::GetPB()
{
	return m_handle;
}

void hummer::CZkRegister::SetPB(CZkServer* pb)
{
	m_handle = pb;
}

void hummer::CZkRegister::SetServer(CServerBase* server)
{
	m_Server = server;
}

/****************CZkActor*****************/
hummer::CZkActor::CZkActor()
:m_id(TOPIC_INVALID_ID), m_handle(NULL){}

hummer::CZkActor::~CZkActor(){}

void hummer::CZkActor::SetServer(CServerBase* server)
{
	m_Server = server;
}

void hummer::CZkActor::SetInfo(const char* ns, TopicId id, const char* name, CZkServer* pb)
{
	m_ns = ns;
	m_id = id;
	m_name = name;
	m_handle = pb;
	return;
}

TopicId hummer::CZkActor::GetTopic()const
{
	return m_id;
}

const string& hummer::CZkActor::GetName()const
{
	return m_name;
}

const string& hummer::CZkActor::GetNs()const
{
	return m_ns;
}

hummer::CZkServer* hummer::CZkActor::GetHandle()
{
	return m_handle;
}

int hummer::CZkActor::SubAct(int type, const char* topic, char* buffer, size_t len, void* arg)
{
	CZkActor* actor = (CZkActor*)arg;
	if (NULL == actor)
	{
		return -1;
	}

	return actor->DoSubActor(type, topic, buffer, len);
}

