#include <stdio.h>
#include "Logger.h"

LoggerPtr CLogger::logger = Logger::getRootLogger();

bool CLogger::isInited = false;

int CLogger::Init ( const char* logfile, const char* logname )
{
    if ( logfile == NULL || logname == NULL )
	{
        return -1;
    }
    string strfile = logfile;
    string strname = logname;
    return CLogger::Init(strfile, strname);
}

int CLogger::Init (const string& logFile, const string& logName)
{
    if ((logFile.empty()) || (logName.empty())) {
        return -1;
    }
    PropertyConfigurator::configure (logFile);
    CLogger::logger = Logger::getLogger (logName);
    CLogger::isInited = true;
    return 0;
}

LoggerPtr& CLogger::getLogger ( )
{
    if ( !isInited ) {
        fprintf(stderr, "FATAL error: Logger not properly initialized.\n");
    }
    return CLogger::logger;
}

