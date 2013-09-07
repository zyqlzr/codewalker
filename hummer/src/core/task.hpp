/*****************************************************
 * Copyright zhengyang
 * FileName: task.hpp 
 * Description: the definition of task
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <boost/shared_ptr.hpp>

#include "msg.hpp"
#include "split.hpp"

#include <set>
#include <string>
using std::set;
using std::string;

namespace hummer
{

class CTask
{
public:
	CTask();
	~CTask();

	enum TaskStatus
	{
		TASK_STATUS_INVALID = -1,
		TASK_STATUS_SCHEDULE = 0,
		TASK_STATUS_RUNNING,
		TASK_STATUS_FAILED,
		TASK_STATUS_FINI,
		TASK_STATUS_ERR,
		TASK_STATUS_MAX
	};

	bool ConvertByMsg(MsgPTR& msg);
	bool ConvertToMsg(MsgPTR& msg);

	void Reset();
	void SetJobId(JID jId);
	void SetTaskId(TID tId);
	void SetServerDes(const string& des);
	void SetTaskType(TaskType tt);
	void SetBusinessType(BusinessType bt);
	void SetSplit(SplitPTR& split);
	void SetSlot(const string& slot);
	void SetSrcAndDst(const set<string>& src, const set<string>& dst);

	const string& GetSlot()const;
	JID GetJobId()const;
	TID GetTaskId()const;
	const string& GetServerDes()const;
	TaskType GetTaskType()const;
	BusinessType GetBusiType()const;
	SplitPTR& GetSplit();
	set<string>& GetSrcs();
	set<string>& GetDsts();
	void Print();
private:
	CTask& operator=(const CTask&);
private:
	TID m_TaskId;
	JID m_JobId;
	string m_ServerDes;
	TaskType m_TaskType;
	BusinessType m_Busi;
	SplitPTR m_Split;
	string m_Slot;
	set<string> m_SrcDes;
	set<string> m_DstDes;
};

};

typedef boost::shared_ptr<hummer::CTask> TaskPTR;

#endif


