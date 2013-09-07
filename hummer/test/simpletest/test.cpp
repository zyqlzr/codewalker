#include "test.hpp"
#include <map>
#include <list>
using std::map;
using std::list;

#include "Logger.h"

//LOG_FORMAT "P():M():arrname[name1=value1, name2=value2, ....]:name(value)"
#define TEST_LOG_HEAD "P(PUBSUB):M(TEST)"

class CInstanceStat
{
public:
	CInstanceStat():m_iErr_(0), m_iCorr_(0), m_iExecNum_(0), m_iTotal_(0){}
	~CInstanceStat()
	{
		m_iErr_ = 0;
		m_iCorr_ = 0;
		m_iExecNum_ = 0;
		m_iTotal_ = 0;
	}
	
	void Err(test_task_t* task)
	{
		if (NULL == task)
		{
			return;
		}
		m_iErr_++;
		m_iExecNum_++;
		m_ErrsInfo.push_back(task->sTaskName);
		return;
	}
	
	void Corr(test_task_t* task)
	{
		if (NULL == task)
		{
			return;
		}
		m_iCorr_++;
		m_iExecNum_++;
		return;
	}
	
	void Total(int num)
	{
		m_iTotal_ = num;
		return;
	}

	int GetErr()const
	{
		return m_iErr_;
	}
	
	int GetCorr()const
	{
		return m_iCorr_;
	}
	
	int GetExecNum()const
	{
		return m_iExecNum_;
	}
	
	void Print(string instanceName)
	{
		//LOG_TRACE("%s:instance(%s):stat[err=%d,corr=%d,execnum=%d,total=%d]"
		//	, TEST_LOG_HEAD, instanceName.c_str(), m_iErr_, m_iCorr_, m_iExecNum_, m_iTotal_);
		LOG_TRACE(TEST_LOG_HEAD<<" instance="<<instanceName<<" err="<<m_iErr_<<" corr="<<m_iCorr_<<" exec="<<m_iExecNum_<<" total="<<m_iTotal_);
		list<string>::iterator iter = m_ErrsInfo.begin();
		for (; iter != m_ErrsInfo.end(); iter++)
		{
			//LOG_TRACE("%s:instance(%s):ErrTask(%s)", TEST_LOG_HEAD, instanceName.c_str(), (*iter).c_str());
			LOG_TRACE(TEST_LOG_HEAD<<" instance="<<instanceName<<" errtask="<<(*iter));
		}
	}
private:
	int m_iErr_;
	int m_iCorr_;
	int m_iExecNum_;
	int m_iTotal_;
	list<string> m_ErrsInfo;
};

class CTestManage
{
public:
	typedef map<string, test_instance_t*> Instances;
	typedef map<string, CInstanceStat> InstanceStat;
	CTestManage(): m_TotalNum(0), m_TotalExec(0), m_TotalErrStat(0), m_TotalCorrStat(0){}
	~CTestManage(){}
	void RegisterInstance(test_instance_t* instance);
	void UnRegisterInstance(test_instance_t* instance);
	void Exec();
	void CurrentExec();

	void Statistic();

private:
	void ExecPerInstance(test_instance_t* instance, CInstanceStat& stat);
private:
	int m_TotalNum;
	int m_TotalExec;
	int m_TotalErrStat;
	int m_TotalCorrStat;
	Instances m_Instances;
	InstanceStat m_Stats;
};

void CTestManage::ExecPerInstance(test_instance_t* instance, CInstanceStat& stat)
{
	if (NULL == instance)
	{
		return;
	}

	test_task_t* taskArray = instance->aTestTask;
	for (int i = 0; i < instance->iTestNum; i++)
	{
		if (INSTANCE_EXEC_PER == instance->iAllExecFlag 
			&& SKIP_TASK == taskArray[i].uExecFlag)
		{
			continue;
		}

		if (END_TASK == taskArray[i].uExecFlag)
		{
			break;
		}

		if (INSTANCE_EXEC_ALL == instance->iAllExecFlag 
			|| EXEC_TASK == taskArray[i].uExecFlag)
		{
			if (TEST_OK == (*(taskArray[i].fCb))(taskArray[i].pArg))
			{
				stat.Corr(&taskArray[i]);
			}
			else if (TEST_ERR == (*(taskArray[i].fCb))(taskArray[i].pArg))
			{
				stat.Err(&taskArray[i]);
			}
			else
			{
				
			}
		}
		
	}
}

void CTestManage::RegisterInstance(test_instance_t* instance)
{
	if (NULL == instance)
	{
		return;
	}

	string instanceName = instance->name;
	Instances::iterator iter = m_Instances.find(instanceName);
	if (iter == m_Instances.end())
	{
		m_Instances.insert(Instances::value_type(instanceName, instance));
	}
	else
	{
		iter->second = instance;
	}
	
	return;
}

void CTestManage::UnRegisterInstance(test_instance_t* instance)
{
	if (NULL == instance)
	{
		return;
	}

	return;
}

void CTestManage::Exec()
{
	VSLogger::Init("log4cxx.properties", "test");
	Instances::iterator instanceIter = m_Instances.begin();
	for (; instanceIter != m_Instances.end(); instanceIter++)
	{
		CInstanceStat stat;
		ExecPerInstance(instanceIter->second, stat);
		m_TotalNum += instanceIter->second->iTestNum;
		m_TotalExec += stat.GetExecNum();
		m_TotalErrStat += stat.GetErr();
		m_TotalCorrStat += stat.GetCorr();
		stat.Print(instanceIter->second->name);
	}
}

void CTestManage::Statistic()
{
	//LOG_TRACE("%s:totalstat[err=%d,corr=%d,execnum=%d,total=%d]"
	//	, TEST_LOG_HEAD, m_TotalErrStat, m_TotalCorrStat, m_TotalExec, m_TotalNum);
	LOG_TRACE(TEST_LOG_HEAD<<" err="<<m_TotalErrStat<<" corr="<<m_TotalCorrStat<<" num="<<m_TotalExec<<" total="<<m_TotalNum);
	return;
}

CTestManage TestManager;

void RegisterInstance(test_instance_t* instance)
{
	TestManager.RegisterInstance(instance);
	return;
}

void UnRegisterInstance(test_instance_t* instance)
{
	TestManager.UnRegisterInstance(instance);
	return;
}

void Exec()
{
	TestManager.Exec();
	TestManager.Statistic();
	return;
}


