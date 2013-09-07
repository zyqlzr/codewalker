#include "test.hpp"
#include "cluster.hpp"
#include "utildef.hpp"
#include "zkactor.hpp"

int ClusterApiTest1(void* parm)
{
	hummer::CIdGenerator generator;
	generator.JustTest();

	hummer::Cluster testcluter;
	if (!testcluter.Init(hummer::SCHEDULE_SIM, &generator))
	{
		HUMMER_ASSERT(false);
	}

	testcluter.UnInit();
	return 0;
}

typedef struct test_server_s
{
	int servernum;
}test_server_t;

int ClusterServerTest(void* arg)
{
	test_server_t* server = (test_server_t*)arg;
	if (!server)
	{
		return -1;
	}

	set<ServerPTR> servers;
	for (int i = 0; i < server->servernum; ++i)
	{
		char serverName[64] = {0};
		sprintf(serverName,"server-%d",i);
		set<BusinessType> busiSet;
		busiSet.insert(BUSI_TEST);
		busiSet.insert(BUSI_SKETCH);

		ServerPTR serverptr(new(std::nothrow) hummer::CServer());
		serverptr->SetServerDes(serverName);
		serverptr->SetCpuCore(8);
		serverptr->SetBusiness(busiSet);
		serverptr->SetPortRange(10000, 9000);
		servers.insert(serverptr);
	}

	hummer::CIdGenerator generator;
	generator.JustTest();

	hummer::Cluster testcluter;
	if (!testcluter.Init(hummer::SCHEDULE_SIM, &generator)
		|| !testcluter.ServerRecover(servers))
	{
		HUMMER_ASSERT(false);
	}
	testcluter.Print();
	testcluter.UnInit();
	return 0;
}

test_server_t serverParm = 
{
	10
};

typedef struct test_schedule_s
{
	int servernum;
	int jobnum;
}test_schedule_t;

int ClusterScheduleTest(void* arg)
{
	test_schedule_t* parm = (test_schedule_t*)arg;
	if (!parm)
	{
		return -1;
	}

	set<ServerPTR> servers;
	for (int i = 0; i < parm->servernum; ++i)
	{
		char serverName[64] = {0};
		sprintf(serverName,"server-%d",i);
		set<BusinessType> busiSet;
		busiSet.insert(BUSI_TEST);
		busiSet.insert(BUSI_SKETCH);

		ServerPTR serverptr(new(std::nothrow) hummer::CServer());
		serverptr->SetServerDes(serverName);
		serverptr->SetCpuCore(8);
		serverptr->SetBusiness(busiSet);
		serverptr->SetPortRange(10000, 9000);
		servers.insert(serverptr);
	}

	list<JobPTR> jobs;
	for (int j = 0; j < parm->jobnum; ++j)
	{
		char jobdes[64] = {0};
		sprintf(jobdes, "job-%d", j);
		set<SplitPTR> splits;
		set<string> srcs;
		set<string> dsts;
		for (int cpu = 0; cpu < 2; ++cpu)
		{
			char id[64] = {0};
			sprintf(id, "%d", cpu);
			string text = "text";
			SplitPTR splitptr(new(std::nothrow) hummer::CSplit());
			splitptr->Init(id, text);
			splits.insert(splitptr);
		}
		JobPTR jobptr(new(std::nothrow) hummer::CJob());
		jobptr->Submit((JID)j, JOB_SIM, BUSI_TEST, jobdes
			, splits, srcs, dsts, 1);
		jobptr->Recover();
		jobs.push_back(jobptr);
	}

	hummer::CIdGenerator generator;
	generator.JustTest();

	hummer::Cluster testcluter;
	if (!testcluter.Init(hummer::SCHEDULE_SIM, &generator)
		|| !testcluter.ServerRecover(servers))
	{
		HUMMER_ASSERT(false);
	}
	testcluter.Print();

	list<JobPTR>::iterator jIter = jobs.begin();
	for (; jIter != jobs.end(); ++jIter)
	{
		set<TaskPTR> tasks;
		if (!testcluter.Schedule(*jIter, tasks))
		{
			printf("schdue task failed\n");
			testcluter.Rollback(*jIter, tasks);
		}
		else
		{
			printf("schedule task ok, num=%d\n", tasks.size());
			testcluter.Commit(*jIter, tasks);
		}

		(*jIter)->Print();
	}

	testcluter.Print();
	//task hb
	//server dead
	size_t count = 0;
	size_t serverDeadNum = servers.size() / 2;
	printf("server dead:count=%d, num=%d\n", count, serverDeadNum);
	set<ServerPTR>::iterator serverIter = servers.begin();
	for (; serverIter != servers.end(); ++serverIter)
	{
		++count;
		if (count == serverDeadNum)
		{
			break;
		}
		printf("dead server:%s\n", (*serverIter)->GetServerDes().c_str());
		testcluter.ServerDead((*serverIter)->GetServerDes());
	}
	testcluter.Print();

	sleep(40);
	set<TaskPTR> timeoutTasks;
	testcluter.TimeOutCheck(timeoutTasks);
	testcluter.Print();	
	testcluter.UnInit();
	return 0;
}

test_schedule_t schedule_parm = 
{
	10,
	10
};

test_instance_t cluster_test = 
{
	"cluster_api_test", 
	100, 
	0, 
	{
		{0, ClusterApiTest1, NULL, "cluster api test1"},
		{0, ClusterServerTest, (void*)(&serverParm), "cluster server test"},
		{1, ClusterScheduleTest, (void*)(&schedule_parm), "cluster schedule test"},
		{END_TASK, NULL, NULL, NULL}
	}
};


