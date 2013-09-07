#ifndef CONF_NODE_DES_H
#define CONF_NODE_DES_H

#include <string>
using std::string;

enum NodeLevel
{
	NODE_INVALID = -1,
	NODE_ROOT = 0,
	NODE_DIR,
	NODE_LEAF,
	NODE_MIX,
	NODE_MAX
};

enum ZooNodeType
{
	ZOO_TYPE_INVALID = -1,
	ZOO_TYPE_NORMAL = 0,
	ZOO_TYPE_EPHEMERAL,
	ZOO_TYPE_SEQUNCE,
	ZOO_TYPE_EPHSEQ,
	ZOO_TYPE_UNDEF,
	ZOO_TYPE_MAX,
};

typedef struct 
{
	const char* sPathname;
	NodeLevel eLevel;
	ZooNodeType eNodeType;
	NodeLevel eChildLevel;	
	ZooNodeType eChildType;
}dir_def_t;


class CDirDef
{
public:
	CDirDef();
	CDirDef(const dir_def_t& def);
	~CDirDef();
	void Init(const dir_def_t& def);
	bool Init(string& path,NodeLevel level, ZooNodeType type
		, NodeLevel clevel, ZooNodeType ctype);   
	
	ZooNodeType GetChildType()const;
	NodeLevel GetChildLevel()const;
	ZooNodeType GetType()const;
	NodeLevel GetLevel()const;
	const string& GetPath()const;
	const CDirDef& operator=(const CDirDef&);
	
	static ZooNodeType FindNode(string& node);
	static NodeLevel FindLevel(string& level);
private:
	string m_path;
	NodeLevel m_eLevel_;
	ZooNodeType m_eType;
	NodeLevel m_eChildLevel;
	ZooNodeType m_eChildType_;
};


#endif

