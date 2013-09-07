/*****************************************************
 * Copyright zhengyang   
 * FileName: testtask.hpp 
 * Description: the test business
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef TEST_TASK_HPP
#define TEST_TASK_HPP

#include "tasker.hpp"
#include "thread.hpp"
#include "processor.hpp"
#include "mutex.hpp"

#include <boost/atomic.hpp>

namespace hummer
{

class CTestSpout;
class CTestProcessor;

class CTestSpout : public CSpout
{
public:
	CTestSpout();
	virtual ~CTestSpout();
	CTasker::TaskerStatus Status();
	bool Bind(size_t id, CProcessor* processor);
	bool Open(TaskPTR& task, CMsgWatcher* watcher, CHummerCtx& ctx);
	void Close();
	bool GetData();
	bool ReEmit(MsgPTR& msg);
private:
	static void* EmitRoutine(void* arg);
private:
	CMsgWatcher* m_watcher;
	CProcessor* m_Processor;
	int m_msgStart;
	int m_msgEnd;
	CThread m_EmitThread;
	bool m_ExitFlag;
};

class CTestProcessor : public CProcessor
{
public:
	CTestProcessor();
	~CTestProcessor();
	bool Bind(size_t id, CProcessor* processor);
	bool Prepare(TaskPTR& task, CMsgWatcher* watcher, CHummerCtx& ctx);
	void Close(); 
	void ProcessMsg(MsgPTR& msg);
private:
	CMsgWatcher* m_watcher;
	string m_Path;
	FILE* m_File;
	CMutex m_Lock;
};

class CTestTasker : public CTasker, public CMsgWatcher
{
public:
	CTestTasker();
	~CTestTasker();
	TaskerStatus Status();
	void Statistic(msg_stat_t& stat); 
	void GetInfo(CTaskerInfo& info);
	bool Run(TaskPTR& task, CHummerCtx& ctx);
	void Stop();

	void Info(msg_stat_t& stat);
	void Watch(MID id, MsgPTR& msg);
	void Ack(MID id);
	void Fail(MID id);
	void GetRedoMsg(list<MsgPTR>& msgs, size_t num);
	size_t RedoNum();
private:
	void TotalInc();
	void AckInc();
	void FailedInc();
	uint32_t GetTotal();
	uint32_t GetAck();
	uint32_t GetFailed();	
private:
	boost::atomic_uint32_t m_TotalCount;
	boost::atomic_uint32_t m_AckCount;
	boost::atomic_uint32_t m_FailedCount;
	map<MID, MsgPTR> m_Msgs;
	CTestSpout m_Spout;
	CTestProcessor m_Processor;
	int m_splitnum;
};

class CTestSplitTasker : public CTasker
{
public:
	CTestSplitTasker();
	~CTestSplitTasker();

	CTasker::TaskerStatus Status();
	void GetInfo(CTaskerInfo& info); 
	bool Run(TaskPTR& task, CHummerCtx& ctx);
	void Stop();
private:
	CMutex m_Lock;
	CTasker::TaskerStatus m_Status;
	CTaskerInfo m_Result;
	int m_TotalNum;
	int m_SplitNum;
};

};

#endif


