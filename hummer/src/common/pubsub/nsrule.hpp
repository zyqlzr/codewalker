/*****************************************************
 * Copyright zhengyang   
 * FileName: nsrule.hpp 
 * Description: the definition of zookeeper rule
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef PUBSUB_NS_RULE_H
#define PUBSUB_NS_RULE_H

#include <string>
#include <map>

#include "pbconf.hpp"

#include "nodedes.hpp"

using std::string;
using std::map;

namespace pubsub
{

class CRule
{
public:
	CRule(): m_nstype(-1){};
	virtual ~CRule(){};
	typedef map<string, CDirDef> DIR_MAP; 
	static CRule* CreateRule(int nstype);
	static void DestroyRule(CRule* nsrule);
	static bool NSType(const char* path, int& nstype);

	virtual bool Init(int nstype, list<CDirDef>& dirs);
	virtual void Destroy();
	bool GetPredefineSet(list<CDirDef>& predefs);

	virtual bool FindDir(const string& Name
		, const string& path, const string& parent
		, NodeLevel& level, ZooNodeType& type
		, string& operName) = 0;
	virtual bool CheckSubPattern(string& endName, NodeLevel level, SubPattern& pattern) = 0;
	virtual bool CheckPubPattern(string& endName, NodeLevel level, PubPattern& pattern) = 0;
protected:
	int m_nstype;
	string m_Root_;
	CDirDef m_RootDef_;
	DIR_MAP m_DirsDef_;
};

class CDefaultRule : public CRule
{
public:
	CDefaultRule(){}
	~CDefaultRule(){}
	bool FindDir(const string& endName, const string& path
			, const string& parent, NodeLevel& level
			, ZooNodeType& type, string& operName);
	bool CheckSubPattern(string& endName, NodeLevel level, SubPattern& pattern);
	bool CheckPubPattern(string& endName, NodeLevel level, PubPattern& pattern);
}; 

}

#endif

