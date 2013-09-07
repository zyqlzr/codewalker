/*****************************************************
 * Copyright zhengyang   
 * FileName: workslot.hpp 
 * Description: the abstraction of slot 
 * Author: zhengyang
 * Date: 2013.08.05
 * *****************************************************/

#ifndef WORK_SLOT_HPP
#define WORK_SLOT_HPP

#include <boost/shared_ptr.hpp>
#include <stdint.h>
#include <string>
using std::string;

namespace hummer
{

class CWorkSlot
{
public:
	CWorkSlot(): m_CpuCore(0), m_Port(0){}

	~CWorkSlot(){}

	void Setup(const string& server, uint32_t cpu, uint32_t port)
	{
		m_ServerBelong = server;
		m_CpuCore = cpu;
		m_Port = port;
	}

	void Clean()
	{
		m_ServerBelong.clear();
		m_CpuCore = 0;
		m_Port = 0;
	}

	const string& ServerDes()const
	{
		return m_ServerBelong;
	}

	const string& Slot()const
	{
		return m_ServerBelong;
	}
	
	uint32_t GetCpuCore()const
	{
		return m_CpuCore;
	}

	uint32_t GetPort()const
	{
		return m_Port;
	}
private:
	string m_ServerBelong;
	uint32_t m_CpuCore;
	uint32_t m_Port;
};

};

typedef boost::shared_ptr<hummer::CWorkSlot> SlotPTR;

#endif

