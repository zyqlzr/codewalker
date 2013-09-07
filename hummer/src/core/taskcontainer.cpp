#include "taskcontainer.hpp"
#include "utildef.hpp"
#include "Logger.h"
#include "worker.hpp"
#include "tasker.hpp"
#include "taskhb.hpp"

hummer::CTaskContainer::CTaskContainer(){}

hummer::CTaskContainer::~CTaskContainer(){}

bool hummer::CTaskContainer::PlugTask(TaskPTR& task, CWorker* server)
{
	if (!task.get()	|| !server)
	{
		LOG_ERROR("Input is null");
		return false;
	}

	BusinessType bt = task->GetBusiType();
	TaskType tt = task->GetTaskType();
	m_Worker = server;
	m_CurrRunner = CTasker::CreateTasker(bt, tt);
	if (!m_CurrRunner)
	{
		LOG_ERROR("Create tasker failed, taskType="<<tt<<",buis="<<bt);
		return false;
	}
	HUMMER_ASSERT(m_CurrRunner);
	m_Task = task;
	return true;
}

bool hummer::CTaskContainer::Excute()
{
	HUMMER_ASSERT(m_CurrRunner);
	HUMMER_ASSERT(m_Worker);
	if (!m_CurrRunner || !m_Worker
		|| !m_CurrRunner->Run(m_Task, m_Worker->GetCtx()))
	{
		LOG_ERROR("run tasker err");
		return false;
	}

	SetTimerSec(HEARTBEAT_GAP);
	if (!m_CmdNotify.Init()
		|| 0 != m_EventLoop.Init(IO_TYPE_EPOLL)
	 	|| 0 != m_EventLoop.AddRead(m_CmdNotify.Fd(), this)
		|| 0 != m_EventLoop.AddTime(this))
	{
		LOG_ERROR("init failed");
		return false;
	}

	m_ExitFlag = true;
	return m_Thread.Start(hummer::CTaskContainer::IoEventLoop, this);
}

void hummer::CTaskContainer::Clean()
{
	m_ExitFlag = false;
	m_Thread.Stop();
	m_CmdNotify.UnInit();
	m_EventLoop.UnInit();
	if (m_CurrRunner)
	{
		m_CurrRunner->Stop();
	}
	CTasker::DestroyTasker(m_CurrRunner);
	m_CurrRunner = NULL;
}

int hummer::CTaskContainer::Time()
{
	if (!m_CurrRunner 
		|| !m_Worker
		|| !m_Task.get())
	{
		LOG_ERROR("tasker is null");
		return -1;
	}

	CTaskerInfo info;
	CTask::TaskStatus ts;
	GetTaskerInfo(ts, info);

	CTaskHBCmd* hbcmd = new(std::nothrow) CTaskHBCmd();
	TaskhbPTR ptaskhb(new(std::nothrow) CTaskHB());
	CmdPTR hbptr(hbcmd);
	if (!ptaskhb.get() || !hbptr.get())
	{
		LOG_ERROR("Lack memory");
		return -1;
	}

	ptaskhb->SetInfo(m_Task->GetJobId(), m_Task->GetTaskId()
		, m_Task->GetBusiType(), m_Task->GetServerDes(), ts);
	ptaskhb->Statistic(info.GetStat());
	ptaskhb->SplitResult(info.GetSplits());
	ptaskhb->Print();

	if (ptaskhb->ConvertToMsg(hbcmd->GetMsg()))
	{
		m_Worker->PushCmd(hbptr);
	}
	else
	{
		LOG_ERROR("tasker heartbeat err");
		return -1;
	}
	return 0;
}

int hummer::CTaskContainer::IoRead(int fd)
{
	// now noused
	HUMMER_ASSERT(fd == m_CmdNotify.Fd());
	list<CmdPTR> cmds;
	if (m_CmdNotify.PollCmd(cmds))
	{
		return 0;
	}
	return -1;
}

int hummer::CTaskContainer::IoWrite(int fd)
{
	HUMMER_ASSERT(false);
	HUMMER_NOUSE(fd);
	return -1;
}

void* hummer::CTaskContainer::IoEventLoop(void* arg)
{
	CTaskContainer* container = (CTaskContainer*)arg;
	if (NULL == container)
	{
		return NULL;
	}

	while(container->m_ExitFlag)
	{
		container->m_EventLoop.Event();		
	}
	LOG_TRACE("container exit loop");
	return  NULL;
}

void hummer::CTaskContainer::GetTaskerInfo(CTask::TaskStatus& ts
		, CTaskerInfo& info)
{
	m_CurrRunner->GetInfo(info);
	switch(m_CurrRunner->Status())
	{
	case CTasker::TASKER_RUNNING:
		ts = CTask::TASK_STATUS_RUNNING;
		break;
	case CTasker::TASKER_FINI:
		ts = CTask::TASK_STATUS_FINI;
		break;
	default:
		ts = CTask::TASK_STATUS_ERR;
		return;
	}
}



