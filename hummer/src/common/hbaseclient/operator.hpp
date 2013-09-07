#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include "hbase_types.h"
#include <boost/shared_ptr.hpp>
#include "msg.hpp"

#include <vector>
using std::vector;

using namespace apache::hadoop::hbase::thrift;

class CRowFilter
{
public:
	CRowFilter(){}
	virtual ~CRowFilter(){}

	virtual bool ColumnSet(vector<Text>& columns) = 0;
	virtual bool Filter(TRowResult& row, MsgPTR& out) = 0;
};

class CRowConvertor
{
public:
	CRowConvertor(){}
	virtual ~CRowConvertor(){}
	
	virtual bool RowConvert(TRowResult& in, MsgPTR& out);
};

typedef boost::shared_ptr<CRowFilter> FilterPTR;
typedef boost::shared_ptr<CRowConvertor> ConvertPTR;

#endif

