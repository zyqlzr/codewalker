#include "datastat.hpp"

#include "Logger.h"

DataStatistic::DataStatistic()
: m_AmqInCounter(0), m_CommInCounter(0)
, m_CommRecvCounter(0), m_ZmqOutCounter(0)
, m_ExitFlag(false)
{}

DataStatistic::~DataStatistic(){}

bool DataStatistic::Start()
{
	m_ExitFlag = true;
	return m_StatThread.Start(DataStatistic::StatisticRoutine, this);
}

void DataStatistic::Stop()
{
	m_ExitFlag = false;
	m_StatThread.Stop();
}

void DataStatistic::AmqIn()
{
	m_AmqInCounter.fetch_add(1);
}

void DataStatistic::CommIn()
{
	m_CommInCounter.fetch_add(1);
}

void DataStatistic::CommRecv()
{
	m_CommRecvCounter.fetch_add(1);
}

void DataStatistic::ZmqOut()
{
	m_ZmqOutCounter.fetch_add(1);
}

uint32_t DataStatistic::GetAmqIn()
{
	return m_AmqInCounter.load();
}

uint32_t DataStatistic::GetCommIn()
{
	return m_CommInCounter.load();
}

uint32_t DataStatistic::GetCommRecv()
{
	return m_CommRecvCounter.load();
}

uint32_t DataStatistic::GetZmqOut()
{
	return m_ZmqOutCounter.load();
}

void* DataStatistic::StatisticRoutine(void* arg)
{
	DataStatistic* stat = (DataStatistic*)arg;
	if (NULL == stat)
	{
		return NULL;
	}
	uint32_t oldAmqInCounter = 0;
	uint32_t oldCommInCounter = 0;
	uint32_t oldCommRecvCouter = 0;
	uint32_t oldZmqOutCounter = 0;

	uint32_t newAmqInCounter = 0;
	uint32_t newCommInCounter = 0;
	uint32_t newCommRecvCouter = 0;
	uint32_t newZmqOutCounter = 0;
	while(stat->m_ExitFlag)
	{
		newAmqInCounter = stat->GetAmqIn();
		newCommInCounter = stat->GetCommIn();
		newCommRecvCouter = stat->GetCommRecv();
		newZmqOutCounter = stat->GetZmqOut();

		LOG_TRACE("statistic AMQ_In total="<<newAmqInCounter<<",tps="<<(newAmqInCounter - oldAmqInCounter) / CONSTANT_STAT_GAP);
		LOG_TRACE("statistic COMM_IN total="<<newCommInCounter<<",tps="<<(newCommInCounter - oldCommInCounter) / CONSTANT_STAT_GAP);	
		LOG_TRACE("statistic COMM_RECV total="<<newCommRecvCouter<<",tps="<<(newCommRecvCouter - oldCommRecvCouter) /CONSTANT_STAT_GAP);
		LOG_TRACE("statistic ZMQ_OUT total="<<newZmqOutCounter<<",tps="<<(newZmqOutCounter - oldZmqOutCounter) / CONSTANT_STAT_GAP);
		sleep(CONSTANT_STAT_GAP);

		oldAmqInCounter = newAmqInCounter;
		oldCommInCounter = newCommInCounter;
		oldCommRecvCouter = newCommRecvCouter;
		oldZmqOutCounter = newZmqOutCounter;
	}
}

DataStatistic globalStatistic;

