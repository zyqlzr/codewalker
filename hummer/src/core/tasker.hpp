/*****************************************************
 * Copyright zhengyang   
 * FileName: tasker.hpp 
 * Description: the definition of tasker
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef TASKER_HPP
#define TASKER_HPP

#include "cmd.hpp"
#include "msg.hpp"
#include "task.hpp"
#include "split.hpp"
#include "hummerctx.hpp"

#include <list>
using std::list;

namespace hummer
{

class CTaskerInfo
{
public:
	CTaskerInfo();
	~CTaskerInfo();
	const CTaskerInfo& operator=(const CTaskerInfo& info);
	void SetStat(const msg_stat_t& stat);
	void AddSplit(SplitPTR& split);

	const msg_stat_t& GetStat()const;
	list<SplitPTR>& GetSplits();
private:
	msg_stat_t m_Stat;
	list<SplitPTR> m_SplitResult;
};

class CTasker
{
public:
	CTasker(){}
	virtual ~CTasker(){}
	enum TaskerStatus
	{
		TASKER_INVALID = -1,
		TASKER_RUNNING = 0,
		TASKER_FINI,
		TASKER_ERR,
		TASKER_MAX
	};

	virtual TaskerStatus Status() = 0;
	virtual void GetInfo(CTaskerInfo& info) = 0; 
	virtual bool Run(TaskPTR& task, CHummerCtx& ctx) = 0;
	virtual void Stop() = 0;

	static CTasker* CreateTasker(BusinessType bt, TaskType tt);
	static void DestroyTasker(CTasker* tasker);
	static void GetSupportBusiness(set<BusinessType>& busis);
};

class CMsgWatcher
{
public:
	CMsgWatcher(){}
	virtual ~CMsgWatcher(){}
	virtual void Info(msg_stat_t& stat) = 0;
	virtual void Watch(MID id, MsgPTR& msg) = 0;	
	virtual void Ack(MID id) = 0;
	virtual void Fail(MID id) = 0;

	virtual void GetRedoMsg(list<MsgPTR>& msgs, size_t num) = 0;
	virtual size_t RedoNum() = 0;
};

};

#endif

