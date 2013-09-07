#include "clock.hpp"
#include <stdio.h>

CClock::CClock (){}

CClock::~CClock (){}

uint64_t CClock::clock_us ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (tv.tv_sec * (uint64_t) 1000000 + tv.tv_usec);
}

uint64_t CClock::clock_ms ()
{
    return clock_us () / 1000;
}

void CClock::us_to_timeval(uint64_t& us, struct timeval& val)
{
	val.tv_sec = us / (uint64_t)1000000;
	val.tv_usec = us % (uint64_t)1000000;
	return;
}

void CClock::ms_to_timeval(uint64_t& ms, struct timeval& val)
{
	val.tv_sec = ms / (uint64_t)1000;
	val.tv_usec = ms % (uint64_t)1000;
	return;
}

