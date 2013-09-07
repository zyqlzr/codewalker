/*****************************************************
 * Copyright zhengyang   
 * FileName: datacache.hpp 
 * Description: the implication of data cache
 * Author: zhengyang
 * Date: 2013.08.04
 * *****************************************************/
#ifndef DATA_CACHE_SET_H
#define DATA_CACHE_SET_H

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>

#include "msg.hpp"
#include "mutex.hpp"

#include <stdint.h>
#include <list>
using std::list;

typedef boost::unordered_map<MID, MsgPTR> DATAS;
typedef boost::shared_ptr<DATAS> DatasPTR;

namespace hummer
{

class DataSet
{
public:
	DataSet();
	~DataSet();

	bool Find(MID id, MsgPTR& ptr);
	bool Erase(MID id);
	bool Erase(MID id, MsgPTR& ptr);
	void Push(MID id, MsgPTR& ptr);
	void Clear();

	void DataMove(DatasPTR& dataptr);
	size_t DataNum();
private:
	CMutex m_Lock;
	DatasPTR m_Datas;
};

class CTimedCached
{
public:
	CTimedCached();
	~CTimedCached();

	bool Init(uint32_t step);
	void UnInit();
	bool PushValue(MID id, MsgPTR& ptr);
	bool EraseValue(MID id);
	bool EraseValue(MID id, MsgPTR& ptr);

	void RoundOneStep(DatasPTR& datas);
	size_t MsgNum()const;
private:
	boost::atomic_uint32_t m_TimePoint;
	boost::atomic_uint32_t m_WheelNum;
	DataSet* m_WheelArr;
};

class CFailedCached
{
public:
	CFailedCached();
	~CFailedCached();

	void MsgsTimeout(DatasPTR& datas);
	void MsgFailed(MID id, MsgPTR& msg);
	void GetFailedByNumber(list<MsgPTR>& msgs, size_t num);
	size_t FailedNum();
private:
	CMutex m_Lock;
	list<DatasPTR> m_Timeout;
	DatasPTR m_Failed;
};

};

#endif

