#include "cmdnotify.hpp"

hummer::CmdNotify::CmdNotify(){}

hummer::CmdNotify::~CmdNotify(){}

bool hummer::CmdNotify::Init()
{
	return m_Signal.Init();
}

void hummer::CmdNotify::UnInit()
{
	m_Signal.UnInit();
}

bool hummer::CmdNotify::PushCmd(CmdPTR& cmd)
{
	if (m_CmdQ.Produce(cmd))
	{
		m_Signal.Send();
		return true;
	}
	LOG_TRACE("CmdNotify push cmd failed");
	return false;
}

bool hummer::CmdNotify::PollCmd(list<CmdPTR>& cmds)
{
	m_Signal.Recv();
	return m_CmdQ.BatchConsume(cmds);
}

int hummer::CmdNotify::Fd()
{
	return m_Signal.GetFd();
}


