#include "server.hpp"
#include "utildef.hpp"
#include "server.pb.h"

hummer::CServer::CServer()
: m_CpuCore(0), m_PortUpper(0), m_PortDown(0){}

hummer::CServer::~CServer(){}

bool hummer::CServer::ConvertByMsg(MsgPTR& msg)
{
	hummer::server* pserver = dynamic_cast<hummer::server*>(msg.get());
	if (!msg.get())
	{
		return false;
	}

	m_ServerDes = pserver->serverdes();
	m_CpuCore = pserver->servercpucore();
	m_PortUpper = pserver->portup();
	m_PortDown = pserver->portdown();
	int businum = pserver->supportbuisness_size();
	for (int i = 0; i < businum; ++i)
	{
		m_BusiSupport.insert((BusinessType)(pserver->supportbuisness(i)));
	} 
	return true;
}

bool hummer::CServer::ConvertToMsg(MsgPTR& msg)
{
	hummer::server* newserver = new(std::nothrow) hummer::server();
	MsgPTR serverptr(newserver);
	if (!serverptr.get())
	{
		return false;
	}

	newserver->set_serverdes(m_ServerDes);
	newserver->set_servercpucore(m_CpuCore);
	newserver->set_portup(m_PortUpper);
	newserver->set_portdown(m_PortDown);
	set<BusinessType>::iterator bIter = m_BusiSupport.begin();
	for (; bIter != m_BusiSupport.end(); ++bIter)
	{
		newserver->add_supportbuisness(*bIter);	
	}

	msg = serverptr;
	return true;
}

void hummer::CServer::SetServerDes(const string& des)
{
	m_ServerDes = des;
}

void hummer::CServer::SetBusiness(set<BusinessType>& busis)
{
	m_BusiSupport = busis;
}

void hummer::CServer::SetPortRange(uint32_t upper, uint32_t down)
{
	HUMMER_ASSERT(upper >= down);
	m_PortUpper = upper;
	m_PortDown = down;
}

const string& hummer::CServer::GetServerDes()const
{
	return m_ServerDes;
}

const set<BusinessType>& hummer::CServer::GetBusiness()const
{
	return m_BusiSupport;
}

void hummer::CServer::GetPortRange(uint32_t& upper, uint32_t& down)
{
	upper = m_PortUpper;
	down = m_PortDown;
}

void hummer::CServer::SetCpuCore(size_t cpucore)
{
	m_CpuCore = cpucore;
}

size_t hummer::CServer::CpuCore()const
{
	return m_CpuCore;
}


