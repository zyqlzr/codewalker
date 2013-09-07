#ifndef PUBSUB_DEF_H
#define PUBSUB_DEF_H


void assert_expr(bool, const char* filename, const char* funcname, int line);

#define PB_ASSERT(expr) \
do \
{  \
	if ((expr))	\
	{	\
		assert_expr(true, __FILE__, __FUNCTION__, __LINE__); \
	}	\
	else	\
	{	\
		assert_expr(false, __FILE__, __FUNCTION__, __LINE__); \
	}	\
}while(0)


#define PB_NOUSE(PARM) ((void)PARM) 

#define LOG_PUBSUB_HEAD "[PUBSUB] "

#endif
