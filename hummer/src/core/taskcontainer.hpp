/*****************************************************
 * Copyright zhengyang   
 * FileName: taskcontainer.hpp 
 * Description: the definiton of task container
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef TASK_CONTAINER_HPP
#define TASK_CONTAINER_HPP

#include "cmd.hpp"
#include "task.hpp"
#include "thread.hpp"
#include "iopoller.hpp"
#include "cmdnotify.hpp"
#include "serverbase.hpp"
#include "tasker.hpp"

namespace hummer
{

class CWorker;

class CTaskContainer : public CTimeEvent, public CIoEvent
{
public:
	CTaskContainer();
	~CTaskContainer();

	bool PlugTask(TaskPTR& task, CWorker* worker);
	bool Notify(CmdPTR& cmd);
	bool Excute();
	void Clean();

	enum
	{
		HEARTBEAT_GAP = 1
	};
private:
	int Time();
	int IoRead(int fd);
	int IoWrite(int fd);
	static void* IoEventLoop(void* arg);
	void GetTaskerInfo(CTask::TaskStatus& ts, CTaskerInfo& info);
private:
	bool m_ExitFlag;
	CThread m_Thread;
	CIoPoller m_EventLoop;
	CmdNotify m_CmdNotify;
	TaskPTR m_Task;
	CTasker* m_CurrRunner;
	CWorker* m_Worker;
};

};

#endif

