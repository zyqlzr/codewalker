/*****************************************************
 * Copyright zhengyang   
 * FileName: hummerctx.hpp 
 * Description: the configure manger of hummer 
 * Author: zhengyang
 * Date: 2013.08.03
 * *****************************************************/
#ifndef HUMMER_CONF_H
#define HUMMER_CONF_H

#include "xmlwrapper.hpp"
#include "topic_def.hpp"

#include <stdint.h>
#include <string>
#include <map>
using std::string;
using std::map;
using std::pair;

namespace hummer
{

class CParmSet
{
public:
	CParmSet();
	~CParmSet();
	bool SetParm(const string& name, string& value);
	bool GetParm(const string& name, string& value);
	bool GetParm(const string& name, int& value);
	bool GetParm(const string& name, bool& value);
	size_t size()const;
	void Copy(map<string, string>& parms);
private:
	map<string, string> m_ParmSet;
};

class CParmObj
{
public:
	CParmObj();
	~CParmObj();
	bool CheckServer(const string& des);
	bool SetServerParm(const string& server, const string& name, string& value);
	bool GetServerParm(const string& server, const string& name, string& value);
	bool GetServer(const string& des, map<string, string>& parms);
private:
	typedef map<string, CParmSet> SERVER_SETS;
	SERVER_SETS m_servers;	
};

class CHummerCtx
{
public:
	CHummerCtx();
	~CHummerCtx();
	bool LoadLocalConf(const char* localConfig);
	bool LoadMemGlobalConf(const char* buf, int len);
	bool LoadMemWorkerConf(const char* buf, int len);

	bool GetZKAddr(string& zk);
	bool GetServerDes(string& des);

	bool GetGlobalParm(const string& des
			, const string& attr, string& value);
	bool GetGlobalParm(const string& des
			, const string& attr, int& value);
	bool GetGlobalParm(const string& des
			, const string& attr, bool& value);
	bool GlobalHasDes(const string& des);
	bool GlobalServer(const string& des, map<string, string>& parms);

	bool GetWorkerParm(const string& des
			, const string& attr, string& value);
	bool GetWorkerParm(const string& des
			, const string& attr, int& value);
	bool GetWorkerParm(const string& des
			, const string& attr, bool& value);
	bool WorkerHasDes(const string& des);

	const char* GetNs()const;
	TopicId GetTaskTId()const;
	TopicId GetTaskHeartbeatTId()const;
	TopicId GetConfTId()const;
	TopicId GetWorkerTId()const;

	TopicId GetSubmitJobTId()const;
	TopicId GetCommitJobTId()const;

	const char* GetGlobalConf()const;
	const char* GetTaskHBName(uint64_t taskId)const;
	bool GetTaskIDFromHBName(const string& hb, uint64_t& id);
	const char* GetDirSubMod()const;

	CParmSet& GetLocalParms();

	static const char* LOCAL_ROOT;
	static const char* LOCAL_DES;
	static const char* LOCAL_ZK;

	static const char* WORKER_CONF;
	static const char* WORKER_ROOT;
	static const char* WORKER_CPU;
	static const char* WORKER_PORTUPPER;
	static const char* WORKER_PORTDOWN;
	static const char* WORKER_FDFSCONF;

	static const char* GLOBAL_ROOT;
	static const char* GLOBAL_CONF_XML;
	enum SrcDstType
	{
		SRC_DST_INVALID = -1,
		SRC_DST_AMQ = 0,
		SRC_DST_MYSQL,
		SRC_DST_MAX
	};
	static const char* SRC_DST_Type;
	static const char* MYSQL_TYPE;
	static const char* AMQ_TYPE;
	static const char* HBASE_TYPE;
	static const char* LOCALPATH_TYPE;

	static const char* DB_NAME;
	static const char* DB_HOST;
	static const char* DB_USR;
	static const char* DB_PW;
	static const char* DB_PORT;
	static const char* DB_TABLE;
	static const char* DB_UNIXSOCK;

	static const char* AMQ_SRCURI;
	static const char* AMQ_DSTURI;
	static const char* AMQ_TQFLAG;
	static const char* AMQ_ACK;

	static const char* LOCAL_PATH;
	static const char* ABSOLUTE_PATH;
	static const char* RELATIVE_PATH;
private:
	CParmSet m_local;
	CParmObj m_workers;
	CParmObj m_global;
};

};

#endif

