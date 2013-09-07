#include "pbdef.hpp"
#include <assert.h>
#include <stdio.h>

void assert_expr(bool flag
		, const char* filename
		, const char* funcname 
		, int line) 
{
	if (!flag)
	{
		printf("assert at %s,%s,%d\n", filename, funcname, line);
	}	
	assert(flag);
}


