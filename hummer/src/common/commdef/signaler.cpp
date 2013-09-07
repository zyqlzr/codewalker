#include <sys/eventfd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

#include "Logger.h"
#include "signaler.hpp"

CSignal::CSignal(): m_WRFD(-1), m_RDFD(-1){}

CSignal::~CSignal(){}

bool CSignal::Init()
{
	if (MakePair(m_RDFD, m_WRFD)
		&& SetUnBlock(m_RDFD) 
		&& SetUnBlock(m_WRFD))
	{
		return true;
	}

	return false;
}

void CSignal::UnInit()
{
	close(m_WRFD);
	close(m_RDFD);
	return;
}

int CSignal::GetFd()
{
	return m_RDFD;
}

void CSignal::Send()
{
	const uint64_t wto = 1;
	int rc = write(m_WRFD, &wto, sizeof(wto));
	if(sizeof(wto) != rc 
		&& errno != EAGAIN)
	{
		LOG_ERROR("write rc="<<rc<<",size="<<sizeof(wto));
		//MCS_ASSERT(false);
	}
}

void CSignal::Recv()
{
	uint64_t rfrom;
	int rc = read(m_RDFD, &rfrom, sizeof (rfrom));
	if(sizeof(rfrom) != rc
		&& errno != EAGAIN)
	{
		LOG_ERROR("read rc="<<rc<<",size="<<sizeof(rfrom));
		//MCS_ASSERT(false);
	}
}

bool CSignal::MakePair(int& rfd, int& wfd)
{
	// Create eventfd object.
	int fd = eventfd (0, 0);
	if (-1 == fd)
	{
		return false;
	}

	wfd = fd;
	rfd = fd;
	return true;
}

bool CSignal::SetUnBlock(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		flags = 0;
	}

	return (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK)) ? false : true;
}

