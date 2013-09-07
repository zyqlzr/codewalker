#include "xmlwrapper.hpp"

CXmlWrapper::CXmlWrapper()
{
	xmlInitParser();
}

CXmlWrapper::~CXmlWrapper()
{
	if (m_doc)
	{
		xmlFreeDoc(m_doc);
	}
	xmlCleanupParser();
}

bool CXmlWrapper::LoadFile(string& file)
{
	m_doc = xmlReadFile(file.c_str(), "utf-8", XML_PARSE_NOBLANKS);
	if (!m_doc)
		return false;
	return true;
}

bool CXmlWrapper::LoadMem(const char* buffer, int size)
{
	m_doc = xmlReadMemory(buffer, size, NULL, "utf-8", XML_PARSE_NOBLANKS);
	if (!m_doc)
		return false;
	return true;
}

bool CXmlWrapper::CheckRoot(string& root)
{
	xmlNodePtr cur = xmlDocGetRootElement(m_doc);
	if (NULL == cur)
	{
		return false;
	}

	if (xmlStrcmp(cur->name, (const xmlChar*)(root.c_str())))
	{
		return false;
	}

	return true;
}

xmlNodePtr CXmlWrapper::GetRootNode()
{
	return 	xmlDocGetRootElement(m_doc);
}

bool CXmlWrapper::GetChilds(xmlNodePtr xmlnode, multimap<string, xmlNodePtr>& childs)
{
	if (NULL == xmlnode)
	{
		return false;
	}

	xmlNodePtr child = xmlnode->xmlChildrenNode;
	while(child != NULL)
	{
		string childName = (const char*)(child->name);
		childs.insert(multimap<string, xmlNodePtr>::value_type(childName, child));
		child = child->next;
	}
	
	return true;
}

bool CXmlWrapper::GetAttr(xmlNodePtr node, string& attr, string& value)
{
	if (NULL == node)
	{
		printf("node is null\n");
		return false;
	}
	xmlChar* pvalue = NULL;
	pvalue = xmlGetProp(node, (xmlChar*)(attr.c_str()));
	if (NULL == pvalue)
	{
		printf("get node attr failed,attr=%s,node=%s\n",
			attr.c_str(), (const char*)node->name);
		return false;
	}

	value = (const char*)pvalue;
	xmlFree(pvalue);
	return true;
}

bool CXmlWrapper::GetAttr(xmlNodePtr node, string& attr, int& value)
{
	string rowValue;
	if (!GetAttr(node, attr, rowValue))
	{
		return false;
	}

	value =atoi(rowValue.c_str());
	return true;
}

bool CXmlWrapper::GetText(xmlNodePtr xmlnode, string& value)
{
	if (NULL == xmlnode)
	{
		return false;
	}
	
	xmlChar* valueptr = xmlNodeGetContent(xmlnode);
	if (NULL == valueptr)
	{
		return false;
	}

	value = (const char*)valueptr;
	xmlFree(valueptr);
	return true;
}


