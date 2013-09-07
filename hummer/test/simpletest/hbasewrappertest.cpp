#include "test.hpp"

#include "utildef.hpp"
#include "hbasewrapper.h"
#include "Logger.h"

#include <set>
using std::set;

typedef struct hbasewrapper_test_s
{
	const char* table;
	char* host;
	int port;
}hbasewrapper_test_t;

const char* image_table_ = "dc_image_info_pre";
const char* label_table_ = "dc_image_label_pre";
const char* product_table_ = "dc_product_info_pre";

int HBaseWrapperTest(void* arg)
{
	hbasewrapper_test_t* parm = (hbasewrapper_test_t*)arg;
	if (!parm)
	{
		HUMMER_ASSERT(false);
	}
	
	string table = parm->table;
	
	CHbaseConnection conn;
	conn.Init(parm->host, parm->port, 1000);
	if (!conn.Run())
	{
		HUMMER_ASSERT(false);
	}
	vector<CRegionSplit> lableSplits;
	conn.GetRegionSplit(table, lableSplits);
	printf("Split num=%d\n", lableSplits.size());

	//create scan
	vector<ScannerID> scans;
	vector<CRegionSplit>::iterator sIter = lableSplits.begin();	
	//for (;sIter != lableSplits.end(); ++sIter)
	//{
		ScannerID id;
		if (conn.CreateRegionScanner(id, *sIter))
		{
			scans.push_back(id);
		}
		else
		{
			HUMMER_ASSERT(false);
		}
	//}
	printf("Scanner num=%d\n", scans.size());
	//scan data
	vector<string> rowlist;
	set<string> columnlist;
	vector<ScannerID>::iterator dataIter = scans.begin();
	for (; dataIter != scans.end(); ++dataIter)
	{
		vector<TRowResult> rFamiliy;
		conn.Next(*dataIter, 3, rFamiliy);
		HUMMER_ASSERT(rFamiliy.size() == 3);

		vector<TRowResult>::iterator rfIter = rFamiliy.begin();
		for (; rfIter != rFamiliy.end(); ++rfIter)
		{
			LOG_TRACE("column size="<<(*rfIter).columns.size());
			map<Text, TCell>::iterator rowfIter = (*rfIter).columns.begin();
			for (; rowfIter != (*rfIter).columns.end(); ++rowfIter)
			{
				LOG_TRACE("Row:"<<(*rfIter).row<<",column:"<<rowfIter->first<<",cell:"<<rowfIter->second.value);
				columnlist.insert(rowfIter->first);	
			}
			rowlist.push_back((*rfIter).row);
		}

		conn.DestoryRegionScanner(*dataIter);
	}

	//get row test:
	vector<string>::iterator rowIter = rowlist.begin();
	for (; rowIter != rowlist.end(); ++rowIter)
	{
		vector<TRowResult> rowDatas;
		conn.GetRow(table, *rowIter, rowDatas);
		vector<TRowResult>::iterator rdIter = rowDatas.begin();
		for (; rdIter != rowDatas.end(); ++rdIter)
		{
			map<Text, TCell>::iterator rowfIter = (*rdIter).columns.begin();
			for (; rowfIter != (*rdIter).columns.end(); ++rowfIter)
			{
				LOG_TRACE("getRow Row:"<<(*rdIter).row<<",column:"<<rowfIter->first<<",cell:"<<rowfIter->second.value);	
			}
		}	
	}

	vector<TRowResult> batchRowDatas;
	conn.GetRows(table, rowlist, batchRowDatas);
	vector<TRowResult>::iterator batchIter = batchRowDatas.begin();
	for (; batchIter != batchRowDatas.end(); ++batchIter)
	{
		map<Text, TCell>::iterator rowfIter = (*batchIter).columns.begin();
		for (; rowfIter != (*batchIter).columns.end(); ++rowfIter)
		{
			LOG_TRACE("batchGetRow Row:"<<(*batchIter).row<<",column:"<<rowfIter->first<<",cell:"<<rowfIter->second.value);	
		}
	}

	set<string>::iterator columnIter = columnlist.begin();
	for (; columnIter != columnlist.end(); ++columnIter)
	{
		vector<TCell> cellDatas;
		vector<string>::iterator rowIter =  rowlist.begin();
		for (; rowIter != rowlist.end(); ++rowIter)
		{
			conn.GetCell(table, *rowIter, *columnIter, cellDatas);
			vector<TCell>::iterator cellIter = cellDatas.begin();
			for (; cellIter != cellDatas.end(); ++cellIter)
			{
				LOG_TRACE("Getcell,row:"<<*rowIter<<",column:"<<*columnIter<<",cell:"<<(*cellIter).value);	
			}
		}
	}
	conn.UnInit();
	return 0;
}

hbasewrapper_test_t lable_parm = 
{
	label_table_, "199.155.122.90", 9090
};

hbasewrapper_test_t image_parm = 
{
	image_table_, "199.155.122.90", 9090
};

hbasewrapper_test_t product_parm = 
{
	product_table_, "199.155.122.90", 9090
};

test_instance_t hbase_wrapper_test = 
{
	"hbase_wrapper_test", 
	100, 
	0, 
	{
		{1, HBaseWrapperTest, (void*)(&lable_parm), "hbase wrapper test1"},
		{0, HBaseWrapperTest, (void*)(&image_parm), "hbase wrapper test1"},
		{0, HBaseWrapperTest, (void*)(&product_parm), "hbase wrapper test1"},
		{END_TASK, NULL, NULL, NULL}
	}
};


