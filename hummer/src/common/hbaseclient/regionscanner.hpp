#ifndef REGION_SCANNER_HPP
#define REGION_SCANNER_HPP

#include "hbasewrapper.h"
#include "operator.hpp"
#include <boost/atomic.hpp>
#include "msg.hpp"

typedef void (*MsgEmit)(MsgPTR& msg, void* arg);

class CScannerStat
{
public:
	CScannerStat();
	~CScannerStat();
	void ScanAdd(uint32_t num);
	void ScanInc();
	void MatchInc();
	void DiscardInc();
	
	uint32_t Match();
	uint32_t Scan();
	uint32_t Discard();
private:
	boost::atomic_uint32_t m_TotalScan;
	boost::atomic_uint32_t m_Match;
	boost::atomic_uint32_t m_Discard;
};

class RegionScanner
{
public:
	RegionScanner();
	~RegionScanner();
	bool Init(CRegionSplit&, CHbaseConnection* hconn
			, FilterPTR& filter);
	void UnInit();

	void SetEmitCB(MsgEmit me, void* arg);
	void SetScanBatch(int32_t num);
	void SetScanCapacity(int32_t num);
	int ScanPerformByNum();
	int ScanPerform();

	CScannerStat& GetScanStat();
	void PrintStat();
private:
	ScannerID m_Id;
	CRegionSplit m_Split;
	CHbaseConnection* m_HConn;
	FilterPTR m_Filter;
	MsgEmit m_MsgCB;
	void* m_Arg;
	int32_t m_BatchNum;
	//row buffer
	vector<TRowResult> m_RowPool;
	size_t m_RowCapacity;
	//scan statistic
	CScannerStat m_Stat;
};

#endif

