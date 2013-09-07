#include "confchecker.hpp"
#include <sys/stat.h>
#include <sys/inotify.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "Logger.h"
#include "xmlloader.hpp"

const char* LOG_CONFCHECK_HEAD = "configure checker ";

/*****************CConfChecker definition**********************/
CConfChecker::CConfChecker(): m_timegap(DEFAULT_TIME_GAP){}

CConfChecker::~CConfChecker(){}

bool CConfChecker::CheckExist(const char* filepath)
{
	if (0 == access(filepath, F_OK))
	{
		return true;
	}
	else 
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"check file: "<<strerror(errno)<<filepath);
		return false;
	}
}

bool CConfChecker::CheckFileAccess(const char* filepath)
{
	if (0 == access(filepath, R_OK | W_OK))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CConfChecker::CheckFileRead(const char* filepath)
{
	if (0 == access(filepath, R_OK))
	{
		return true;
	}
	return false;
}

bool CConfChecker::CheckDirAccess(const char* dirpath)
{
	if (0 == access(dirpath, R_OK | W_OK | X_OK))
	{
		return true;
	}
	else
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"check file: "<<strerror(errno));
		return false;
	}
}

bool CConfChecker::CreateDir(const char* dirpath)
{
	if (-1 == mkdir(dirpath, DIR_ACCESS_MODE))
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"create dir="<<dirpath<<" failed,"<<strerror(errno));
		return false;
	}
	return true;
}

bool CConfChecker::CreateFile(const char* filepath)
{
	int fd = open(filepath, O_RDWR | O_CREAT, FILE_ACCESS_MODE);
	if (-1 == fd)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"create file="<<filepath<<" failed,"<<strerror(errno));
		return false;
	}
	
	close(fd);
	return true;
}

bool CConfChecker::CheckIsDir(const char* filepath)
{
	struct stat filestat;
	if (-1 == stat(filepath, &filestat))
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"get stat of file "<<filepath<<" failed,"<<strerror(errno));
		return false;
	}
	
	return S_ISDIR(filestat.st_mode)? true : false;
}

bool CConfChecker::InitByConfigure()
{
	xmlNodePtr rootNode = NULL;
	string root = ROOT_STRING;
	if (!m_xmlparser.CheckRoot(root))
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"the root node of xml !="<<root);
		return false;
	}
	
	multimap<string, xmlNodePtr> rootChilds;
	rootNode = m_xmlparser.GetRootNode();
	if (NULL == rootNode || !m_xmlparser.GetChilds(rootNode, rootChilds))
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"parse xml failed");
		return false;
	}

	multimap<string, xmlNodePtr>::iterator iter = rootChilds.find(PARM_STRING);
	if (rootChilds.end() == iter)
	{
		m_RootPath = DEFAULT_ROOT_PATH;
		m_timegap = DEFAULT_TIME_GAP;
	}

	string workdirAttr = WORKDIR_ATTR;
	string timegapAttr = TIMEGAP_ATTR;
	string zkAttr = ZKADDR_ATTR;
	if (!m_xmlparser.GetAttr(iter->second, workdirAttr, m_RootPath)
		|| !m_xmlparser.GetAttr(iter->second, timegapAttr, m_timegap)
		|| !m_xmlparser.GetAttr(iter->second, zkAttr, m_ZKAddr))
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"get parm attr failed");
		return false;
	}
	else
	{
		rootChilds.erase(iter);
	}
	
	if (!CheckExist(m_RootPath.c_str())
		&& !CreateDir(m_RootPath.c_str()))
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"root dir is not exist or create failed");
		return false;
	}
	
	if (!CheckIsDir(m_RootPath.c_str())
		|| !CheckDirAccess(m_RootPath.c_str()))
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"root path is not dir or access permission err");
		return false;
	}

	if (!SetUpNotifyDir(rootChilds))
	{
		LOG_ERROR(LOG_CONFCHECK_HEAD<<"set up notify dir err");
		return false;
	}

	return true;
}

