#include "test.hpp"
#include "Logger.h"
#include "utildef.hpp"
#include "topic_def.hpp"
#include "zkserver.hpp"

#include <stdio.h>
#include <string.h>

typedef struct zk_core_s
{
	const char* host;
}zk_core_t;

class CTestRegister : public hummer::CZkRegister
{
public: 
	CTestRegister(){}
	~CTestRegister(){}
	void DoUnconnected()
	{
		printf("CTestRegister conn\n");
	}

	void DoConnected()
	{
		printf("CTestRegister unconn\n");
	}
};

class CTestActor : public hummer::CZkActor
{
public:
	CTestActor(){}
	~CTestActor(){}
	int DoSubActor(int type, const char* topic, char* buffer, size_t len)
	{
		printf("Listen: type=%d,topic=%s,data=%s\n"
			, type, topic, buffer);
		return 0;
	}
};

int TestZkActor(void* arg)
{
	zk_core_t* parm = (zk_core_t*)arg;
	if (!parm)
	{
		return -1;
	}

	hummer::CZkServer writer;
	hummer::CZkServer listener;
	CTestRegister testR;
	CTestActor testA;
	
	const char* test_path = "test_node";
	testR.SetPB(&listener);
	
	testA.SetInfo(HUMMER_NS_STRING, TOPIC_TASK, test_path, &listener);
	if (!listener.Sub(&testA)
		|| !listener.Register(&testR))
	{
		HUMMER_ASSERT(false);
	}
	
	if (!listener.Init(parm->host)
		|| !writer.Init(parm->host))
	{
		HUMMER_ASSERT(false);
	}

	char buf[64] = {0};
	sprintf(buf, "sksksksksksks");

	sleep(1);
	if (!writer.push(HUMMER_NS_STRING, TOPIC_TASK, test_path, buf, 64))
	{
		HUMMER_ASSERT(false);
	}

	sleep(1);
	if (!writer.del(HUMMER_NS_STRING, TOPIC_TASK, test_path))
	{
		HUMMER_ASSERT(false);
	}

	sleep(1);
	if (!writer.push(HUMMER_NS_STRING, TOPIC_TASK, test_path, buf, 64))
	{
		HUMMER_ASSERT(false);
	}

	sprintf(buf, "totototottototo");
	sleep(1);
	if (!writer.push(HUMMER_NS_STRING, TOPIC_TASK, test_path, buf, 64))
	{
		HUMMER_ASSERT(false);
	}
	sleep(10);

	writer.Unit();
	listener.Unit();
	return 0;
}

const char* ZOO_IP="192.168.0.104:2181";

zk_core_t core_parm = 
{
	ZOO_IP
};

test_instance_t zkcore_test = 
{
	"pubsub_api_test", 
	100, 
	0, 
	{
		{1, TestZkActor, (void*)(&core_parm), "core parm"},
		{END_TASK, NULL, NULL, NULL}
	}
};


