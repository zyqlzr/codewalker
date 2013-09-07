#include "test.hpp"
#include "utildef.hpp"
#include "Logger.h"
#include "sketch.pb.h"
#include "datacache.hpp"

#include <stdio.h>
#include <string.h>

typedef struct timed_cached_s
{
	int iMsgNum;
	int stepNum;
}timed_cached_t;

int timeCachedTest(void* arg)
{
	timed_cached_t* parm = (timed_cached_t*)arg;
	if (!parm)
	{
		return -1;
	}

	hummer::CTimedCached dataset;
	dataset.Init(parm->stepNum / 2);
	int64_t msgid = 0;
	uint32_t totalCount = 0;
	uint32_t timeoutCount = 0;
	int perStepMsgNum = parm->iMsgNum / parm->stepNum;
	for (int i = 0; i < parm->stepNum; ++i)
	{
		DatasPTR timeoutData;
		for (int j = 0; j < perStepMsgNum; ++j)
		{
			++msgid;
			char id[128] = {0};
			sprintf(id, "%ld",msgid);
			string idstring = id;
			hummer::image* pImage = new(std::nothrow) hummer::image();
			MsgPTR imageptr(pImage);
			pImage->set_iid(idstring);
			dataset.PushValue(msgid, imageptr);
			++totalCount;
		}
		dataset.RoundOneStep(timeoutData);
		if (timeoutData.get())
		{
			timeoutCount += timeoutData->size();
			totalCount -= timeoutData->size();
			/*DATAS::iterator tIter = timeoutData->begin();
			for (; tIter != timeoutData->end(); ++tIter)
			{
				LOG_TRACE("Timeout id="<<(tIter->first));
			}*/
		}
		
		//printf("TimeCached step, timeout=%d, loop=%d, msgnum=%d, total=%d\n"
		//		, timeoutCount, i, dataset.MsgNum(), totalCount);
	}

	int64_t maxId = parm->stepNum; 
	int64_t startId = parm->stepNum / 2;
	for (int m = 0; m < parm->stepNum; ++m)
	{
		for (int n = 0; n < perStepMsgNum; ++n)
		{
			int64_t msgid = (m + 1) * (n + 1);
			dataset.EraseValue(msgid);
		}
		printf("TimeCached step, delete msg, left_msgnum=%d\n"
			, dataset.MsgNum());	
	}
	dataset.UnInit();
	return 0;
}

timed_cached_t cached_test = 
{
	40,
	10
};

timed_cached_t cached_parm = 
{
	100000,
	10
};

test_instance_t time_cached_test = 
{
	"time_cached_test",
	100,
	0,
	{
		{1, timeCachedTest, (void*)(&cached_test), "cached parm test"},
		{END_TASK, NULL, NULL, NULL}
	}
};


