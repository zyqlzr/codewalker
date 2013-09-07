#include "serverbase.hpp"
#include "utildef.hpp"

int hummer::CServerBase::Time()
{
	Timer();
	return 0;
}

bool hummer::CServerBase::PushCmd(CmdPTR& cmd)
{
	return m_CmdNotify.PushCmd(cmd);
}

int hummer::CServerBase::IoRead(int fd)
{
	HUMMER_ASSERT(fd == m_CmdNotify.Fd());
	list<CmdPTR> cmds;
	if (m_CmdNotify.PollCmd(cmds))
	{
		list<CmdPTR>::iterator cIter = cmds.begin();
		for (; cIter != cmds.end(); ++cIter)
		{
			ConsumeCmd(*cIter);		
		}
		return 0;
	}
	return -1;
}

int hummer::CServerBase::IoWrite(int fd)
{
	HUMMER_ASSERT(false);
	HUMMER_NOUSE(fd);
	return -1;
}

void* hummer::CServerBase::IoEventLoop(void* arg)
{
	CServerBase* server = (CServerBase*)arg;
	if (NULL == server)
	{
		return NULL;
	}

	while(server->m_ExitFlag)
	{
		server->m_EventLoop.Event();		
	}
	LOG_TRACE("Server exit loop");
	return  NULL;
}

hummer::CServerBase::CServerBase(){}

hummer::CServerBase::~CServerBase(){}

bool hummer::CServerBase::Init(const char* conf)
{
	if (NULL == conf)
	{
		LOG_ERROR("config file is null");
		return false;
	}

	if (!m_Conf.LoadLocalConf(conf) 
			|| !m_Conf.GetZKAddr(m_ZkAddr)
			|| !m_Conf.GetServerDes(m_ServerDes))
	{
		LOG_ERROR("load config err");
		return false;
	}
	else
	{
		LOG_TRACE("Server start, zk="<<m_ZkAddr<<",des="<<m_ServerDes);
	}

	if (!m_Zk.Register(this))
	{
		LOG_ERROR("zk register err");
		return false;
	}

	return DoInit();
}

void hummer::CServerBase::UnInit()
{
	m_ExitFlag = false;
	m_Thread.Stop();
	DoUnInit();
	m_CmdNotify.UnInit();
	m_EventLoop.UnInit();
	m_Zk.Unit();	
	return;
}

void hummer::CServerBase::Run()
{
	if (!m_CmdNotify.Init()
		|| 0 != m_EventLoop.Init(IO_TYPE_EPOLL))
	{
		LOG_ERROR("Server init failed");
		return;
	}

	SetTimerSec(SERVER_TIMER);
	if (0 != m_EventLoop.AddRead(m_CmdNotify.Fd(), this)
		|| 0 != m_EventLoop.AddTime(this))
	{
		LOG_ERROR("Start event loog failed");
		return;
	}

	if (!m_Zk.Init(m_ZkAddr.c_str()))
	{
		LOG_ERROR("Init zk failed");
		return;
	}

	DoRun();
	m_ExitFlag = true;
	m_Thread.Start(CServerBase::IoEventLoop, this);
	return;
}

hummer::CHummerCtx& hummer::CServerBase::GetCtx()
{
	return m_Conf;
}

hummer::CZkServer& hummer::CServerBase::GetZk()
{
	return m_Zk;
}

const string& hummer::CServerBase::GetServerDes()
{
	return m_ServerDes;
}


