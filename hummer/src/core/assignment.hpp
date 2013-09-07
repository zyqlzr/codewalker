/*****************************************************
 * Copyright zhengyang
 * FileName: assignment.hpp 
 * Description: the assignment record of task
 * Author: zhengyang
 * Date: 2013.08.01
 * *****************************************************/
#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP

#include "msg.hpp"

namespace hummer
{

class CAssignment
{
public:
	CAssignment(): m_id(0), m_FiniFlag(false){}

	~CAssignment(){}

	void Assign(TID id, const string& split)
	{
		m_id = id;
		m_Split = split;
		m_FiniFlag = false;
	}

	const string& Split()
	{
		return m_Split;
	}

	bool IsFini()const
	{
		return m_FiniFlag;
	}

	void Finish()
	{
		m_FiniFlag = true;
	}

	void Clean()
	{
		m_id = 0;
		m_Split.clear();
		m_FiniFlag = false;
	}

	TID GetTid()const
	{
		return m_id;
	}
private:
	TID m_id;
	string m_Split;
	bool m_FiniFlag;
};

};

#endif

