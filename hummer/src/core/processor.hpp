/*****************************************************
 * Copyright zhengyang   
 * FileName: processor.hpp 
 * Description: the abstract api of processor
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef HUMMER_PROCESSOR_HPP
#define HUMMER_PROCESSOR_HPP

#include "msg.hpp"
#include "task.hpp"
#include "hummerctx.hpp"
#include "tasker.hpp"

namespace hummer
{

class CProcessor;

class CSpout
{
public:
	CSpout(){}
	virtual ~CSpout(){}
	virtual CTasker::TaskerStatus Status() = 0;
	virtual bool Bind(size_t id, CProcessor* processor) = 0;
	virtual bool Open(TaskPTR& task, CMsgWatcher* watcher, CHummerCtx& ctx) = 0;
	virtual void Close() = 0;
	virtual bool GetData() = 0;
	virtual bool ReEmit(MsgPTR& msg) = 0;
};

class CProcessor
{
public:
	CProcessor(){}
	virtual ~CProcessor(){}
	virtual bool Bind(size_t id, CProcessor* processor) = 0;
	virtual bool Prepare(TaskPTR& task, CMsgWatcher* watcher, CHummerCtx& ctx) = 0;
	virtual void Close() = 0; 
	virtual void ProcessMsg(MsgPTR& msg) = 0;
};

};

#endif


