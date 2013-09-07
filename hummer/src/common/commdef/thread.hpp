/*****************************************************
 * Copyright zhengyang
 * FileName: thread.hpp
 * Description: the thread wrapper
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_THREAD_H
#define PUBSUB_THREAD_H

#include <pthread.h>

typedef void* (* Thread_CB)(void*);

class CThread
{
public:
	CThread();
	~CThread();
	
	bool Start(Thread_CB fcb, void* arg);
	void Stop();
private:
	static void* ThreadRoutine(void* arg); 
	void* m_pArg_;
	Thread_CB m_fCB_;
	pthread_t m_Thread_;
};


#endif
