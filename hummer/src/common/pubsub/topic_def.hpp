/*****************************************************
 * Copyright zhengyang   
 * FileName: topic_def.hpp 
 * Description: the definition of topic
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_TOPIC_DEF_H
#define PUBSUB_TOPIC_DEF_H

enum ZOO_CONSTANT
{
	MAX_ZOO_DATA_LEN = 1024 * 1024,
	MAX_TOPIC_LEN = 256 
};

enum TopicId
{
	TOPIC_INVALID_ID = -1,
	//public topic
	TOPIC_ROOT,
	TOPIC_CONF,
	TOPIC_CONFIG_SERVER,
	//mcs namespace support
	TOPIC_WORKER,
	TOPIC_TASK,
	TOPIC_TASK_HEARTBEAT,
	TOPIC_SUBMIT_JOB,
	TOPIC_COMMIT_JOB,
	TOPIC_MASTER,
	//counter namespace support
	TOPIC_COUNTER,
	//test namespace support
	TOPIC_TEST_NOR,
	TOPIC_TEST_TMP,
	TOPIC_TEST_SEQ,
	TOPIC_TEST_SEQTMP,
	TOPIC_MAX
};

extern const char* HUMMER_NS_STRING;
extern const char* COUNTER_NS_STRING;
extern const char* TEST_NS_STRING;
extern const char* CONFIG_NS_STRING;

extern const char* TEST_NS_TOPIC;
extern const char* TEST_NOR_TOPIC;
extern const char* TEST_TMP_TOPIC;
extern const char* TEST_SEQ_TOPIC;
extern const char* TEST_SEQTMP_TOPIC; 

#endif

