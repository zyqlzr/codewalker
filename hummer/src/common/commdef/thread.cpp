/*****************************************************
Copyright 2010-20XX ZY 
FileName: clusterzoo.cpp
Description: the wrapper of zookeeper api 
Author: zhengyang
Date: 2013.06.03
*****************************************************/

#include "thread.hpp"
#include <pthread.h>
#include <stdio.h> 


#define THREAD_UNINIT 0

CThread::CThread():m_pArg_(NULL), m_fCB_(NULL), m_Thread_(THREAD_UNINIT){}

CThread::~CThread()
{
	if (NULL != m_pArg_)
	{
		m_pArg_ = NULL;
	}

	if (NULL != m_fCB_)
	{
		m_fCB_ = NULL;
	}
}

bool CThread::Start(Thread_CB fcb, void* arg)
{
	m_pArg_ = arg;
	m_fCB_ = fcb;
	if (0 != pthread_create(&m_Thread_, NULL, ThreadRoutine, (void*)this))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CThread::Stop()
{
	if(THREAD_UNINIT != m_Thread_)
	{
		pthread_join(m_Thread_, NULL);
	}
	m_Thread_ = THREAD_UNINIT;
}


void* CThread::ThreadRoutine(void* arg)
{
	CThread* thread = NULL;

	if (NULL == arg)
	{
		return NULL;
	}

	thread = (CThread*)arg;
	thread->m_fCB_(thread->m_pArg_);
	return NULL;
}


