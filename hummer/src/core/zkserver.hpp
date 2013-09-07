/*****************************************************
 * Copyright zhengyang   
 * FileName: zkserver.hpp 
 * Description: the wrapper of zookeeper
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef CONF_SERVER_PB_H
#define CONF_SERVER_PB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <list>
#include <set>
using std::list;
using std::set;

#include "pubsub.hpp"

namespace hummer
{

class CServerBase;
class CZkServer;

class CZkRegister
{
public:
	CZkRegister();
	virtual ~CZkRegister();
	void SetPB(CZkServer* pb);
	void SetServer(CServerBase* server);
	CZkServer* GetPB();

	virtual void DoUnconnected() = 0;
	virtual void DoConnected() = 0;	
protected:
	CZkServer* m_handle;
	CServerBase* m_Server;
};

class CZkActor
{
public:
	CZkActor();
	virtual ~CZkActor();

	void SetServer(CServerBase* server);
	void SetInfo(const char* ns, TopicId id, const char* name, CZkServer* pb);
	TopicId GetTopic()const;
	const string& GetName()const;
	const string& GetNs()const;
	CZkServer* GetHandle();

	static int SubAct(int type, const char* topic, char* buffer, size_t len, void* arg);
	virtual int DoSubActor(int type, const char* topic, char* buffer, size_t len) = 0;	
protected:
	TopicId m_id;
	string m_name;
	string m_ns;
	CZkServer* m_handle;
	CServerBase* m_Server;
};

class CZkServer
{
public:
	CZkServer();
	~CZkServer();
	bool Init(const char* addr);
	void Unit();
	PubsubStatus Status()const;
	PubsubErrCode Code()const;

	bool exist(const char* ns, TopicId id, const char* name);
	bool del(const char* ns, TopicId id, const char* name);
	bool push(const char* ns, TopicId id, const char* name, char* data, size_t len);
	bool pull(const char* ns, TopicId id, const char* name, char* data, size_t& len);
	bool PullChild(const char* ns, TopicId id, const char* name, set<string>& childs);

	bool CreateCounter(const char* cname, int64_t& initvalue);
	bool NextCounter(const char* cname, int64_t& cvalue);
	bool DelCounter(const char* cname);

	bool Sub(CZkActor* actor);
	void UnSub(CZkActor* actor);
	bool Register(CZkRegister* reg);
	void Ungister(CZkRegister* reg);
private:
	bool DoSub(CZkActor* actor);
	static int RegisterZK(int status, void*arg); 	
	static const int TOPIC_LENGTH;
private:
	pbHandle m_handle;
	PubsubStatus m_status;
	int m_code;
	char* m_topicbuf;
	list<CZkActor*> m_actors;
	list<CZkRegister*> m_registers;	
	//topic path to subactor
};

};

#endif

