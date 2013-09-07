/*****************************************************
  Copyright 2010-20XX ZY 
FileName: clusterzoo.cpp
Description: the wrapper of zookeeper api 
Author: zhengyang
Date: 2013.06.03
 *****************************************************/

#include "mutex.hpp"

CMutex::CMutex()
{
	pthread_mutex_init (&m_Mutex_, NULL);
}

CMutex::~CMutex()
{
	pthread_mutex_destroy (&m_Mutex_);
}

void CMutex::Lock()
{
	pthread_mutex_lock (&m_Mutex_);
}

void CMutex::UnLock()
{
	pthread_mutex_unlock (&m_Mutex_);
}

pthread_mutex_t* CMutex::GetMutex()
{
	return &m_Mutex_;
}

CGuard::CGuard(CMutex* lock): m_Pointer_(lock)
{
	m_Pointer_->Lock();
}

CGuard::~CGuard()
{
	m_Pointer_->UnLock();
}



