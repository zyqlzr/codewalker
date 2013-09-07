/*****************************************************
 * Copyright zhengyang   
 * FileName: serverbase.hpp 
 * Description: the parent class of monitor  
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef SERVER_BASE_HPP
#define SERVER_BASE_HPP

#include "zkserver.hpp"
#include "iopoller.hpp"
#include "cmdnotify.hpp"
#include "thread.hpp"
#include "hummerctx.hpp"
#include "serverbase.hpp"

namespace hummer
{

class CServerBase : public CZkRegister, public CTimeEvent, public CIoEvent
{
public:
	CServerBase();
	virtual ~CServerBase();
	
	enum ServerType
	{
		SERVER_INVALID = -1,
		SERVER_MASTER = 0,
		SERVER_WORKER,
		SERVER_MAX
	};

	enum
	{
		SERVER_TIMER = 5
	};
	bool Init(const char* conf);
	void UnInit();
	void Run();

	bool PushCmd(CmdPTR& cmd);
	virtual bool ConsumeCmd(CmdPTR& cmd) = 0;
	virtual void Timer() = 0;
	virtual bool DoInit() = 0;
	virtual void DoRun() = 0;
	virtual void DoUnInit() = 0;
	virtual ServerType GetType() = 0;
	
	CHummerCtx& GetCtx();
	CZkServer& GetZk();
	const string& GetServerDes();	
private:
	int Time();
	int IoRead(int fd);
	int IoWrite(int fd);
	static void* IoEventLoop(void* arg);
protected:
	bool m_ExitFlag;
	CThread m_Thread;
	string m_ServerDes;
	string m_ZkAddr;
	CIoPoller m_EventLoop;
	CmdNotify m_CmdNotify;
	CZkServer m_Zk;
	CHummerCtx m_Conf;
};

};

#endif


