#include "xmlloader.hpp"
#include "Logger.h"

#include <errno.h>
#include <string.h>
#include <stdio.h>

extern const char* LOG_CONFCHECK_HEAD;

CXmlFile::CXmlFile(): m_text(NULL), m_len(0){}

CXmlFile::~CXmlFile()
{
	if (NULL != m_text)
	{
		delete [] m_text;
		m_text = NULL;
	}
}

char* CXmlFile::XmlText()
{
	return m_text;
}

size_t CXmlFile::XmlLen()
{
	return m_len;
}

bool CXmlFile::LoadXml(const char* xmlpath)
{
	FILE* fs = fopen(xmlpath, "r");
	if (NULL == fs)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"open xml "<<xmlpath<<"failed, "<<strerror(errno));
		return false;
	}
	
	fseek(fs, 0, SEEK_END);
	m_len = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	if (NULL != m_text)
	{
		delete [] m_text;
		m_text = NULL;
	}

	m_text = new(std::nothrow) char[m_len];
	if (NULL == m_text)
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"malloc buffer failed,"<<xmlpath<<","<<m_len);
		fclose(fs);
		delete [] m_text;
		m_text = NULL;
		return false;
	}

	if (m_len != fread(m_text, sizeof(char), m_len, fs))
	{
		LOG_TRACE(LOG_CONFCHECK_HEAD<<"fread failed");
		fclose(fs);
		delete [] m_text;
		m_text = NULL;
		return false;
	}

	fclose(fs);
	return true;
}

bool CXmlFile::Equal(CXmlFile& xml)
{
	if (NULL == xml.XmlText() 
		|| m_len != xml.XmlLen())
	{
		return false;
	}
	
	return (0 == memcmp(m_text, xml.XmlText(), m_len)) ? true : false;
}



