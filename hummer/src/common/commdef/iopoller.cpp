#include "iopoller.hpp"
#include <sys/time.h>

#include "clock.hpp"

const char* EVE_LOG_HEAD = "base_event ";

const int CIoPoller::DEF_HASH_BUCKET = 1000;
const int CIoPoller::MAX_FD_READY = 1000;

extern io_poller_t io_epoll;

CIoPoller::CIoPoller()
: m_handle(NULL)
, m_Action(NULL)
, m_TimeEventCount(0)
{
    //m_events.reserve(DEF_HASH_BUCKET);
    memset(&m_results, 0, sizeof(result_t));
}

CIoPoller::~CIoPoller()
{
    if (NULL != m_handle)
    {
        m_handle = NULL;
    }

    if (NULL != m_Action)
    {
        m_Action = NULL;
    }
}

int CIoPoller::InitIo(io_poller_t ** pAction, io_type_e type)
{
    if (NULL == pAction)
    {
        return -1;
    }

    switch(type)
    {
    case IO_TYPE_EPOLL:
        {
            LOG_TRACE(EVE_LOG_HEAD<<"the type of io_handle is epoll");
            *pAction = &io_epoll;
        }
        break;
    case IO_TYPE_NONE:
    default:
        {
            LOG_ERROR(EVE_LOG_HEAD<<"the io type is not support");
            return -1;
        }
    }
        return 0;
}

int CIoPoller::Init(io_type_e type)
{
    if (0 != InitIo(&m_Action, type))
    {
        return -1;
    }

    m_handle = (*(m_Action->IoHandleCreate))();
    if (NULL == m_handle)
    {
        LOG_ERROR(EVE_LOG_HEAD<<"create io handle failed");
        return -1;
    }

    m_results.resultJob = (job_t *)new(std::nothrow) char[MAX_FD_READY * sizeof(job_t)];
    if (NULL == m_results.resultJob)
    {
        (*(m_Action->IoHandleDestroy))(m_handle);
        LOG_ERROR(EVE_LOG_HEAD<<"malloc job result set failed");
        return -1;
    }

    memset(m_results.resultJob, 0, MAX_FD_READY * sizeof(job_t));
    m_results.count = 0;
    m_results.maxcount = MAX_FD_READY;
    return 0;
}

void CIoPoller::UnInit()
{
    if (NULL != m_handle && NULL != m_Action)
    {
        if (-1 == (*(m_Action->IoHandleDestroy))(m_handle))
        {
            LOG_ERROR(EVE_LOG_HEAD<<"destroy io handle failed");
        }
    }

    if (NULL != m_results.resultJob)
    {
        delete [] m_results.resultJob;
    }

    memset(&m_results, 0, sizeof(result_t));
    m_events.clear();
    return;
}

int CIoPoller::AddRead(int sock, CIoEvent* io)
{
	if (NULL == io || sock <= 0)
	{
		return -1;
	}

	io->SetRead();	
	m_events[sock] = io; 
	return (*(m_Action->IoEventAdd))(m_handle, sock, 0, io->Type());
}

int CIoPoller::AddWrite(int sock, CIoEvent* io)
{
	if (NULL == io || sock <= 0)
	{
		return -1;
	}

	io->SetWrite();	
	m_events[sock] = io; 
	return (*(m_Action->IoEventAdd))(m_handle, sock, 0, io->Type());
}

int CIoPoller::DelRead(int sock, CIoEvent* io)
{
	if (NULL == io || sock <= 0)
	{
		return -1;
	}
	
	CIoEvent* oldio = m_events[sock];
	if (oldio != io)
	{
		return -1;
	}

	io->ResetRead();
	if (io->IsNoIO())
	{
		m_events[sock] = NULL;
	}
	return (*(m_Action->IoEventAdd))(m_handle, sock, 0, io->Type());
}

int CIoPoller::DelWrite(int sock, CIoEvent* io)
{
	if (NULL == io || sock <= 0)
	{
		return -1;
	}
	
	CIoEvent* oldio = m_events[sock];
	if (oldio != io)
	{
		return -1;
	}

	io->ResetWrite();
	if (io->IsNoIO())
	{
		m_events[sock] = NULL;
	}
	return (*(m_Action->IoEventAdd))(m_handle, sock, 0, io->Type());
}

int CIoPoller::AddTime(CTimeEvent* time)
{
	if (NULL == time)
	{
		return -1;
	}
	time->UpdateTime(CClock::clock_us());
	m_TimeList.push_back(time);
	//printf("%d timer in IoPoller\n", m_TimeList.size());
	return 0;
}

int CIoPoller::DelTime(CTimeEvent* time)
{
	if (NULL == time)
	{
		return -1;
	}

	TIMES::iterator iter = m_TimeList.begin();
	for (; iter != m_TimeList.end(); ++iter)
	{
		if (time == *iter)
		{
			m_TimeList.erase(iter);
			break;
		}
	}
	return 0;
}

bool CIoPoller::ComputeNearTime(struct timeval& time)
{
	uint64_t timegap = 0;
	uint64_t least = 0;
	uint64_t now =  CClock::clock_us();
	if (0 == m_TimeList.size())
	{
		return false;
	}

	for (TIMES::iterator iter = m_TimeList.begin(); iter != m_TimeList.end(); iter++)
	{
		if (0 == least)
		{
			least = (*iter)->NextTime();
		}
	
        	if (least > (*iter)->NextTime())
		{
			least = (*iter)->NextTime();
		}
	}

	if (0 == least)
	{
		return false;
	}

	if (least <= now)
	{
		timegap = 0;		
	}
	else
	{
		timegap = least - now;
	}

	CClock::us_to_timeval(timegap, time);
	//LOG_TRACE(EVE_LOG_HEAD<<"us="<<least<<" timeval sec="<<time.tv_sec<<" usec="<<time.tv_usec);
	return true;
}

void CIoPoller::ProcessTime()
{
	uint64_t now = CClock::clock_us();
	for (TIMES::iterator iter = m_TimeList.begin(); iter != m_TimeList.end(); iter++)
	{
		if ((*iter)->NextTime() < now)
		{
			(*iter)->Time();
			(*iter)->UpdateTime(now);
		}
	}
	return;
}

void CIoPoller::Event()
{
	int iRet = -1;
	struct timeval timeGap, *tmp;
	if (ComputeNearTime(timeGap))
	{
		tmp = &timeGap;
	}
	else
	{
		tmp = NULL;
	}

	iRet = (*(m_Action->IoEventDispatch))(m_handle, tmp, &m_results);
	if (iRet >= 0)
	{
		for (size_t i = 0; i < m_results.count; i++)
		{
			job_t * job = &((m_results.resultJob)[i]);
			CIoEvent* io = m_events[job->iSock];
					
			if ((job->iMask & EV_TYPE_READ) 
                		&& NULL != io && io->IsRead()
				&& 0 != io->IoRead(job->iSock))
            		{
                		LOG_ERROR(EVE_LOG_HEAD<<"excute read callback failed ");
            		}

			if ((job->iMask & EV_TYPE_WRITE)
				&& NULL != io && io->IsWrite()
				&& 0 != io->IoWrite(job->iSock))
            		{
                		LOG_ERROR(EVE_LOG_HEAD<<"excute write callback failed ");
            		}

            job->iSock = 0;
            job->iMask = EV_TYPE_NONE;
        }
        m_results.count = 0;
    }
    ProcessTime();
}


