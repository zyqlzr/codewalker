/*****************************************************
 * Copyright zhengyang
 * FileName: event.hpp
 * Description: io event
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_EVENT_H
#define PUBSUB_EVENT_H

#include <stdint.h>

extern const unsigned int EV_TYPE_NONE;
extern const unsigned int EV_TYPE_READ;
extern const unsigned int EV_TYPE_WRITE;
extern const unsigned int EV_TYPE_MAX;

class CIoEvent
{
public:
	CIoEvent();
	virtual ~CIoEvent();
	bool IsNoIO();
	bool IsRead();
	bool IsWrite();
	void SetRead();
	void SetWrite();
	void ResetRead();
	void ResetWrite();
	void SetRW();
	unsigned int Type();

	virtual int IoRead(int fd) = 0;
	virtual int IoWrite(int fd) = 0;
private:
	unsigned int m_type;
};

class CTimeEvent
{
public:
	CTimeEvent();
	virtual ~CTimeEvent();
	void SetTimerSec(uint64_t sec);
	void SetTimerMs(uint64_t ms);
	void SetTimeUs(uint64_t us);
	uint64_t NextTime();
	uint64_t Gap();
	void UpdateTime(uint64_t now);

	virtual int Time() = 0;
private:
	uint64_t m_TimeGap;
	uint64_t m_NextTime;
};

#endif

