#include "nsrule.hpp"
#include "topic_def.hpp" 
#include "pbconf.hpp"

#include "Logger.h"
#include "pbdef.hpp"

#include <string.h>

/***************************CRule definition ******************/
bool pubsub::CRule::GetPredefineSet(list<CDirDef>& predefs)
{
	if (0 == m_DirsDef_.size())
	{
		return false;
	}
	map<string, CDirDef>::iterator iter = m_DirsDef_.begin();
	for (;iter != m_DirsDef_.end(); iter++)
	{
		LOG_TRACE(LOG_PUBSUB_HEAD<<"GetPre dir="<<iter->second.GetPath());
		predefs.push_back(iter->second);	
	}
	return true;
}

pubsub::CRule* pubsub::CRule::CreateRule(int nstype)
{
	if (nstype == CPubSubConf::HUMMER_NS)
	{
		return new(std::nothrow) CDefaultRule();
	}
	else if(nstype == CPubSubConf::CONFIG_NS)
	{
		return new(std::nothrow) CDefaultRule();
	}
	else if(nstype == CPubSubConf::COUNTER_NS)
	{
		return new(std::nothrow) CDefaultRule();
	}
	else if (nstype == CPubSubConf::TEST_NS)
	{
		return new(std::nothrow) CDefaultRule(); 
	}
	else
	{
		return NULL;
	}
}

void pubsub::CRule::DestroyRule(CRule* nsrule)
{
	if (NULL != nsrule)
	{
		delete nsrule;
	}
	return;
}

extern const char* HUMMER_NS_TOPIC;
extern const char* COUNTER_NS_TOPIC;
extern const char* TEST_NS_TOPIC;
extern const char* DUMP_NS_TOPIC;
extern const char* CONFIG_NS_TOPIC;

bool pubsub::CRule::NSType(const char* path, int& nstype)
{
	if (NULL != strstr(path, HUMMER_NS_TOPIC))
	{
		nstype = CPubSubConf::HUMMER_NS;
	}
	else if (NULL != strstr(path, COUNTER_NS_TOPIC))
	{
		nstype = CPubSubConf::COUNTER_NS;
	}
	else if (NULL != strstr(path, TEST_NS_TOPIC))
	{
		nstype = CPubSubConf::TEST_NS;
	}
	else if (NULL != strstr(path, CONFIG_NS_TOPIC))
	{
		nstype = CPubSubConf::CONFIG_NS;
	}
	else
	{
		return false;
	}
	return true;
}

bool pubsub::CRule::Init(int nstype, list<CDirDef>& dirs)
{
	list<CDirDef>::iterator dirIter = dirs.begin();
	for (; dirIter != dirs.end(); ++dirIter)
	{
		string name = dirIter->GetPath();
		m_DirsDef_.insert(DIR_MAP::value_type(name, *dirIter));
		char buf[1024] = {0};
		sprintf(buf,"path=%s,L=%d,T=%d,CL=%d,CT=%d"
			, name.c_str(), dirIter->GetLevel(), dirIter->GetType()
			, dirIter->GetChildLevel(), dirIter->GetChildType());
		LOG_TRACE(LOG_PUBSUB_HEAD<<"Dir_def: %s"<<buf);
	} 
	m_nstype = nstype;
	LOG_TRACE(LOG_PUBSUB_HEAD<<"ns type="<<m_nstype<<" dir num="<<m_DirsDef_.size()); 
	return true;
}
void pubsub::CRule::Destroy()
{
	m_Root_.clear();
	m_DirsDef_.clear();
	return;	
}


/******************CDefaultRule definition*******************/
bool pubsub::CDefaultRule::FindDir(const string& name
				, const string& path, const string& parent
				, NodeLevel& level, ZooNodeType& type
				, string& operName)
{
	if (path == m_Root_)
	{
		level = m_RootDef_.GetLevel();
		type = m_RootDef_.GetType();
		operName = path;
		//LOG_TRACE(LOG_PUBSUB_HEAD<<"1 path="<<path<<",parent="<<parent<<",level="<<level<<",type="<<type);
		return true;
	}
	else if (parent == m_Root_)
	{
		map<string, CDirDef>::iterator iter = m_DirsDef_.find(path);
		if (iter == m_DirsDef_.end())
		{
			return false;
		}
	
		level = iter->second.GetLevel();
		type = iter->second.GetType();
		operName = path;
		//LOG_TRACE(LOG_PUBSUB_HEAD<<"2 path="<<path<<",parent="<<parent<<",level="<<level<<",type="<<type);
		return true;
	}
	else
	{
		map<string, CDirDef>::iterator iter = m_DirsDef_.find(path);
		if (iter != m_DirsDef_.end())
		{
			level = iter->second.GetLevel();
			type = iter->second.GetType();
			operName = path;
		}
		else
		{
			map<string, CDirDef>::iterator iter = m_DirsDef_.find(parent);
			if (iter == m_DirsDef_.end())
			{
				return false;
			}

			if (name == "*" || name == "/")
			{
				level = iter->second.GetLevel();
				type = iter->second.GetType();
				operName = parent;
			}
			else
			{
				level = iter->second.GetChildLevel();
				type = iter->second.GetChildType();
				operName = path;
			}
		}
		//LOG_TRACE(LOG_PUBSUB_HEAD<<"3 path="<<path<<",parent="<<parent<<",level="<<level<<",type="<<type<<",name="<<name);
		return true;
	}
}

bool pubsub::CDefaultRule::CheckSubPattern(string& endName, NodeLevel level, SubPattern& pattern)
{
	PB_NOUSE(level);
	if (0 == endName.length())
	{
		pattern = SUB_ALL;
	}
	else if (endName == "*")
	{
		pattern = SUB_ALL;
	}
	else
	{
		pattern = SUB_SINGLE;
	}
	
	return true;
}

bool pubsub::CDefaultRule::CheckPubPattern(string& endName, NodeLevel level, PubPattern& pattern)
{
	if (endName.length() == 0 || endName == "*")
	{
		pattern = PUB_UNABLE;
	}
	
	if (level == NODE_LEAF)
	{
		pattern = PUB_ABLE;
	}
	else
	{
		pattern = PUB_UNABLE;
	}
	
	return true;
}


