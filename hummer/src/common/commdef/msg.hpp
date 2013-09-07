/*****************************************************
 * Copyright zhengyang
 * FileName: msg.hpp
 * Description: message public head file
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef MSG_HPP
#define MSG_HPP

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <google/protobuf/message.h>

//the job type describer text in job xml
#define JOB_SIM_STRING "SIM"
#define JOB_MAP_STRING "MAP"

//the 
#define SPLIT_DEF_STRING "DEF"
#define SPLIT_PRE_STRING "PRE"
#define SPLIT_MAP_STRING "MAP"

//msg id
typedef int64_t MID;
//task id
typedef uint64_t TID;
//job id
typedef uint64_t JID;
//topology id
typedef uint64_t TOPOLOGYID;
//autopointer of message
typedef boost::shared_ptr<google::protobuf::Message> MsgPTR;

enum BusinessType
{
	BUSI_INVALID = -1,
	BUSI_TEST = 0,
	BUSI_SKETCH,
	BUSI_MAX
};

enum TaskType
{
	TASK_INVALID = -1,
	TASK_SIM = 0,
	TASK_SPLIT,
	TASK_MAP,
	TASK_MAX
};


enum JobType
{
	JOB_INVALID = -1,
	JOB_SIM = 0,
	JOB_MAP,
	JOB_MAX
};

typedef struct msg_stat_s
{
	uint64_t uMsgTotal;
	uint64_t uMsgErr;
	uint64_t uMsgOk;
}msg_stat_t;

#endif 

