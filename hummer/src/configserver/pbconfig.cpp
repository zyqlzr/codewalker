#include "pbconfig.hpp"

#include "Logger.h"
#include "pbdef.hpp"

extern const char* LOG_CONFCHECK_HEAD;

CPBConfigure::CPBConfigure(): m_handle(NULL), m_status(PUBSUB_STATUS_UNINIT), m_code(PB_OK), m_topicbuf(NULL){}

CPBConfigure::~CPBConfigure(){}

bool CPBConfigure::Init(const char* addr)
{
	m_topicbuf = new(std::nothrow) char[TOPIC_LENGTH];
	if (NULL == m_topicbuf)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"malloc failed");
		return false;
	}

	m_handle = pubsub_init(PUBSUB_ZOO, addr, CPBConfigure::RegisterZK, (void*)this);
	if (NULL == m_handle)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"Init pubsub failed,addr="<<addr);
		delete [] m_topicbuf;
		m_topicbuf = NULL;
		return false;
	}

	memset(m_topicbuf, 0, TOPIC_LENGTH);
	m_status = pubsub_status(m_handle);
	if (PUBSUB_STATUS_OK == m_status)
	{
		Register();
	}
	return true;
}

void CPBConfigure::Unit()
{
	pubsub_uninit(m_handle);
	m_status = PUBSUB_STATUS_UNINIT;
	if (NULL != m_topicbuf)
	{
		delete [] m_topicbuf;
		m_topicbuf = NULL;
	}
}

PubsubStatus CPBConfigure::Status()const
{
	return m_status;
}

PubsubErrCode CPBConfigure::Code()const
{
	return (PubsubErrCode)m_code;
} 

bool CPBConfigure::pub(const char* ns, const char* xml, char* data, size_t len)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	m_code = get_topic(m_handle, ns, CONFIG_TOPIC_ID, xml, m_topicbuf, tlen);
	if(PB_OK != m_code)
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"get topic failed,"<<xml);
		return false;
	}
	
	m_code = pub_topic(m_handle, m_topicbuf, data, len);
	if (PB_OK != m_code)
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"pub topic failed,"<<xml<<","<<m_code);
		m_status = pubsub_status(m_handle);
		return false;
	}
	
	return true;
}

bool CPBConfigure::push(const char* ns, const char* xml, char* data, size_t len)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	m_code = get_topic(m_handle, ns, CONFIG_TOPIC_ID, xml, m_topicbuf, tlen);
	if(PB_OK != m_code)
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"Get topic failed,"<<xml<<","<<m_code);
		return false;
	}
	
	m_code = pub_topic(m_handle, m_topicbuf, data, len);
	if (PB_OK != m_code)
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"Pub topic failed,"<<xml<<","<<m_code);
		m_status = pubsub_status(m_handle);
		return false;
	}

	return true;
}

bool CPBConfigure::pull(const char* ns, const char* xml, char* data, size_t& len)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	if(PB_OK != get_topic(m_handle, ns, CONFIG_TOPIC_ID, xml, m_topicbuf, tlen))
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"Get topic failed,"<<xml);
		return false;
	}

	m_code = pub_topic(m_handle, m_topicbuf, data, len);
	if (PB_OK != m_code)
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"Pub topic failed,"<<xml<<","<<m_code);
		m_status = pubsub_status(m_handle);
		return false;
	}
	
	return true;
}  

bool CPBConfigure::pubJob(const char* ns, const char* xml, char* data, size_t len)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	m_code = get_topic(m_handle, ns, TOPIC_SUBMIT_JOB, xml, m_topicbuf, tlen);
	if(PB_OK != m_code)
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"Get job topic failed,"<<xml);
		return false;
	}

	if (exist_topic(m_handle, m_topicbuf))
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"The job is already exist,"<<xml);
		return false;
	}

	m_code = pub_topic(m_handle, m_topicbuf, data, len);
	if (PB_OK != m_code)
	{
		m_status = pubsub_status(m_handle);
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"Pub job failed,"<<xml<<","<<m_status);
		return false;
	}

	return true;
}

bool CPBConfigure::pushJob(const char* ns, const char* xml, char* data, size_t len)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	m_code = get_topic(m_handle, ns, TOPIC_SUBMIT_JOB, xml, m_topicbuf, tlen);
	if(PB_OK != m_code)
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"Get job topic failed,"<<xml);
		return false;
	}
	
	if (PB_OK == exist_topic(m_handle, m_topicbuf))
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"The job is already exist,"<<m_topicbuf);
		return false;
	}

	m_code = pub_topic(m_handle, m_topicbuf, data, len);
	if (PB_OK != m_code)
	{
		m_status = pubsub_status(m_handle);
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"The job is pub failed"<<xml<<","<<m_code);
		return false;
	}

	return true;
}

bool CPBConfigure::pullJob(const char* ns, const char* xml, char* data, size_t& len)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	if(PB_OK != get_topic(m_handle, ns, TOPIC_SUBMIT_JOB
		, xml, m_topicbuf, tlen))
	{
		return false;
	}

	m_code = pub_topic(m_handle, m_topicbuf, data, len);
	if (PB_OK != m_code)
	{
		m_status = pubsub_status(m_handle);
		return false;
	}
	
	return true;
}

bool CPBConfigure::DelJob(const char* ns, const char* xml)
{
	size_t tlen = TOPIC_LENGTH;
	memset(m_topicbuf, 0, TOPIC_LENGTH);

	if(PB_OK != get_topic(m_handle, ns, TOPIC_SUBMIT_JOB
		, xml, m_topicbuf, tlen))
	{
		return false;
	}

	m_code = delete_topic(m_handle, m_topicbuf);
	if (PB_OK != m_code)
	{
		m_status = pubsub_status(m_handle);
		return false;
	}
	
	return true;
}

bool CPBConfigure::Register()
{
	if (m_ConfigTopic.length() == 0)
	{
		size_t tlen = TOPIC_LENGTH;
		memset(m_topicbuf, 0, TOPIC_LENGTH);
	
		if (PB_OK == get_topic(m_handle, CONFIG_NS_STRING, TOPIC_CONFIG_SERVER, "ConfigureServer", m_topicbuf, tlen))
		{
			m_ConfigTopic = m_topicbuf;
		}
		else
		{
			printf("check register failed\n");
			return false;
		}
	}

	if (PB_OK == pub_topic(m_handle, m_ConfigTopic.c_str(), NULL, 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

int CPBConfigure::RegisterZK(int status, void* arg)
{
	CPBConfigure* pbconf = (CPBConfigure*)arg;
	if (NULL == pbconf)
	{
		return -1;
	}

	if (PUBSUB_STATUS_OK == status
		&& pbconf->Register())
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"configserver register ok");
	}
	else 
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"pubsub unconnected or register failed");
	}
}

const TopicId CPBConfigure::CONFIG_TOPIC_ID = TOPIC_CONF;

const int CPBConfigure::TOPIC_LENGTH = 256;

