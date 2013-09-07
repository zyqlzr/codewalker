#include "pollerbase.hpp"
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/epoll.h>

#include <errno.h>

#include "event.hpp"
#include "Logger.h"

const char* LOG_EPOLL = "EPOLL_API ";

const int EPOLL_MAX_FD = 1024;

typedef struct epoll_info_s
{
	int iEpHandle;
	int iListenMax;
	struct epoll_event * pEvArray;
}epoll_info_t;

void * epoll_create()
{
	epoll_info_t * epollInfo = NULL;
	int bufferSize = 0;
	struct rlimit limit;

	if (-1 == getrlimit(RLIMIT_NOFILE, &limit))
	{
        	LOG_ERROR(LOG_EPOLL<<"EPOLL call getrlimit failed");
		return NULL;
	}

	int listen_max = (EPOLL_MAX_FD > limit.rlim_cur) ? limit.rlim_cur : EPOLL_MAX_FD;
	bufferSize = sizeof(epoll_info_t) + sizeof(struct epoll_event) * listen_max;
	epollInfo = (epoll_info_t *)malloc(bufferSize);
	if (NULL == epollInfo)
	{
		LOG_ERROR(LOG_EPOLL<<"create epoll infomation failed ");
		return NULL;
	}
	memset(epollInfo, 0, bufferSize);

	epollInfo->pEvArray = (struct epoll_event *)((char *)epollInfo + sizeof(epoll_info_t));
	epollInfo->iEpHandle = epoll_create(listen_max);
	if (-1 == epollInfo->iEpHandle)
	{
		switch(errno)
		{
		case ENOMEM:
                	LOG_ERROR(LOG_EPOLL<<"call epoll_create failed ");			
			break;
		case ENFILE:
                	LOG_ERROR(LOG_EPOLL<<"call epoll_create failed, exceed max file descriptor ");
			break;
		case EINVAL:
                	LOG_ERROR(LOG_EPOLL<<"call epoll_create failed, input param is error ");
			break;
		default:
			break;
		}
	}
	epollInfo->iListenMax = listen_max;
	return epollInfo;
}

int EpollAdd(void* handle, int fd, unsigned int oldMask, unsigned int newMask)
{
	epoll_info_t * epollInfo = NULL;
	struct epoll_event ev = {0, {0}};
	unsigned int op = (oldMask == EV_TYPE_NONE) ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

	epollInfo = (epoll_info_t *)handle;
	if (NULL == epollInfo)
	{
		LOG_ERROR(LOG_EPOLL<<"input epoll infomation is null ");
		return -1;
	}

	ev.data.fd = fd;
	if (newMask & EV_TYPE_READ)
	{
		ev.events |= EPOLLIN;
	}

	if (newMask & EV_TYPE_WRITE)
	{
		ev.events |= EPOLLOUT;
	}

	if (epoll_ctl(epollInfo->iEpHandle ,op,fd,&ev) == -1)
	{
		return -1;
	}
	return 0;
}

int EpollDel(void* handle, int fd, unsigned int oldMask, unsigned int newMask)
{
	epoll_info_t * epollInfo = NULL;
	struct epoll_event ev = {0, {0}};
	unsigned int mask = oldMask & (~newMask);
	
	unsigned int op = (mask == EV_TYPE_NONE) ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;
	
	epollInfo = (epoll_info_t *)handle;
	if (NULL == epollInfo)
	{
		LOG_ERROR(LOG_EPOLL<<"input epoll infomation is null ");
		return -1;
	}

	ev.data.fd = fd;
	if (mask & EV_TYPE_READ)
	{
		ev.events |= EPOLLIN;
	}

	if (mask & EV_TYPE_WRITE)
	{
		ev.events |= EPOLLOUT;
	}

	if (epoll_ctl(epollInfo->iEpHandle ,op,fd,&ev) == -1)
	{
		return -1;
	}
	return 0;
}

int EpollDispatch(void* handle,  struct timeval * timeout, void* resultset)
{
	epoll_info_t * epollInfo = (epoll_info_t *)handle;
	result_t* result = (result_t *)resultset;
	int ret = -1;
	if (NULL == handle || NULL == result)
	{
		LOG_ERROR(LOG_EPOLL<<"EPOLL input epoll infomation is null");
		return -1;
	}

	ret = epoll_wait(epollInfo->iEpHandle
		, epollInfo->pEvArray
		, epollInfo->iListenMax 
		, timeout ? (timeout->tv_sec*1000 + timeout->tv_usec/1000) : -1);
	if (ret > 0)
	{
		struct epoll_event *tmpev = NULL;
		for (int i = 0; i < ret; i++)
		{
			tmpev = epollInfo->pEvArray + i;
			if (result->count >= result->maxcount)
			{
				LOG_ERROR(LOG_EPOLL<<"the count of ready io is exceed max ");
				break;
			}
		}

		result->resultJob[result->count].iSock = tmpev->data.fd;
		result->resultJob[result->count].iMask |= tmpev->events;
		result->count++;
        }

	return ret;
}

int EpollDestroy(void* handle)
{
	epoll_info_t * epollInfo = (epoll_info_t *)handle;
	if (NULL == epollInfo)
	{
		LOG_ERROR(LOG_EPOLL<<"input epoll infomation is null ");
		return -1;
	}

	free(epollInfo);
	handle = NULL;
	return 0;
}

io_poller_t io_epoll = 
{
	epoll_create, 
	EpollAdd, 
	EpollDel, 
	EpollDispatch, 
	EpollDestroy, 
};

