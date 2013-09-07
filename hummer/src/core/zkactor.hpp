/*****************************************************
 * Copyright zhengyang   
 * FileName: zkactor.hpp 
 * Description: subscriber of zookeeper dircectory
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef ZK_ACTOR_HPP
#define ZK_ACTOR_HPP

#include "zkserver.hpp"
#include "hummerctx.hpp"
#include "task.hpp"
#include "job.hpp"
#include "cmdimp.hpp"
#include "serverbase.hpp"
#include "server.hpp"

namespace hummer
{

class CZkPuller
{
public:
	CZkPuller(){}
	virtual ~CZkPuller(){}
	virtual bool PullChilds(CmdPTR& cmd) = 0;
	virtual bool PullPer(const string& name, CmdPTR& data) = 0;
	virtual bool PullData(char* buf, size_t& max_buf_len) = 0;
};

class CTaskHBActor : public CZkActor, public CZkPuller
{
public:
	CTaskHBActor();
	~CTaskHBActor();
	bool PullChilds(CmdPTR& cmd);
	bool PullPer(const string& name, CmdPTR& data);
	bool PullData(char* buf, size_t& max_buf_len);

	bool HB(MsgPTR& heartbt);
	void DelHB(TID tid);
	int DoSubActor(int type, const char* topic, char* buffer, size_t len);
};

class CServerActor : public CZkActor, public CZkPuller
{
public:
	CServerActor();
	~CServerActor();
	bool PullChilds(CmdPTR& cmd);
	bool PullPer(const string& name, CmdPTR& data);
	bool PullData(char* buf, size_t& max_buf_len);

	int DoSubActor(int type, const char* topic, char* buffer, size_t len);
	bool Update(ServerPTR& server);
	bool Exist(const string& des);
};

class CXmlActor : public CZkActor, public CZkPuller
{
public:
	CXmlActor();
	~CXmlActor();

	bool PullChilds(CmdPTR& cmd);
	bool PullPer(const string& name, CmdPTR& data);
	bool PullData(char* buf, size_t& max_buf_len);

	int DoSubActor(int type, const char* topic, char* buffer, size_t len);
};

class CTaskActor : public CZkPuller, public CZkActor 
{
public:
	CTaskActor();
	~CTaskActor();
	int DoSubActor(int type, const char* topic, char* buffer, size_t len);
	bool PullChilds(CmdPTR& cmd);
	bool PullPer(const string& name, CmdPTR& data);
	bool PullData(char* buf, size_t& max_buf_len);

	bool AssignTask(const set<TaskPTR>& tasks);
	void StopTask(const set<TaskPTR>& tasks);
private:
	CTaskActor& operator=(const CTaskActor& assigner);
};

class CJobActor : public CZkPuller, public CZkActor
{
public:
	CJobActor();
	~CJobActor();
	int DoSubActor(int type, const char* topic, char* buffer, size_t len);
	bool PullChilds(CmdPTR& cmd);
	bool PullPer(const string& name, CmdPTR& data);
	bool PullData(char* buf, size_t& max_buf_len);

	bool CommitJob(const JobPTR& job);
	void RollbackJob(const JobPTR& job);	
};

class CIdGenerator
{
public:
	CIdGenerator();
	~CIdGenerator();
	void JustTest();
	void SetInfo(CZkServer* pb, const char* ns
		, const char* jobCounter, const char* taskCounter);
	void CheckCounter();
	bool JobId(uint64_t& jobId);
	bool TaskId(uint64_t& taskId);
private:
	bool m_TestFlag;
	CZkServer* m_pb;
	string m_ns;
	string m_JobIdCounter;
	string m_TaskIdCounter;
};

};

#endif