bool CConfChecker::SetUpNotifyDir(multimap<string, xmlNodePtr>& notifydirs)
{
	multimap<string, xmlNodePtr>::iterator notifyIter = notifydirs.begin();
	for (; notifyIter != notifydirs.end(); ++notifyIter)
	{
		string notifydir = m_RootPath + "/" + notifyIter->first;
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"notify name="<<notifyIter->first<<" dir="<<notifydir);
		if (!CheckExist(notifydir.c_str())
			&& !CreateDir(notifydir.c_str()))
		{
			continue;
		}

		if (!CheckIsDir(notifydir.c_str())
			|| !CheckDirAccess(notifydir.c_str()))
		{
			continue;
		}

		multimap<string, xmlNodePtr> xmls;
		if (!m_xmlparser.GetChilds(notifyIter->second, xmls) 
			|| xmls.size() == 0)
		{
			continue;
		}
		
		CConfigDir dir;
		dir.SetDirName(notifyIter->first);
		m_notifys.insert(NOTIFY_MAP::value_type(notifydir, dir));
		map<string, xmlNodePtr>::iterator xmlIter = xmls.begin();
		for (; xmlIter != xmls.end(); ++xmlIter)
		{
			string nsAttr = NS_ATTR;
			string nameAttr = NAME_ATTR;
			string nsValue;
			string nameValue;
			bool hasNs = m_xmlparser.GetAttr(xmlIter->second, nsAttr, nsValue); 
			bool hasName = m_xmlparser.GetAttr(xmlIter->second, nameAttr, nameValue);
			if (!hasNs && !hasName)
			{
				continue;
			}
			AddOneXml(notifyIter->first, notifydir, nsValue, nameValue);	
		}	
	}
	return true;
}

void CConfChecker::LoadCurrent()
{
	NOTIFY_MAP::iterator iter = m_notifys.begin();
	for (; iter != m_notifys.end(); ++iter)
	{
		set<string> xmls;
		iter->second.GetXmlfiles(xmls);
		set<string>::iterator xmlIter = xmls.begin();
		for (; xmlIter != xmls.end(); ++xmlIter)
		{
			if (0 == (*xmlIter).length())
			{
				continue;
			}
			UpdateXml(iter->first.c_str(), (*xmlIter).c_str());		
		}
	}
}

void CConfChecker::AddOneXml(const string& dirname, string& dirpath, string& nsname, string& xmlname)
{
	NOTIFY_MAP::iterator iter = m_notifys.find(dirpath);
	if (m_notifys.end() == iter)
	{
		return;
	}
	LOG_TRACE(LOG_CONFCHECK_HEAD<<"add:DIR="<<dirname<<","<<nsname<<","<<xmlname<<",type="<<iter->second.IsJob());	
	iter->second.AddXml(dirname, nsname, xmlname);
}

void CConfChecker::DirPaths(list<string>& dirpaths)
{
	NOTIFY_MAP::iterator iter = m_notifys.begin();
	for (; iter != m_notifys.end(); ++iter)
	{
		dirpaths.push_back(iter->first);
	}
	return;
}

bool CConfChecker::InitModule()
{
	list<string> dirpaths;
	DirPaths(dirpaths);
	NotifyEvent(IN_CLOSE_WRITE);
	NotifyEvent(IN_CREATE);
	NotifyEvent(IN_MOVED_TO);
	NotifyEvent(IN_MOVED_FROM);
	NotifyEvent(IN_MOVE_SELF);
	NotifyEvent(IN_DELETE);
	if (!m_monitor.Init(this, m_timegap, dirpaths) 
		|| !m_pbconf.Init(m_ZKAddr.c_str()))
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"Init monitor or pb failed");
		return false;
	}

	return true;
}

bool CConfChecker::Start(string& confpath)
{
	if (confpath.length() == 0)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"input configpath is err");
		return false;
	}

	if (!CheckExist(confpath.c_str()) 
		|| CheckIsDir(confpath.c_str()))
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"check config path err,"<<confpath);
		return false;
	}

	if (!m_xmlparser.LoadFile(confpath))
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"load config failed,"<<confpath);
		return false;
	}
	
	if (!InitByConfigure() || !InitModule())
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"init config or module failed");
		return false;
	}

	LoadCurrent();
	return true;
}

void CConfChecker::Stop()
{
	m_monitor.UnInit();
	m_pbconf.Unit();
	return;
}

bool CConfChecker::Notify(uint32_t mask, const char* dir, const char* filename)
{
	switch(mask)
	{
	case IN_CLOSE_WRITE:
	case IN_MOVED_TO:
		return UpdateXml(dir, filename);
	case IN_CREATE:
		break;
	case IN_MOVED_FROM:
	case IN_DELETE:
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"create or move from event");
		return DelXml(dir, filename);
		break;	 
	}	
	return true;
}

