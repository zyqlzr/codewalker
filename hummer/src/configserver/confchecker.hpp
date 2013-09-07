#ifndef CONF_CHECKER_H
#define CONF_CHECKER_H

#include "thread.hpp"
#include "pbdef.hpp"
#include "xmlwrapper.hpp"
#include "filenotifier.hpp"
#include "pbconfig.hpp"

#include <string>
#include <map>
#include <set>
#include <list>
using std::string;
using std::map;
using std::set;
using std::pair;
using std::list;

class CConfigDir
{
public:
	CConfigDir();
	~CConfigDir();
	void SetDirName(const string& dirname);
	bool AddXml(const string& dirname, const string& ns, const string& xml);
	bool HaveXmlFile(const char* xml);
	const string& NS()const;
	const string& DirName()const;
	void GetXmlfiles(set<string>& files);
	bool IsJob();
	enum DirType
	{
		DIR_INVALID = -1,
		DIR_CONF = 0,
		DIR_JOB,
		DIR_MAX
	};
	static const char* JOB_DIR_NAME;
private:
	string m_ns;
	string m_dirname;
	DirType m_type;	
	set<string> m_xmls; 
};

class CConfChecker : public CNotify 
{
public:
	CConfChecker();
	~CConfChecker();
	bool Start(string& confpath);
	void Stop();
	bool Notify(uint32_t mask, const char* dir, const char* filename);
private:
	void LoadCurrent();

	bool InitByConfigure();
	bool InitModule();
	bool SetUpNotifyDir(multimap<string, xmlNodePtr>& notifydirs);

	bool CheckExist(const char* filepath);
	bool CheckIsDir(const char* filepath);
	bool CheckFileAccess(const char* filepath);
	bool CheckFileRead(const char* filepath);
	bool CheckDirAccess(const char* dirpath);
	bool CreateDir(const char* dirpath);
	bool CreateFile(const char* filepath);

	bool UpdateXml(const char* dir, const char* filename);
	bool DelXml(const char* dir, const char* filename);
	void DirPaths(list<string>& dirpaths);
	void AddOneXml(const string& dirname, string& dirpath, string& nsname, string& xmlname);
	static void* ThreadRoutine(void* arg);

	static const int DIR_ACCESS_MODE;
	static const int FILE_ACCESS_MODE;

	static const char* ROOT_STRING;
	static const char* PARM_STRING;
	static const char* TIMEGAP_ATTR;
	static const char* WORKDIR_ATTR;
	static const char* ZKADDR_ATTR;

	static const char* NAME_ATTR;
	static const char* NS_ATTR;
	static const char* DEFAULT_ROOT_PATH;
	static const int DEFAULT_TIME_GAP;
private:
	CXmlWrapper m_xmlparser;
	string m_RootPath;
	int m_timegap;
	string m_ZKAddr;
	//directory path to (ns, directory name)
	typedef map<string, CConfigDir> NOTIFY_MAP;
	NOTIFY_MAP m_notifys;
	CNotifierMonitor m_monitor;
	CPBConfigure m_pbconf;
};

#endif

