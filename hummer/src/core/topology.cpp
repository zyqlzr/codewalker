#include "topology.hpp"
#include "utildef.hpp"

hummer::CTopology::CTopology(){}

hummer::CTopology::~CTopology(){}

bool hummer::CTopology::ConvertByMsg(MsgPTR& msg)
{
	HUMMER_NOUSE(msg);	
	return true;
}

bool hummer::CTopology::ConvertToMsg(MsgPTR& msg)
{
	HUMMER_NOUSE(msg);
	return true;
}

void hummer::CTopology::SetTopologyId(TOPOLOGYID id)
{
	m_TopoId = id;
}

const map<string, hummer::CNode>& hummer::CTopology::GetNodes()const
{
	return m_Nodes;
}




