/*****************************************************
 * Copyright zhengyang   
 * FileName: cmdnotify.hpp 
 * Description: the cmd notifier
 * Author: zhengyang
 * Date: 2013.08.03
 * *****************************************************/
#ifndef COMMAND_NOTIFY_H
#define COMMAND_NOTIFY_H

#include "batchq.hpp"
#include "cmd.hpp"
#include "signaler.hpp"
#include "iopoller.hpp"

namespace hummer
{

class CmdNotify
{
public:
	CmdNotify();
	virtual	~CmdNotify();
	bool Init();
	void UnInit();
	bool PushCmd(CmdPTR& cmd);
	bool PollCmd(list<CmdPTR>& cmd);
	int Fd();	
private:
	BatchQueue<CmdPTR> m_CmdQ;
	CSignal m_Signal;
};

};

#endif

