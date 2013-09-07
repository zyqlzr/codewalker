/*****************************************************
 * Copyright zhengyang   
 * FileName: listener.hpp 
 * Description: the abstract api of listener
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_LISTENER_H
#define PUBSUB_LISTENER_H

namespace pubsub
{

class CListener
{
public:
	CListener(){};
	virtual ~CListener(){};

	virtual int NodeCreate(const char* path) = 0;
	virtual int NodeDelete(const char* path) = 0;
	virtual int DateChange(const char* path) = 0;
	virtual int ChildChange(const char* path) = 0;
};

}

#endif
