#include "testtask.hpp"
#include "Logger.h"
#include "unistd.h"
#include "testcounter.pb.h"
#include "utildef.hpp"

hummer::CTestTasker::CTestTasker()
: m_TotalCount(0), m_AckCount(0), m_FailedCount(0), m_splitnum(0){}

hummer::CTestTasker::~CTestTasker(){}

hummer::CTasker::TaskerStatus hummer::CTestTasker::Status()
{
	if (GetTotal() == GetAck())
	{
		return TASKER_FINI;
	}
	else if (GetFailed() > 0)
	{
		return TASKER_ERR;
	}
	else
	{
		return TASKER_RUNNING;
	}
}

void hummer::CTestTasker::GetRedoMsg(list<MsgPTR>& msgs, size_t num)
{
	HUMMER_NOUSE(msgs);
	HUMMER_NOUSE(num);
	return;
}

size_t hummer::CTestTasker::RedoNum()
{
	return 0;
}

void hummer::CTestTasker::GetInfo(CTaskerInfo& info)
{
	msg_stat_t stat;
	Info(stat);
	info.SetStat(stat);
}

bool hummer::CTestTasker::Run(TaskPTR& task, CHummerCtx& ctx)
{
	m_Spout.Bind(1, &m_Processor);
	if (!m_Spout.Open(task, this, ctx)
		|| !m_Processor.Prepare(task, this, ctx))
	{
		return false;
	}

	string splitdata = task->GetSplit()->GetText();
	m_splitnum = atoi(splitdata.c_str());
	
	m_Spout.GetData();
	return true;
}

void hummer::CTestTasker::Stop()
{
	LOG_TRACE("TestTasker stop");
	m_Spout.Close();
	m_Processor.Close();
}

void hummer::CTestTasker::Info(msg_stat_t& stat)
{
	stat.uMsgTotal = GetTotal();
	stat.uMsgOk = GetAck();
	stat.uMsgErr = GetFailed();
}

void hummer::CTestTasker::Watch(MID id, MsgPTR& msg)
{
	TotalInc();
	m_Msgs.insert(map<MID, MsgPTR>::value_type(id, msg));
}

void hummer::CTestTasker::Ack(MID id)
{
	AckInc();
	m_Msgs.erase(id);
}

void hummer::CTestTasker::Fail(MID id)
{
	FailedInc();
	m_Msgs.erase(id);
}

void hummer::CTestTasker::TotalInc()
{
	m_TotalCount.fetch_add(1);		
}

void hummer::CTestTasker::AckInc()
{
	m_AckCount.fetch_add(1);
}

void hummer::CTestTasker::FailedInc()
{
	m_FailedCount.fetch_add(1);
}

uint32_t hummer::CTestTasker::GetTotal()
{
	return m_TotalCount.load();
}

uint32_t hummer::CTestTasker::GetAck()
{
	return m_AckCount.load();
}

uint32_t hummer::CTestTasker::GetFailed()
{
	return m_FailedCount.load();
}

/*************spout and processor*****************/
hummer::CTestSpout::CTestSpout()
: m_watcher(NULL), m_Processor(NULL)
, m_msgStart(0), m_msgEnd(0)
, m_ExitFlag(false){}

hummer::CTestSpout::~CTestSpout(){}

hummer::CTasker::TaskerStatus hummer::CTestSpout::Status()
{
	return hummer::CTasker::TASKER_RUNNING;
}

bool hummer::CTestSpout::Bind(size_t id, CProcessor* processor)
{
	HUMMER_NOUSE(id);
	m_Processor = processor;
	return true;
}

bool hummer::CTestSpout::Open(TaskPTR& task, CMsgWatcher* watcher, CHummerCtx& ctx)
{
	HUMMER_NOUSE(ctx);
	if (!watcher || !task.get())
	{
		LOG_ERROR("input is err");
		return false;
	}

	string splitdata = task->GetSplit()->GetText();
	LOG_TRACE("DATA Split:"<<splitdata);
	string::size_type pos = splitdata.find(":");
	if (pos == string::npos)
	{
		LOG_ERROR("split text is err");
		return false;
	}

	
	m_msgStart = atoi(splitdata.substr(0, pos).c_str());
	m_msgEnd = atoi(splitdata.substr(pos + 1).c_str());
	m_watcher = watcher;
	return true;
}

void hummer::CTestSpout::Close()
{
	m_ExitFlag = false;
	m_EmitThread.Stop();
}

bool hummer::CTestSpout::GetData()
{
	m_ExitFlag = true;
	return m_EmitThread.Start(hummer::CTestSpout::EmitRoutine, this);
}

bool hummer::CTestSpout::ReEmit(MsgPTR& msg)
{
	HUMMER_NOUSE(msg);
	return true;
}

