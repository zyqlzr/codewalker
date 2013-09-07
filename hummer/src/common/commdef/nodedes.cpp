#include "nodedes.hpp"

const char* NOR_NODE = "nor";
const char* TMP_NODE = "tmp";
const char* SEQ_NODE = "seq";
const char* SEQTMP_NODE = "tmpseq";
const char* UNDEF_NODE = "undef";

const char* ROOT_LEVEL = "root";
const char* DIR_LEVEL = "dir";
const char* LEAF_LEVEL = "leaf";
const char* MIX_LEVEL = "mix";
/*************************CDirDef definition**********************/
CDirDef::CDirDef(): m_eLevel_(NODE_INVALID)
			    , m_eType(ZOO_TYPE_INVALID), m_eChildType_(ZOO_TYPE_INVALID){}
			    
void CDirDef::Init(const dir_def_t& def)
{
	m_path = def.sPathname;
	m_eLevel_ = def.eLevel;
	m_eType = def.eNodeType;
	m_eChildLevel = def.eChildLevel;
	m_eChildType_ = def.eChildType;
	return;
}

bool CDirDef::Init(string& path,NodeLevel level, ZooNodeType type
		, NodeLevel clevel, ZooNodeType ctype)
{
	if(path.length() == 0 || level == NODE_INVALID 
		|| clevel == NODE_INVALID || type == ZOO_TYPE_INVALID
		|| ctype == ZOO_TYPE_INVALID)
	{
		return false;
	}
 
	m_path = path;
	m_eLevel_ = level;
	m_eType = type;
	m_eChildType_ = ctype;
	m_eChildLevel = clevel;
	return true;
}
 
CDirDef::CDirDef(const dir_def_t& def)
{
	m_path = def.sPathname;
	m_eLevel_ = def.eLevel;
	m_eType = def.eNodeType;
	m_eChildLevel = def.eChildLevel;
	m_eChildType_ = def.eChildType;
}

const CDirDef& CDirDef::operator=(const CDirDef& def)
{
	m_path = def.m_path;
	m_eLevel_ = def.m_eLevel_;
	m_eType = def.m_eType;
	m_eChildLevel = def.m_eChildLevel;
	m_eChildType_ = def.m_eChildType_;
	return *this;
}

CDirDef::~CDirDef(){}

NodeLevel CDirDef::GetChildLevel()const
{
	return m_eChildLevel;
}

ZooNodeType CDirDef::GetChildType()const
{
	return m_eChildType_;
}

ZooNodeType CDirDef::GetType()const
{
	return m_eType;
}

NodeLevel CDirDef::GetLevel()const
{
	return m_eLevel_;
}

const string& CDirDef::GetPath()const
{
	return m_path;
}


ZooNodeType CDirDef::FindNode(string& node)
{
	if (node == NOR_NODE)
	{
		return ZOO_TYPE_NORMAL;
	}
	else if (node == TMP_NODE)
	{
		return ZOO_TYPE_EPHEMERAL;
	}
	else if (node == SEQ_NODE)
	{
		return ZOO_TYPE_SEQUNCE;
	}
	else if (node == SEQTMP_NODE)
	{
		return ZOO_TYPE_EPHSEQ;
	}
	else 
	{
		return ZOO_TYPE_INVALID;
	}
}

NodeLevel CDirDef::FindLevel(string& level)
{
	if (level == ROOT_LEVEL)
	{
		return NODE_ROOT;
	}
	else if (level == DIR_LEVEL)
	{
		return NODE_DIR;
	}
	else if (level == LEAF_LEVEL)
	{
		return NODE_LEAF;
	}
	else if (level == MIX_LEVEL)
	{
		return NODE_MIX;
	}
	else
	{
		return NODE_INVALID;
	}
}


