/*****************************************************
  Copyright 2010-20XX ZY 
FileName: clusterzoo.cpp
Description: the wrapper of zookeeper api 
Author: zhengyang
Date: 2013.06.03
 *****************************************************/

#include "rule.hpp"
#include "Logger.h"
#include "pbdef.hpp"
#include "nsrule.hpp"



int pubsub::CRules::ParsePath(const string& path, string& parent, string& name)
{
	if (path.length() <= 1)
	{
		return -1;
	}
	
	string::size_type pos = path.find_last_of('/');
	if (string::npos == pos)
	{
		return -1;
	}
	
	name = path.substr(pos + 1);
	parent = path.substr(0, pos);
	return 0;
}

bool pubsub::CRules::CheckPath(const char* path
		, NodeLevel& level, ZooNodeType& type
		, SubPattern& sub, PubPattern& pub
		, string& parent, string& operName)
{
	string allName = path;
	string endName;
	int nstype = 0;

	if (-1 == ParsePath(allName, parent, endName) 
		|| !(CRule::NSType(path, nstype)))
	{
		return false;
	}

	RULE_MAP::iterator typeIter = m_NsRule.find(nstype);
	if (typeIter == m_NsRule.end() 
		|| !(typeIter->second->FindDir(endName, allName, parent, level, type, operName))
		|| !(typeIter->second->CheckSubPattern(endName, level, sub))
		|| !(typeIter->second->CheckPubPattern(endName, level, pub)))
	{
		return false;
	}
	
	return true;
}

bool pubsub::CRules::CheckPathSubPattern(const char* path
		, NodeLevel& level, ZooNodeType& type
		, SubPattern& pattern, string& parent, string& operName)
{
	string allName = path;
	string endName;
	int nstype = 0;

	if (-1 == ParsePath(allName, parent, endName) 
		|| !(CRule::NSType(path, nstype)))
	{
		return false;
	}

	RULE_MAP::iterator typeIter = m_NsRule.find(nstype);
	if (typeIter == m_NsRule.end() 
		|| !(typeIter->second->FindDir(endName, allName, parent, level, type, operName)) 
		|| !(typeIter->second->CheckSubPattern(endName, level, pattern)))
	{
		return false;
	}
	
	return true;
}

bool pubsub::CRules::CheckPathPubPattern(const char* path
		, NodeLevel& level, ZooNodeType& type
		, PubPattern& pattern, string& parent, string& operName)
{
	string allName = path;
	string endName;
	int nstype = 0;

	if (-1 == ParsePath(allName, parent, endName) 
		|| !(CRule::NSType(path, nstype)))
	{
		return false;
	}

	RULE_MAP::iterator typeIter = m_NsRule.find(nstype);
	if (typeIter == m_NsRule.end() 
		|| !(typeIter->second->FindDir(endName, allName, parent, level, type, operName)) 
		|| !(typeIter->second->CheckPubPattern(endName, level, pattern)))
	{
		return false;
	}
	
	return true;
}

pubsub::CRules::CRules(){}

pubsub::CRules::~CRules(){}

int pubsub::CRules::InitRule(pubsub::CPubSubConf& conf)
{
	list<int> nslist;
	if (!conf.GetNSList(nslist)
		|| nslist.size() == 0)
	{
		return -1;
	}
	
	list<int>::iterator nsIter = nslist.begin();
	for (; nsIter != nslist.end(); ++nsIter)
	{
		CRule* ns = pubsub::CRule::CreateRule(*nsIter);
		if (NULL == ns)
		{
			LOG_TRACE(LOG_PUBSUB_HEAD<<"create ns "<<*nsIter<<" failed"); 
			continue;
		}
		
		list<CDirDef> dirs;
		if (!conf.GetNSDirs(*nsIter, dirs) 
			|| !ns->Init(*nsIter, dirs))
		{
			LOG_TRACE(LOG_PUBSUB_HEAD<<"init ns "<<*nsIter<<" failed"); 
			CRule::DestroyRule(ns);
			continue;
		}
	
		m_NsRule.insert(RULE_MAP::value_type(*nsIter, ns));
	}
	
	return 0;
}

void pubsub::CRules::UnInitRule()
{
	m_NsRule.clear();
	return;
}

pubsub::CRules::CRuleIterator::CRuleIterator(CRules& rules)
: m_rules(rules), m_pointer(rules.m_NsRule.begin()){}

pubsub::CRules::CRuleIterator::~CRuleIterator(){}

pubsub::CRule* pubsub::CRules::CRuleIterator::operator*()
{
	if (m_pointer != m_rules.m_NsRule.end())
	{
		return m_pointer->second;
	}
	return NULL;
}

pubsub::CRules::CRuleIterator& pubsub::CRules::CRuleIterator::operator++()
{
	if (m_pointer != m_rules.m_NsRule.end())
	{
		++m_pointer;
	}
	return *this;
}

bool pubsub::CRules::CRuleIterator::IsEnd()
{
	if (m_pointer == m_rules.m_NsRule.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

