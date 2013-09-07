/*****************************************************
 * Copyright zhengyang
 * FileName: xmlwrapper.hpp
 * Description: xml file wrapper
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef XML_WRAPPER_H
#define XML_WRAPPER_H

#include "parser.h"

#include <string>
#include <map>
using std::string;
using std::multimap;

class CXmlWrapper
{
public:
	CXmlWrapper();
	~CXmlWrapper();

	bool LoadFile(string& file);
	bool LoadMem(const char* buf, int size);
	bool CheckRoot(string& root);
	xmlNodePtr GetRootNode();
	bool GetText(xmlNodePtr xmlnode, string& value);
	bool GetChilds(xmlNodePtr xmlnode, multimap<string, xmlNodePtr>& childs);
	bool GetAttr(xmlNodePtr, string& attr, string& value);
	bool GetAttr(xmlNodePtr, string& attr, int& value);
private:
	xmlDocPtr m_doc;
};

#endif


