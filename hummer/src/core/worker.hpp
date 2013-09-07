/*****************************************************
 * Copyright zhengyang   
 * FileName: worker.hpp 
 * Description: worker monitor
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef WORKER_HPP
#define WORKER_HPP

#include <map>
using std::map;

#include "serverbase.hpp"
#include "zkactor.hpp"


namespace hummer
{

class CTaskContainer;

class CWorker : public CServerBase
{
public:
	CWorker();
	~CWorker();
	bool ConsumeCmd(CmdPTR& cmd);
	void Timer();
	bool DoInit();
	void DoRun();
	void DoUnInit();
	void DoUnconnected();
	void DoConnected();
	ServerType GetType();
private:
	bool ProcessNetErr(CmdPTR& cmd);
	bool ProcessXml(CmdPTR& cmd);
	bool ProcessTask(CmdPTR& cmd);
	bool ProcessConn(CmdPTR& cmd);
	bool ProcessTaskhb(CmdPTR& cmd);
	void ProcessTaskhbDel(TID tid);
	bool CheckExist();

	bool RegisterServer();
	bool ServerUpdate();
	void ReleaseAllTasker();
private:
	bool m_FirstInitFlag;
	bool m_InitFlag;
	bool m_GetConfFlag;
	CXmlActor m_XmlSub;
	CTaskActor m_TaskSub;
	CTaskHBActor m_ThbPusher;
	CServerActor m_ServerPusher;
	typedef map<TID, CTaskContainer*> TASK_SLOTS;
	TASK_SLOTS m_Tasks;
};

};

#endif

