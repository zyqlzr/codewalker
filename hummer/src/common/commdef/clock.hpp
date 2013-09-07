/*****************************************************
 * Copyright zhengyang
 * FileName: clock.hpp
 * Description: linux clock api
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef CLOCK_H
#define CLOCK_H
#include <sys/time.h>
#include <stdint.h>

class CClock
{
public:
	CClock ();
	~CClock ();
	static uint64_t clock_us ();
	static uint64_t clock_ms ();
	static void us_to_timeval(uint64_t& us, struct timeval& val);
	static void ms_to_timeval(uint64_t& ms, struct timeval& val);
private:
	CClock (const CClock&);
	const CClock &operator = (const CClock&);
};

#endif
