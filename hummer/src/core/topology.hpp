#ifndef TOPOLOGY_HPP
#define TOPOLOGY_HPP

#include "msg.hpp"
#include <map>
#include <string>
using std::map;
using std::string;

namespace hummer
{

//unused now
class CNode
{
public:
	CNode(){}
	~CNode(){}
private:
};

class CTopology
{
public:
	CTopology();
	~CTopology();

	bool ConvertByMsg(MsgPTR& msg);
	bool ConvertToMsg(MsgPTR& msg);
	
	void SetTopologyId(TOPOLOGYID id);
	const map<string, CNode>& GetNodes()const;
private:
	TOPOLOGYID m_TopoId; 
	map<string, CNode> m_Nodes;
};

};

typedef boost::shared_ptr<hummer::CTopology> TopologyPTR;

#endif