bool CConfChecker::UpdateXml(const char* dir, const char* filename)
{
	string path = dir;
	path += "/";
	path += filename;
	LOG_TRACE(LOG_CONFCHECK_HEAD<<"update xml "<<"Dir="<<dir<<" file="<<filename);
	if (!CheckExist(path.c_str())
		|| !CheckFileRead(path.c_str()))
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"no permission "<<filename);
		return false;
	}
	
	NOTIFY_MAP::iterator findIter = m_notifys.find(dir);
	if (m_notifys.end() == findIter)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"not find "<<dir);
		return false;
	}

	if (findIter->second.IsJob())
	{
		CXmlFile xml;
		if (!xml.LoadXml(path.c_str()))
		{
			LOG_TRACE(LOG_CONFCHECK_HEAD<<"load failed "<<filename);
			return false;
		}

		if (!m_pbconf.pushJob(findIter->second.NS().c_str(), filename, xml.XmlText(), xml.XmlLen()))
		{
			return false;
		}
	}
	else
	{
		if (!findIter->second.HaveXmlFile(filename))
		{
			LOG_TRACE(LOG_CONFCHECK_HEAD<<"not find "<<filename);
			return false;
		}

		CXmlFile xml;
		if (!xml.LoadXml(path.c_str()))
		{
			LOG_TRACE(LOG_CONFCHECK_HEAD<<"load failed "<<filename);
			return false;
		}

		if (!m_pbconf.push(findIter->second.NS().c_str(), filename, xml.XmlText(), xml.XmlLen()))
		{
			return false;
		}
	}
	return true;
}

bool CConfChecker::DelXml(const char* dir, const char* filename)
{
	string path = dir;
	path += "/";
	path += filename;

	NOTIFY_MAP::iterator findIter = m_notifys.find(dir);
	if (m_notifys.end() == findIter)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"not find "<<dir);
		return false;
	}

	if (findIter->second.IsJob())
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"del xml "<<"Dir="<<dir<<" file="<<filename);
		if (!m_pbconf.DelJob(findIter->second.NS().c_str(), filename))
		{
			LOG_TRACE(LOG_CONFCHECK_HEAD<<"del xml false,"<<"Dir="<<dir<<" file="<<filename);
			return false;
		}
		else
		{
			LOG_TRACE(LOG_CONFCHECK_HEAD<<"del xml ok,"<<"Dir="<<dir<<" file="<<filename);
		}
	}

	return true;
}

const char* CConfChecker::ROOT_STRING = "configureserver";
const char* CConfChecker::PARM_STRING = "parm";

const char* CConfChecker::TIMEGAP_ATTR = "timegap";
const char* CConfChecker::WORKDIR_ATTR = "workdir";
const char* CConfChecker::ZKADDR_ATTR = "zk";

const char* CConfChecker::NAME_ATTR = "name";
const char* CConfChecker::NS_ATTR = "ns";

const int CConfChecker::DIR_ACCESS_MODE = 0777;
const int CConfChecker::FILE_ACCESS_MODE = 0666;
const char* CConfChecker::DEFAULT_ROOT_PATH = "./conf";
const int CConfChecker::DEFAULT_TIME_GAP = 5;

/****************CConfigDir definition*********************/
CConfigDir::CConfigDir():m_type(DIR_INVALID){}

CConfigDir::~CConfigDir(){}

bool CConfigDir::AddXml(const string& dirname, const string& ns, const string& xml)
{
	if (dirname != m_dirname)
	{
		return false;
	}

	if (m_ns.length() == 0)
	{
		m_ns = ns;
	}
	
	m_xmls.insert(xml);
	return true;
}

bool CConfigDir::HaveXmlFile(const char* xml)
{
	set<string>::iterator iter = m_xmls.find(xml);
	if (m_xmls.end() == iter)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void CConfigDir::SetDirName(const string& dirname)
{
	m_dirname = dirname;
	if (m_dirname == JOB_DIR_NAME)
	{
		m_type = DIR_JOB;
	}
	else
	{
		m_type = DIR_CONF;
	}
}

const string& CConfigDir::NS()const
{
	return m_ns;
}

const string& CConfigDir::DirName()const
{
	return m_dirname;
} 	

void CConfigDir::GetXmlfiles(set<string>& files)
{
	files = m_xmls;
	return;
}

bool CConfigDir::IsJob()
{
	if (m_type == DIR_JOB)
	{
		return true;
	}
	else
	{
		return false;
	}
}

const char* CConfigDir::JOB_DIR_NAME = "submit_job";

