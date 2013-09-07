#include "regionscanner.hpp"
#include "Logger.h"

RegionScanner::RegionScanner()
: m_Id(-1), m_HConn(NULL), m_MsgCB(NULL)
, m_Arg(NULL), m_BatchNum(0), m_RowCapacity(0){}

RegionScanner::~RegionScanner(){}

bool RegionScanner::Init(CRegionSplit& rSplit
		, CHbaseConnection* hconn, FilterPTR& filter)
{
	if (!hconn || !filter.get())
	{
		LOG_ERROR("conn or filter null");
		return false;
	}

	vector<Text> columns;
	m_Split = rSplit;
	m_HConn = hconn;
	if (!filter->ColumnSet(columns))
	{
		return false;
	}

	if (m_HConn->CreateRegionScanner(m_Id, m_Split, columns))
	{
		m_Filter = filter;
		return true;
	}

	LOG_ERROR("create scanner failed");
	return false;
}

void RegionScanner::UnInit()
{
	if (m_HConn)
	{
		m_HConn->DestoryRegionScanner(m_Id);
	}
	m_Filter.reset();
	m_Id = 0;
	m_HConn = NULL;
	m_MsgCB = NULL;
}

void RegionScanner::SetEmitCB(MsgEmit me, void* arg)
{
	m_MsgCB = me;
	m_Arg = arg;
}

void RegionScanner::SetScanBatch(int32_t num)
{
	if (num <= 0)
	{
		return;
	}
	m_BatchNum = num;
}

void RegionScanner::SetScanCapacity(int32_t num)
{
	if (num <= 0)
	{
		return;
	}
	m_RowCapacity = num;
}

int RegionScanner::ScanPerform()
{
	if (!m_HConn || !m_Filter.get() || !m_MsgCB)
	{
		LOG_ERROR("conn or filter is err");
		return -1;
	}

	m_RowPool.clear();
	m_RowPool.reserve(m_RowCapacity);
	int rc = m_HConn->Next(m_Id, m_RowPool);
	if (SCAN_EXCEPTION == rc
		|| SCAN_ERR == rc)
	{
		LOG_ERROR("Scan exception,"<<m_Id<<","<<m_BatchNum);
		return -1;
	}

	size_t scanNum = m_RowPool.size();
	//LOG_TRACE("per scan info,rn:"<<scanNum<<",bn:"<<m_BatchNum<<",cn:"<<m_RowCapacity);
	vector<TRowResult>::iterator rowIter = m_RowPool.begin();
	for (; rowIter != m_RowPool.end(); ++rowIter)
	{
		MsgPTR out;
		if (!m_Filter->Filter(*rowIter, out))
		{
			m_Stat.DiscardInc();
		}
		else
		{
			m_Stat.MatchInc();
			(*m_MsgCB)(out, m_Arg);
		}
	}

	if (m_RowPool[scanNum - 1].row == m_Split.KeyEnd())
	{
		LOG_TRACE("Scanner:Region End");
	}

	m_Stat.ScanAdd(scanNum);
	return scanNum;
}

int RegionScanner::ScanPerformByNum()
{
	if (!m_HConn || !m_Filter.get() || !m_MsgCB)
	{
		LOG_ERROR("conn or filter is err");
		return -1;
	}

	if (m_RowCapacity == 0
		|| m_BatchNum <= 0)
	{
		LOG_ERROR("capacity or batch is err");
		return -1;
	}

	m_RowPool.clear();
	m_RowPool.reserve(m_RowCapacity);
	int rc = m_HConn->Next(m_Id, m_BatchNum, m_RowPool);
	if (SCAN_EXCEPTION == rc
		|| SCAN_ERR == rc)
	{
		LOG_ERROR("Scan exception,"<<m_Id<<","<<m_BatchNum);
		return -1;
	}

	size_t scanNum = m_RowPool.size();
	//LOG_TRACE("per scan info,rn:"<<scanNum<<",bn:"<<m_BatchNum<<",cn:"<<m_RowCapacity);
	vector<TRowResult>::iterator rowIter = m_RowPool.begin();
	for (; rowIter != m_RowPool.end(); ++rowIter)
	{
		MsgPTR out;
		if (!m_Filter->Filter(*rowIter, out))
		{
			m_Stat.DiscardInc();
		}
		else
		{
			m_Stat.MatchInc();
			(*m_MsgCB)(out, m_Arg);
		}
	}
	m_Stat.ScanAdd(scanNum);
	return scanNum; 
}

CScannerStat& RegionScanner::GetScanStat()
{
	return m_Stat;
}

void RegionScanner::PrintStat()
{
	LOG_TRACE("RegionScanner stat, scan="<<m_Stat.Scan()<<",match="<<m_Stat.Match()<<",discard="<<m_Stat.Discard());
}

/*************CScannerStat definition****************/
CScannerStat::CScannerStat()
: m_TotalScan(0), m_Match(0), m_Discard(0){}

CScannerStat::~CScannerStat(){}

void CScannerStat::ScanAdd(uint32_t num)
{
	m_TotalScan.fetch_add(num);
}

void CScannerStat::ScanInc()
{
	m_TotalScan.fetch_add(1);
}

void CScannerStat::MatchInc()
{
	m_Match.fetch_add(1);
}

void CScannerStat::DiscardInc()
{
	m_Discard.fetch_add(1);
}
	
uint32_t CScannerStat::Match()
{
	return m_Match.load();
}

uint32_t CScannerStat::Scan()
{
	return m_TotalScan.load();
}

uint32_t CScannerStat::Discard()
{
	return m_Discard.load();
}


