/*****************************************************
 * Copyright zhengyang   
 * FileName: taskhb.hpp 
 * Description: the definition of task heartbeat
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef TASK_HB_HPP
#define TASK_HB_HPP

#include "msg.hpp"
#include "task.hpp"
#include "split.hpp"
#include <stdint.h>
#include <boost/shared_ptr.hpp>

#include <list>
#include <string>
using std::list;
using std::string;

namespace hummer
{

class CTaskHB
{
public:
	CTaskHB();
	~CTaskHB();

	bool ConvertByMsg(MsgPTR& msg);
	bool ConvertToMsg(MsgPTR& msg);

	void SetInfo(JID jid, TID tid, BusinessType bt
			, const string& des, CTask::TaskStatus ts);
	void SplitResult(list<SplitPTR>& results);
	void Statistic(const msg_stat_t& msgstat);
	list<SplitPTR>& GetSplitResult();

	TID GetTid()const;
	JID GetJid()const;
	BusinessType GetBusi()const;
	const string& GetDes()const;
	CTask::TaskStatus GetTaskStatus()const;
	void GetStatistic(uint64_t& total, uint64_t& err, uint64_t& ok);
	void Print();
private:
	TID m_Tid;
	JID m_Jid;
	BusinessType m_Busi;
	string m_ServerDes;
	CTask::TaskStatus m_Status;
	msg_stat_t m_stat;
	list<SplitPTR> m_Splits;
};

};

typedef boost::shared_ptr<hummer::CTaskHB> TaskhbPTR;

#endif

