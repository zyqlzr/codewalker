#ifndef CONF_SERVER_PB_H
#define CONF_SERVER_PB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pubsub.hpp"

class CPBConfigure
{
public:
	CPBConfigure();
	~CPBConfigure();
	bool Init(const char* addr);
	void Unit();
	PubsubStatus Status()const;
	PubsubErrCode Code()const;
	static int RegisterZK(int status, void* arg);
	bool Register(); 
	bool pub(const char* ns, const char* xml, char* data, size_t len);
	bool push(const char* ns, const char* xml, char* data, size_t len);
	bool pull(const char* ns, const char* xml, char* data, size_t& len);
	
	bool pubJob(const char* ns, const char* xml, char* data, size_t len);
	bool pushJob(const char* ns, const char* xml, char* data, size_t len);
	bool pullJob(const char* ns, const char* xml, char* data, size_t& len);
	bool DelJob(const char* ns, const char* xml);
private:
	static const TopicId CONFIG_TOPIC_ID;
	static const int TOPIC_LENGTH;
private:
	pbHandle m_handle;
	PubsubStatus m_status;
	int m_code;
	char* m_topicbuf;
	string m_ConfigTopic;
};

#endif