void* hummer::CTestSpout::EmitRoutine(void* arg)
{
	CTestSpout* spout = (CTestSpout*)arg;
	if (!spout)
	{
		return NULL;
	}
	
	LOG_TRACE("Enter data thread");
	int emitNum = spout->m_msgStart;
	while(spout->m_ExitFlag)
	{
		usleep(40000);
		hummer::testcounter* counter = new(std::nothrow) hummer::testcounter();
		MsgPTR counterptr(counter);
		counter->set_counter(emitNum);
		spout->m_watcher->Watch(emitNum, counterptr);
		emitNum++;
		spout->m_Processor->ProcessMsg(counterptr);
		if (emitNum >= spout->m_msgEnd)
		{
			LOG_TRACE("spout emit end,from "<<spout->m_msgStart<<",to "<<spout->m_msgEnd);
			break;
		}
		else
		{
			LOG_TRACE("emit count:"<<emitNum);
		}
	}

	return NULL;
}

/**************CTestProcessor definition****************/
hummer::CTestProcessor::CTestProcessor(){}

hummer::CTestProcessor::~CTestProcessor(){}

bool hummer::CTestProcessor::Bind(size_t id, CProcessor* processor)
{
	HUMMER_NOUSE(id);
	HUMMER_NOUSE(processor);
	return true;
}

bool hummer::CTestProcessor::Prepare(TaskPTR& task, CMsgWatcher* watcher, CHummerCtx& ctx)
{
	if (!task.get())
	{
		HUMMER_ASSERT(false);
	}

	string relativePath;
	if (!ctx.GetGlobalParm(CHummerCtx::LOCAL_PATH, CHummerCtx::RELATIVE_PATH, relativePath))
	{
		HUMMER_ASSERT(false);
	}
	task->Print();
	{
		char fileBuf[128] = {0};
		uint32_t jobId = task->GetJobId();
		uint32_t taskId = task->GetTaskId();
		sprintf(fileBuf, "%s-%lu-%lu.count"
			, task->GetServerDes().c_str()
			, jobId, taskId);
		m_Path = relativePath + fileBuf;
		LOG_TRACE("relative path:"<<relativePath<<",file="<<m_Path<<",j="<<jobId<<",t="<<taskId);
	}
	m_File = fopen(m_Path.c_str(), "wb");
	if (!m_File)
	{
		HUMMER_ASSERT(false);
	}
	m_watcher = watcher;
	HUMMER_NOUSE(task);
	return true;
}

void hummer::CTestProcessor::Close()
{
	if (m_File)
	{
		CGuard guard(&m_Lock);
		fclose(m_File);
		m_File = NULL;
	}
	return;
}

void hummer::CTestProcessor::ProcessMsg(MsgPTR& msg)
{
	hummer::testcounter* counter = dynamic_cast<hummer::testcounter*>(msg.get());
	if (counter && m_watcher)
	{
		CGuard guard(&m_Lock);
		char buf[64] = {0};
		sprintf(buf, "%lu\n", counter->counter());
		if (m_File)
		{
			fwrite(buf, sizeof(char), strlen(buf), m_File);
		}
		m_watcher->Ack(counter->counter());
	}
	return;
}

/*********CTestSplitTasker**********/
hummer::CTestSplitTasker::CTestSplitTasker()
: m_Status(hummer::CTasker::TASKER_INVALID), m_TotalNum(0), m_SplitNum(0){}

hummer::CTestSplitTasker::~CTestSplitTasker(){}

hummer::CTasker::TaskerStatus hummer::CTestSplitTasker::Status()
{
	CGuard guard(&m_Lock);
	return m_Status;
}

void hummer::CTestSplitTasker::GetInfo(CTaskerInfo& info)
{
	CGuard guard(&m_Lock);
	info = m_Result;
}

//split text, DATA="total-1"
bool hummer::CTestSplitTasker::Run(TaskPTR& task, CHummerCtx& ctx)
{
	if (!task.get())
	{
		return false;
	}
	CGuard guard(&m_Lock);
	SplitPTR& split = task->GetSplit();
	string splitdata = split->GetText();
	LOG_TRACE("recv task:id="<<split->GetId()<<",type="<<split->GetType()<<",text="<<split->GetText());
	string::size_type pos = splitdata.find("-");
	if (pos == string::npos)
	{
		return false;
	}
	m_TotalNum = atoi(splitdata.substr(0, pos).c_str());
	m_SplitNum = atoi(splitdata.substr(pos + 1).c_str());
	int perSplit = m_TotalNum / m_SplitNum;
	for (int i = 0; i < m_SplitNum; ++i)
	{
		SplitPTR split(new(std::nothrow) CSplit());
		if (!split.get())
		{
			return false;
		}

		char splitId[64] = {0};
		char splitText[64] = {0};
		sprintf(splitId, "split-%d", i);
		sprintf(splitText, "%d:%d"
			, i * perSplit, (i + 1) * perSplit);
		split->Init(CSplit::SPLIT_MAP, splitId, splitText);
		m_Result.AddSplit(split);
	}

	m_Status = CTasker::TASKER_FINI;
	return true;
}

void hummer::CTestSplitTasker::Stop()
{
	CGuard guard(&m_Lock);
	m_TotalNum = 0;
	m_SplitNum = 0;
	m_Status = hummer::CTasker::TASKER_INVALID;
}



