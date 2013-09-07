/*****************************************************
 * Copyright zhengyang   
 * FileName: handler.hpp 
 * Description: the definition of callback handler 
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_HANDLE_H
#define PUBSUB_HANDLE_H

#include <stdlib.h>

namespace pubsub
{

typedef int (*WatchHandler)(int type, const char* topic, char* data, size_t len, void* arg);

class CHandler
{
public:
	CHandler():m_fHandler_(NULL), m_pArg_(NULL){};
	virtual ~CHandler(){};

	void SetHandle(WatchHandler fcb, void* arg)
	{
		m_fHandler_ = fcb;
		m_pArg_ = arg;
	}

	WatchHandler GetFunc()
	{
		return m_fHandler_;
	}

	void* GetArg()
	{
		return m_pArg_;
	}
	
	void Exec(int type, const char* topic, char* data, size_t len)
	{
		if (NULL != m_fHandler_)
		{
			(*m_fHandler_)(type,topic, data, len, m_pArg_);
		}
	}
protected:
	WatchHandler m_fHandler_;
	void* m_pArg_;
};

}

#endif
