/*****************************************************
 * Copyright zhengyang
 * FileName: mutex.hpp
 * Description: mutex definition 
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_MUTEX_H
#define PUBSUB_MUTEX_H

#include <pthread.h>

class CMutex
{
public:
	CMutex();
	~CMutex();
	void Lock();
	void UnLock();
	pthread_mutex_t* GetMutex();
private:
	CMutex(const CMutex&);
	const CMutex& operator=(const CMutex&);
private:
	pthread_mutex_t m_Mutex_;
};

class CGuard
{
public:
	CGuard(CMutex* lock);
	~CGuard();
private:
	CGuard();
	const CGuard& operator=(const CGuard&);
private:
	CMutex* m_Pointer_;
};


#endif
