#ifndef DATA_STAT_HPP
#define DATA_STAT_HPP

#include <boost/atomic.hpp>
#include "thread.hpp"

class DataStatistic
{
public:
	DataStatistic();
	~DataStatistic();

	bool Start();
	void Stop();

	void AmqIn();
	void CommIn();
	void CommRecv();
	void ZmqOut();

	uint32_t GetAmqIn();
	uint32_t GetCommIn();
	uint32_t GetCommRecv();
	uint32_t GetZmqOut();
	static void* StatisticRoutine(void* arg);
	enum
	{
		CONSTANT_STAT_GAP = 5 
	};
private:
	boost::atomic_uint32_t m_AmqInCounter;
	boost::atomic_uint32_t m_CommInCounter;
	boost::atomic_uint32_t m_CommRecvCounter;
	boost::atomic_uint32_t m_ZmqOutCounter;
	bool m_ExitFlag;
	CThread m_StatThread;
};

extern DataStatistic globalStatistic;

#endif

