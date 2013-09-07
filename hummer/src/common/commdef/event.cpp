#include "event.hpp"

const unsigned int EV_TYPE_NONE = 0;
const unsigned int EV_TYPE_READ = 1;
const unsigned int EV_TYPE_WRITE = 2;
const unsigned int EV_TYPE_MAX = 4;

/*****************CIoEvent definition*********************/
CIoEvent::CIoEvent(): m_type(EV_TYPE_NONE){}

CIoEvent::~CIoEvent(){}

bool CIoEvent::IsNoIO()
{
	return (m_type == EV_TYPE_NONE) ? true : false;
}


bool CIoEvent::IsRead()
{
	return (m_type & EV_TYPE_READ) ? true : false;
}

bool CIoEvent::IsWrite()
{
	return (m_type & EV_TYPE_WRITE) ? true : false; 
}

void CIoEvent::SetRead()
{
	m_type |= EV_TYPE_READ;
}

void CIoEvent::SetWrite()
{
	m_type |= EV_TYPE_WRITE;
}

void CIoEvent::ResetRead()
{
	m_type &=~EV_TYPE_READ;
}

void CIoEvent::ResetWrite()
{
	m_type &=~EV_TYPE_WRITE;
}

void CIoEvent::SetRW()
{
	m_type |= EV_TYPE_READ;
	m_type |= EV_TYPE_WRITE;
}

unsigned int CIoEvent::Type()
{
	return m_type; 
}

/*******************CTimeEvent definition******************/
CTimeEvent::CTimeEvent()
: m_TimeGap(0), m_NextTime(0){}

CTimeEvent::~CTimeEvent(){}

uint64_t CTimeEvent::NextTime()
{
	return m_NextTime;
}

uint64_t CTimeEvent::Gap()
{
	return m_TimeGap;
}

void CTimeEvent::UpdateTime(uint64_t now)
{
	m_NextTime = now + m_TimeGap;	
}

void CTimeEvent::SetTimerSec(uint64_t sec)
{
	m_TimeGap = sec * (uint64_t)1000000;
}

void CTimeEvent::SetTimerMs(uint64_t ms)
{
	m_TimeGap = ms * (uint64_t)1000;
}

void CTimeEvent::SetTimeUs(uint64_t us)
{
	m_TimeGap = us;
}

