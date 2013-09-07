#include "cmdimp.hpp"
#include "Logger.h"
#include "utildef.hpp"

/**************CXmlCmd definition**************/
hummer::CXmlCmd::CXmlCmd()
: m_Type(XML_INVALID), m_ActType(ACT_INVALID)
, m_buf(NULL), m_len(0){}

hummer::CXmlCmd::~CXmlCmd()
{
	if (m_buf)
	{
		delete [] m_buf;
	}
}

bool hummer::CXmlCmd::Stuff(const char* xmlname, XmlType type
		, ActType acttype, char* buf, size_t len)
{
	if (NULL == xmlname 
		|| type <= XML_INVALID
		|| type >= XML_MAX)
	{
		return false;
	}

	if (buf && 0 != len)
	{
		m_buf = new char[len];
		if (NULL == m_buf)
		{
			return false;
		}
	
		memset(m_buf, 0, len);
		memcpy(m_buf, buf, len);
		m_len = len;
	}
	
	m_Type = type;
	m_ActType = acttype;
	m_xmlname = xmlname;
	return true;
}

hummer::CXmlCmd::XmlType hummer::CXmlCmd::GetXmlType()const
{
	return m_Type;
}

int hummer::CXmlCmd::GetType()const
{
	return CMD_XML;	
}

hummer::ActType hummer::CXmlCmd::GetActType()const
{
	return m_ActType;
}

const string& hummer::CXmlCmd::XmlName()const
{
	return m_xmlname;
}

void* hummer::CXmlCmd::GetBuf()const
{
	return m_buf; 
}

size_t hummer::CXmlCmd::GetLen()const
{
	return m_len;
}

/******************CTaskCmd definition*****************/
hummer::CTaskCmd::CTaskCmd()
: m_ActType(ACT_INVALID){}

hummer::CTaskCmd::~CTaskCmd(){}

bool hummer::CTaskCmd::Stuff(ActType type, const char* topic
		, void* buf, int len)
{
	if (!topic)
	{
		return false;
	}

	m_ActType = type;
	m_TaskName = topic;
	if (buf && 0 != len)
	{
		MsgPTR taskptr(new(std::nothrow) hummer::task());
		if (!taskptr.get() 
			|| !taskptr->ParseFromArray(buf, len))
		{
			return false;
		}
		m_TaskSub = taskptr;
	}

	return true;
}

int hummer::CTaskCmd::GetType()const
{
	return CMD_TASK;
}

MsgPTR& hummer::CTaskCmd::GetMsg()
{
	return m_TaskSub;
}

hummer::ActType hummer::CTaskCmd::GetActType()const
{
	return m_ActType;
}

const string& hummer::CTaskCmd::GetTaskName()const
{
	return m_TaskName;	
}
/***************CNetErrCmd definition***************/
hummer::CNetErrCmd::CNetErrCmd(){}

hummer::CNetErrCmd::~CNetErrCmd(){}

int hummer::CNetErrCmd::GetType()const
{
	return CMD_NETERR;
}

/***************CServerCmd definition****************/
hummer::CServerCmd::CServerCmd(): m_Type(ACT_INVALID){}

hummer::CServerCmd::~CServerCmd(){}

bool hummer::CServerCmd::Stuff(const char* topic, ActType type
		, void* buf, int len)
{
	if (!topic)
	{
		return false;
	}

	m_serverdes = topic;
	m_Type = type;
	if (buf && 0 != len)
	{
		MsgPTR serverptr(new(std::nothrow) hummer::server());
		if (!serverptr.get()
			|| !serverptr->ParseFromArray(buf, len))
		{
			return false;
		}
		m_ServerInfo = serverptr;
	}

	return true;
}

hummer::ActType hummer::CServerCmd::GetActType()
{
	return m_Type;
}

int hummer::CServerCmd::GetType()const
{
	return CMD_SERVER;
}

MsgPTR& hummer::CServerCmd::GetMsg()
{
	return m_ServerInfo;
}

const string& hummer::CServerCmd::ServerDes()const
{
	return m_serverdes;
}

/*************CTaskHBCmd definition*************/
hummer::CTaskHBCmd::CTaskHBCmd(): m_Type(ACT_INVALID){}

hummer::CTaskHBCmd::~CTaskHBCmd(){}

bool hummer::CTaskHBCmd::Stuff(const char* topic, ActType type
		, void* buf, int len)
{
	if (!topic)
	{
		return false;
	}

	m_HBName = topic;
	m_Type = type;
	if (buf && 0 != len)
	{
		MsgPTR taskhbptr(new(std::nothrow) hummer::taskHB());
		if (!taskhbptr.get() 
			|| !taskhbptr->ParseFromArray(buf, len))
		{
			LOG_TRACE("taskhb parse array failed");
			return false;
		}
		m_TaskHB = taskhbptr;
	}

	return true;
}

hummer::ActType hummer::CTaskHBCmd::GetActType()
{
	return m_Type;
}

const string& hummer::CTaskHBCmd::TaskHBName()const
{
	return m_HBName;
}

int hummer::CTaskHBCmd::GetType()const
{
	return CMD_TASKHB;
}

MsgPTR& hummer::CTaskHBCmd::GetMsg()
{
	return m_TaskHB;
}

/************ConnCmd definition***********/
hummer::ConnCmd::ConnCmd(){}

hummer::ConnCmd::~ConnCmd(){}

int hummer::ConnCmd::GetType()const
{
	return CMD_CONN;
}

/************CMsgContainer definition***************/
hummer::CMsgContainer::CMsgContainer()
: m_TextType(TEXT_INVLID){}

hummer::CMsgContainer::~CMsgContainer(){}

bool hummer::CMsgContainer::Stuff(TextType type)
{
	m_TextType = type;
	return true;
}

int hummer::CMsgContainer::GetType()const
{
	return MSG_CONTAINER;
}

hummer::CMsgContainer::TextType hummer::CMsgContainer::GetTextType()const
{
	return m_TextType;
}

map<string, MsgPTR>& hummer::CMsgContainer::Msgs()
{
	return m_childs;
}

/*************CmdContainer definition*************/
hummer::CmdContainer::CmdContainer():m_CmdType(CMD_TYPE_INVALID){}

hummer::CmdContainer::~CmdContainer(){}

bool hummer::CmdContainer::Stuff(CmdTextType type)
{
	if (type <= CMD_TYPE_INVALID 
		|| type >= CMD_TYPE_MAX)
	{
		return false;
	}

	m_CmdType = type;
	return true;
}

int hummer::CmdContainer::GetType()const
{
	return CMD_CONTAINER; 
}

hummer::CmdContainer::CmdTextType hummer::CmdContainer::GetTextType()const
{
	return m_CmdType;
}

map<string, CmdPTR>& hummer::CmdContainer::Cmds()
{
	return m_childs;
}

