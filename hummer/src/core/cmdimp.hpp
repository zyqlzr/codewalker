/*****************************************************
 * Copyright zhengyang   
 * FileName: cmdimp.hpp 
 * Description: the implication of cmd
 * Author: zhengyang
 * Date: 2013.08.03
 * *****************************************************/
#ifndef SERVER_DEF_H
#define SERVER_DEF_H

#include "msg.hpp"
#include "cmd.hpp"
#include "zkserver.hpp"
#include "job.pb.h"
#include "server.pb.h"

#include <boost/shared_array.hpp>

#include <map>
using std::map;

typedef boost::shared_array<char> BufPTR;

namespace hummer
{

enum ActType
{
	ACT_INVALID = -1,
	ACT_ADD = 0,
	ACT_DEL,
	ACT_MOD,
	ACT_MAX
};

class CXmlCmd : public Cmd
{
public:
	CXmlCmd();
	~CXmlCmd();

	enum XmlType
	{
		XML_INVALID = -1,
		XML_WORKER = 0,
		XML_GLOBAL,
		XML_JOB,
		XML_MAX
	};

	bool Stuff(const char* xmlname, XmlType type
			, ActType acttype, char* buf
			, size_t len);
	XmlType GetXmlType()const;
	ActType GetActType()const;
	const string& XmlName()const;

	int GetType()const;
	void* GetBuf()const;
	size_t GetLen()const;
private:
	XmlType m_Type;
	ActType m_ActType;
	char* m_buf;
	int m_len;
	string m_xmlname;
};

class CTaskCmd : public Cmd
{
public:
	CTaskCmd();
	~CTaskCmd();
	bool Stuff(ActType acttype, const char* topic
			, void* buf, int len);
	int GetType()const;
	ActType GetActType()const;
	const string& GetTaskName()const;
	MsgPTR& GetMsg();
private:
	ActType m_ActType;
	string m_TaskName;
	MsgPTR m_TaskSub;	
};

class CTaskHBCmd : public Cmd
{
public:
	CTaskHBCmd();
	~CTaskHBCmd();
	bool Stuff(const char* topic, ActType type, void* buf, int len);
	ActType GetActType();
	int GetType()const;
	const string& TaskHBName()const;
	MsgPTR& GetMsg();
private:
	ActType m_Type;
	string m_HBName;
	MsgPTR m_TaskHB;
};

class CServerCmd : public Cmd
{
public:
	CServerCmd();
	~CServerCmd();
	bool Stuff(const char* topic, ActType type, void* buf, int len);
	ActType GetActType();
	int GetType()const;
	MsgPTR& GetMsg();
	const string& ServerDes()const;
private:
	ActType m_Type;
	MsgPTR m_ServerInfo;
	string m_serverdes;
};

class CMsgContainer : public Cmd
{
public:
	CMsgContainer();
	~CMsgContainer();
	enum TextType
	{
		TEXT_INVLID = -1,
		TEXT_TASK = 0,
		TEXT_TASKHB,
		TEXT_SUBMIT_JOB,
		TEXT_COMMIT_JOB,
		TEXT_SERVER,
		TEXT_MAX
	};
	
	bool Stuff(TextType type);
	int GetType()const;
	TextType GetTextType()const;
	map<string, MsgPTR>& Msgs();
private:
	TextType m_TextType;
	map<string, MsgPTR> m_childs;
};

class CmdContainer : public Cmd
{
public:
	CmdContainer();
	~CmdContainer();
	enum CmdTextType
	{
		CMD_TYPE_INVALID = -1,
		CMD_TYPE_SUBMITXML = 0,
		CMD_TYPE_MAX
	};
	bool Stuff(CmdTextType type);
	int GetType()const;
	CmdTextType GetTextType()const;
	map<string, CmdPTR>& Cmds();
private:
	CmdTextType m_CmdType;
	map<string, CmdPTR> m_childs;
};

class ConnCmd : public Cmd
{
public:
	ConnCmd();
	~ConnCmd();
	int GetType()const;
};

class CNetErrCmd : public Cmd
{
public:
	CNetErrCmd();
	~CNetErrCmd();
	int GetType()const;
};

};

#endif

