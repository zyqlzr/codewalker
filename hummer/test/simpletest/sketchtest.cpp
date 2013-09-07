#include "test.hpp"
#include "utildef.hpp"
#include "Logger.h"
#include <vector>
#include <string>
#include <set>
#include "sketchtask.hpp"
#include "sketchprocessor.hpp"
#include "sketchstat.hpp"
#include "timedwatcher.hpp"

using std::vector;
using std::string;
using std::set;

typedef struct sketch_test_s
{
	const char* productTable;
	const char* host;
	int port;
	const char* fds;
	const char* redisHost;
	int redisPort;
}sketch_test_t;

const char* redisHost_ = "192.168.3.161";
int redisPort_ = 6378;
const char* fastdfs = "./fastdfs.cfg";
const char* sketch_product_table_ = "himport_product_label";

int sketchTest(void* arg)
{
	sketch_test_t* parm = (sketch_test_t*)arg;
	if (!parm)
	{
		HUMMER_ASSERT(false);
	}

	string fdfs = parm->fds;
	string hbaseHost = parm->host;
	int hbasePort = parm->port;
	string redisHost = parm->redisHost;
	int redisPort = parm->redisPort;

	string productTable = parm->productTable;

	//hummer::CSketchSimpleWatcher watcher;	
	hummer::CTimedWatcher watcher;
	if (!watcher.Start(5, 1))
	{
		HUMMER_ASSERT(false);
	}

	hummer::CProductHbaseSpout spout;
	hummer::CalcProcessor calc;
	hummer::CRedisProcessor redis;

	if (!spout.Test(&calc, &watcher
			, productTable, hbaseHost, hbasePort, 1000))
	{
		HUMMER_ASSERT(false);
	}
	printf("fastdfs addr=%s\n", fdfs.c_str());
	if (!calc.Test(&watcher, &redis, 5, fdfs.c_str()))
	{
		HUMMER_ASSERT(false);
	}

	if (!redis.Test(&watcher, redisHost.c_str()
			, redisPort, 0, 1, 2))
	{
		HUMMER_ASSERT(false);
	}

	if (!sketchStat.Start())
	{
		HUMMER_ASSERT(false);
	}

	if (!spout.GetData())
	{
		HUMMER_ASSERT(false);
	}

	//for (int i = 0; i < 3; i++)
	//{
	sleep(10);	
	//}
	spout.Close();
	LOG_TRACE("spout close");	
	calc.Close();
	LOG_TRACE("calc close");
	redis.Close();
	LOG_TRACE("redis close");

	msg_stat_t stat;
	watcher.Info(stat);
	printf("Static, total=%lu,ack=%lu, failed=%lu\n"
		, stat.uMsgTotal, stat.uMsgOk, stat.uMsgErr);
	printf("watch redo num = %d\n", watcher.RedoNum());
	watcher.Stop();
	return 0;
}

sketch_test_t sketch_parm = 
{
	sketch_product_table_
	, "10.0.0.179", 9090
	, fastdfs, redisHost_, redisPort_
};

test_instance_t hbase_sketch_test = 
{
	"hbase_sketch_test", 
	100, 
	0, 
	{
		{1, sketchTest, (void*)(&sketch_parm), "hbase sketch test"},
		{END_TASK, NULL, NULL, NULL}
	}
};


