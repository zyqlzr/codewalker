#ifndef LOG_H
#define LOG_H

#include <string>
#include <iostream>

#include <logger.h>
#include <propertyconfigurator.h>

using namespace log4cxx;
using namespace std;

#define LOG_TRACE(...) (LOG4CXX_TRACE((CLogger::getLogger()), __VA_ARGS__))
#define LOG_DEBUG(...) (LOG4CXX_DEBUG((CLogger::getLogger()), __VA_ARGS__))
#define LOG_INFO(...) (LOG4CXX_INFO((CLogger::getLogger()), __VA_ARGS__))
#define LOG_WARN(...) (LOG4CXX_WARN((CLogger::getLogger()), __VA_ARGS__))
#define LOG_ERROR(...) (LOG4CXX_ERROR((CLogger::getLogger()), __VA_ARGS__))
#define LOG_FATAL(...) (LOG4CXX_FATAL((CLogger::getLogger()), __VA_ARGS__))

class CLogger
{
public:
	static LoggerPtr& getLogger();
	static int Init(const string&, const string&);
	static int Init(const char*, const char*);
private:
	CLogger();
	CLogger( const CLogger &other );
	~CLogger();
	CLogger& operator=( const CLogger &other );
	static LoggerPtr logger;
	static bool isInited;
};

#endif

