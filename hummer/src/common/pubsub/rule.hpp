/*****************************************************
 * Copyright zhengyang   
 * FileName: rule.hpp 
 * Description: implication of zookeeper directory rule
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_RULE_H
#define PUBSUB_RULE_H

#include "pbconf.hpp"

#include <map>
#include <list>
using std::map;
using std::list;

namespace pubsub
{

class CRule;

class CRules
{
public:
	CRules();
	~CRules();
	typedef map<int, CRule*> RULE_MAP;
	int InitRule(pubsub::CPubSubConf& conf);
	void UnInitRule();
	static int ParsePath(const string& path, string& parent, string& name);
	bool CheckPath(const char* path, NodeLevel& level
					, ZooNodeType& type, SubPattern& sub
					, PubPattern& pub, string& parent
					, string& operName);
	bool CheckPathSubPattern(const char* path, NodeLevel& level
					, ZooNodeType& type, SubPattern& pattern
					, string& parent, string& operName);
	bool CheckPathPubPattern(const char* path, NodeLevel& level
					, ZooNodeType& type, PubPattern& pattern
					, string& parent, string& operName);
	ZooNodeType GetChildType(const char* path)const;
	ZooNodeType GetLevel(const char* path);

	class CRuleIterator
	{
	public:
		CRuleIterator(CRules& rules);
		~CRuleIterator();
		CRule* operator*();
		CRuleIterator& operator++();
		bool IsEnd();
	private:
		CRuleIterator();
		CRuleIterator& operator=(const CRuleIterator&);
	private:
		CRules& m_rules;
		RULE_MAP::iterator m_pointer; 		
	};
	friend class CRules::CRuleIterator;
private:
	RULE_MAP m_NsRule;
};

}

#endif
