/*****************************************************
 * Copyright zhengyang
 * FileName: pollerbase.hpp
 * Description: io event loop api definition 
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef POLLER_BASE_H
#define POLLER_BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum io_type_e
{
	IO_TYPE_NONE = 0x00,
	IO_TYPE_EPOLL = 0x02,
	IO_TYPE_MAX = 0x80
};

typedef struct job_s
{
	int iSock;
	unsigned int iMask;
}job_t;

typedef struct io_poller_s
{
	void* (* IoHandleCreate)();
	int (* IoEventAdd)(void* handle, int fd, unsigned int oldMask, unsigned int newMask);
	int (* IoEventDel)(void* handle, int fd, unsigned int oldMask, unsigned int newMask);
	int (* IoEventDispatch)(void* handle,  struct timeval * timeout, void* result);
	int (* IoHandleDestroy)(void* handle); 
}io_poller_t;

typedef struct result_s
{
    size_t count;
    size_t maxcount;
    job_t* resultJob;
}result_t;

#endif

