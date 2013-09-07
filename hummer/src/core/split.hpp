/*****************************************************
 * Copyright zhengyang   
 * FileName: split.hpp 
 * Description: the definition of split
 * Author: zhengyang
 * Date: 2013.08.03
 * *****************************************************/
#ifndef SPLIT_HPP
#define SPLIT_HPP

#include <boost/shared_ptr.hpp>

#include <string>
using std::string;

namespace hummer
{

class CSplit
{
public:
	enum SplitType
	{
		SPLIT_INVALID = -1,
		SPLIT_DEF = 0,
		SPLIT_PRE,
		SPLIT_MAP,
		SPLIT_MAX
	};

	CSplit(): m_Type(SPLIT_DEF){}
	virtual ~CSplit(){}
	void Init(SplitType type, const string& id, const string& text)
	{
		m_Type = type;
		m_id = id;
		m_text = text;
	}

	void Clean()
	{
		m_id.clear();
		m_text.clear();
	}

	const string& GetText()const
	{
		return m_text;
	}

	const string& GetId()const
	{
		return m_id;
	}

	SplitType GetType()const
	{
		return m_Type;
	}
private:
	string m_id;
	string m_text;
	SplitType m_Type;	
};

};

typedef boost::shared_ptr<hummer::CSplit> SplitPTR;

#endif 

