#ifndef PUBSUB_H
#define PUBSUB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "topic_def.hpp"

#include <string>
#include <set>
using std::string;
using std::set;

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_PBHandle NULL

enum PubsubStatus
{
	PUBSUB_STATUS_UNINIT = -1,
	PUBSUB_STATUS_OK = 0,
	PUBSUB_STATUS_NETERR,
	PUBSUB_STATUS_ERR
};

enum PubsubErrCode
{
	PB_OK = 0,
	PB_SYSERR = -1,
	PB_NETERR = -2,
	PB_TOPICFORBID = -3,
	PB_TOPICINVALID = -4,
	PB_NOTOPIC = -5,
	PB_TOPICEXIST = -6,
	PB_WRITECONFLICT = -7,
	PB_DATAOVER = -8
};

enum NotifyType
{
	NT_INVALID = -1,
	NT_TOPIC_CREATE = 0,
	NT_TOPIC_DEL,
	NT_TOPIC_UPDATE,
	NT_TOPIC_CHILD,
	NT_TOPIC_CHILD_ADD,
	NT_TOPIC_CHILD_DEL,
	NT_TOPIC_CHILD_CHANGE,
	NT_MAX,
};

enum PubSubType
{
	PUBSUB_INVALID = -1,
	PUBSUB_ZOO = 0,
	PUBSUB_MAX,
};

typedef void* pbHandle;

typedef int (*StatusNotify)(int status, void* arg);
typedef int (*DataSubNotify)(int type, const char* topic, char* data, size_t len, void* arg);

/*brief initialize the pubsub session, it will blocked until the session is connected
 * or connected failed 
 * \parm type, now only support PUBSUB_ZOO, it means pubsub based on the zookeepe
 * \parm add, the network address
 * \parm statusCB, the asynchronous callback function used to notify the status of
 * the pubsub session, it will not notify the user while session first connected.
 * \parm arg, the parameter of StatusNotify
 * \return, if ok,return session handle,else return NULL */
pbHandle pubsub_init(PubSubType type, const char* addr, StatusNotify statusCB, void* arg);
PubsubStatus pubsub_status(pbHandle pbh);

int create_topic(pbHandle pbh, const char* topic, char* data, size_t len);
int delete_topic(pbHandle pbh, const char* topic);
int exist_topic(pbHandle pbh, const char* topic);
int get_topic(pbHandle pbh, const char* ns, TopicId appid, const char*topic_name, char* buf, size_t& len);

int pub_topic(pbHandle pbh, const char* topic, char* data, size_t len);
int sub_topic(pbHandle pbh, const char* topic, DataSubNotify notifycb, void* arg);
int pubsub_topic(pbHandle pbh, const char* topic, char* data, size_t len, DataSubNotify notifycb, void* arg);
int unsub_topic(pbHandle pbh, const char* topic);

int push_topic(pbHandle pbh, const char* topic, char* data, size_t len);
int pull_topic(pbHandle pbh, const char* topic, char* data, size_t& len);
int pull_subtopic(pbHandle pbh, const char* topic, set<string>& subtopics);

int create_counter(pbHandle pbh, const char* counterName, int64_t& initcount); 
int exist_counter(pbHandle pbh, const char* counterName);
int get_next_count(pbHandle pbh, const char* counterName, int64_t& count);
int get_current_count(pbHandle pbh, const char* counterName, int64_t& count);
int delete_counter(pbHandle pbh, const char* counterName);

void pubsub_uninit(pbHandle pbh);

#ifdef __cplusplus
}
#endif

#endif

