/*****************************************************
 * Copyright zhengyang   
 * FileName: pbconf.hpp 
 * Description: the configure of pubsub
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_CONF_H
#define PUBSUB_CONF_H

#include "zoowatcher.hpp"
#include "listener.hpp"
#include "handler.hpp"
#include "topic.hpp"
#include "topic_def.hpp"

#include "nodedes.hpp"

#include <list>
#include <string>
using std::list;
using std::string;

namespace pubsub
{

enum SubPattern
{
	SUB_INVALID = -1,
	SUB_SINGLE = 0,
	SUB_ALL,
	SUB_UNABLE,
};

enum PubPattern
{
	PUB_INVALID = -1,
	PUB_ABLE = 0,
	PUB_UNABLE
};

typedef int32_t NODE_PATTERN;

class CPubSubConf
{
public:
	CPubSubConf();
	~CPubSubConf();
	static const int HUMMER_NS;
	static const int CONFIG_NS;
	static const int COUNTER_NS;
	static const int TEST_NS;

	void Init(const char* confpath);	
	bool LoadConf(const char* confpath);
	void LoadDefault();
	bool GetNSRoot(int nstype, CDirDef& dir);
	bool GetNSDirs(int nstype, list<CDirDef>& ns);
	bool GetNSList(list<int>& nslist);
	size_t GetNsTopicNum(int nstype);
	
	int GetTopic(const char* ns, TopicId appid, const char* name, string& topic);
private: 
	int GetHummerTopic(TopicId appid, const char* name, string& topic);
	int GetTestTopic(TopicId appid, const char* name, string& topic);
	int GetCounterTopic(TopicId appid, const char* name, string& topic);
	int GetConfigTopic(TopicId appid, const char* name, string& topic);
private:
	list<CDirDef> m_HummerNs;
	list<CDirDef> m_CounterNs;
	list<CDirDef> m_TestNs;
	list<CDirDef> m_ConfNs;
private:
	static const dir_def_t HUMMER_ROOT;

	static const dir_def_t COUNTER_ROOT;
	static const dir_def_t COUNTER_LOCK;
	static const dir_def_t COUNTER_VALUE;

	static const dir_def_t TEST_ROOT;
	static const dir_def_t TEST_NOR_DIR;
	static const dir_def_t TEST_TMP_DIR;
	static const dir_def_t TEST_SEQ_DIR;
	static const dir_def_t TEST_SEQTMP_DIR;
public:
	static CPubSubConf& GetInstance();
};

}

#endif
