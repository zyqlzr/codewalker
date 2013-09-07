/*****************************************************
 * Copyright zhengyang   
 * FileName: server.hpp 
 * Description: the server info 
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef MCS_SERVER_HPP
#define MCS_SERVER_HPP

#include "msg.hpp"
#include <boost/shared_ptr.hpp>

#include <set>
#include <string>
using std::set;
using std::string;

namespace hummer
{

class CServer
{
public:
	CServer();
	~CServer();

	bool ConvertByMsg(MsgPTR& msg);
	bool ConvertToMsg(MsgPTR& msg);

	void SetServerDes(const string& des);
	void SetCpuCore(size_t cpucore);
	void SetBusiness(set<BusinessType>& busis);
	void SetPortRange(uint32_t upper, uint32_t down);

	const string& GetServerDes()const;
	size_t CpuCore()const;
	const set<BusinessType>& GetBusiness()const;
	void GetPortRange(uint32_t& upper, uint32_t& down);
private:
	string m_ServerDes;
	size_t m_CpuCore;
	uint32_t m_PortUpper;
	uint32_t m_PortDown;
	set<BusinessType> m_BusiSupport;
};

};

typedef boost::shared_ptr<hummer::CServer> ServerPTR;

#endif

