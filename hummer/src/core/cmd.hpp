/*****************************************************
 * Copyright zhengyang   
 * FileName: cmd.hpp 
 * Description: the definition of cmd
 * Author: zhengyang
 * Date: 2013.08.02
 * *****************************************************/
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <boost/shared_ptr.hpp>

namespace hummer
{

class Cmd
{
public:
	Cmd(){}
	virtual ~Cmd(){}

	enum CmdType
	{
		CMD_INVALID = -1,
		CMD_XML = 0,
		CMD_TASK,
		CMD_CONN,
		CMD_NETERR,
		CMD_SPOUTEXCEPTION,
		CMD_SPOUTNETERR,
		CMD_TASKHB,
		CMD_SERVER,
		CMD_TOPOLOGY,
		MSG_CONTAINER,
		CMD_CONTAINER,
		CMD_MAX
	};
	virtual int GetType()const = 0;
};

};

typedef boost::shared_ptr<hummer::Cmd> CmdPTR;

#endif
