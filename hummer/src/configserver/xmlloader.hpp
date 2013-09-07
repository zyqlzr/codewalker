#ifndef CONFSERVER_FILE_MOVER_H
#define CONFSERVER_FILE_MOVER_H

#include <string>
using std::string;

class CXmlFile
{
public:
	CXmlFile();
	~CXmlFile();
	char* XmlText();
	size_t XmlLen();
	bool LoadXml(const char* xmlpath);
	bool Equal(CXmlFile& xml);
private:
	string m_xml;
	char* m_text;
	size_t m_len;
};

#endif


