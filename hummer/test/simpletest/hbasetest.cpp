#include "test.hpp"
#include "utildef.hpp"
#include "protocol/TBinaryProtocol.h"
#include "protocol/TCompactProtocol.h"
#include "transport/TBufferTransports.h"
#include "transport/TTransportUtils.h"
#include "transport/TSocket.h"
#include <boost/shared_ptr.hpp>
#include "Hbase.h"

#include "Logger.h"
#include <vector>
#include <string>
#include <set>
using std::vector;
using std::string;
using std::set;

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::hadoop::hbase::thrift;
/*
using apache::thrift::protocol::TBinaryProtocol;
using apache::thrift::protocol::TCompactProtocol;
using apache::thrift::protocol::TProtocol;
using apache::thrift::transport::TFramedTransport;
using apache::thrift::transport::TSocket;
using apache::thrift::transport::TTransport;
using apache::thrift::TException;
*/
typedef struct hbase_test_s
{
	char* host;
	int port;
}hbase_test_t;

const char* image_table = "dc_image_info_pre";
const char* label_table = "dc_image_label_pre";
const char* product_table = "dc_product_info_pre";

int HBaseClientTest(void* arg)
{
	hbase_test_t* parm = (hbase_test_t*)arg;
	if (!parm)
	{
		HUMMER_ASSERT(false);
	}

	const string host = parm->host;
	const int port = parm->port;
	boost::shared_ptr<TSocket> socket(new TSocket(host, port));
	socket->setConnTimeout(1000);
	socket->setRecvTimeout(1000);
	socket->setSendTimeout(1000);

	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	if (!socket.get() || !transport.get() || !protocol.get())
	{
		HUMMER_ASSERT(false);
	}

	HbaseClient client(protocol);
	try
	{
		transport->open();
		set<Text> tableNameFind;
		vector<Text> tableNames;
		client.getTableNames(tableNames);
		if (tableNames.size() > 0)
		{
			vector<Text>::iterator tIter = tableNames.begin();
			for (; tIter != tableNames.end(); ++tIter)
			{
				printf("table name:%s\n", (*tIter).c_str());
				tableNameFind.insert(*tIter);
			}
		}

		string imageTable = image_table;
		string labelTable = label_table;
		string productTable = product_table;
		if (tableNameFind.end() == tableNameFind.find(imageTable)
			|| tableNameFind.end() == tableNameFind.find(labelTable)
			|| tableNameFind.end() == tableNameFind.find(productTable))
		{
			HUMMER_ASSERT(false);
		}

		//image table info
		Text imageReginStart;
		Text imageReginStop;
		vector<Text> imageColumns;

		vector<TRegionInfo> regionsInImage;
		map<Text, ColumnDescriptor> columnsInImage;
		client.getTableRegions(regionsInImage, imageTable);
		printf("Region num in image table,num=%d\n", regionsInImage.size());
		vector<TRegionInfo>::iterator irIter = regionsInImage.begin();
		for (; irIter != regionsInImage.end(); ++irIter)
		{
			imageReginStart = (*irIter).startKey;
			imageReginStop = (*irIter).endKey;
			printf("Region,id=%d,name=%s,start=%s,stop=%s\n"
				, (*irIter).id, (*irIter).name.c_str()
				, (*irIter).startKey.c_str(), (*irIter).endKey.c_str());	
		}

		client.getColumnDescriptors(columnsInImage, imageTable);
		map<Text, ColumnDescriptor>::iterator imageIter = columnsInImage.begin();
		for ( ; imageIter != columnsInImage.end(); ++imageIter)
		{
			imageColumns.push_back(imageIter->first);
			LOG_TRACE("image column key="<<imageIter->first<<",v_name="<<imageIter->second.name<<",v_ver="<<imageIter->second.maxVersions);
		}

		//image scanner
		ScannerID imageScan = client.scannerOpenWithStop(imageTable, imageReginStart, imageReginStop, imageColumns);
		vector<TRowResult> imageResult;
		client.scannerGetList(imageResult, imageScan, 100);
		printf("image scan, pre-get:100, actural:%u\n",imageResult.size());
		vector<TRowResult>::iterator resIter = imageResult.begin();
		for (; resIter != imageResult.end(); ++resIter)
		{
			map<Text, TCell>::iterator cellIter = (*resIter).columns.begin();
			for (; cellIter != (*resIter).columns.end(); ++cellIter)
			{
				printf("column,key=%s,value=%s\n"
					, cellIter->first.c_str(), cellIter->second.value.c_str());
			}			
		}


		// label table info
		vector<TRegionInfo> regionsInLabel;
		
		map<Text, ColumnDescriptor> columnsInLabel;
		client.getTableRegions(regionsInLabel, labelTable);
		printf("Region num in lable table, num=%d\n", regionsInLabel.size());
		vector<TRegionInfo>::iterator lrIter = regionsInLabel.begin();
		for (; lrIter != regionsInLabel.end(); ++lrIter)
		{
			printf("Region,id=%d,name=%s,start=%s,stop=%s\n"
				, (*lrIter).id, (*lrIter).name.c_str()
				, (*lrIter).startKey.c_str(), (*lrIter).endKey.c_str());	
		}

		client.getColumnDescriptors(columnsInLabel, labelTable);
		map<Text, ColumnDescriptor>::iterator labelIter = columnsInLabel.begin();
		for ( ; labelIter != columnsInLabel.end(); ++labelIter)
		{
			LOG_TRACE("label column key="<<labelIter->first<<",v_name="<<labelIter->second.name<<",v_ver="<<labelIter->second.maxVersions);
		}

		//product table info
		vector<TRegionInfo> regionsInProduct;
		map<Text, ColumnDescriptor> columnsInProduct;
		client.getTableRegions(regionsInProduct, productTable);
		printf("Region num in product table, num=%d\n", regionsInProduct.size());
		vector<TRegionInfo>::iterator prIter = regionsInProduct.begin();
		for (; prIter != regionsInProduct.end(); ++prIter)
		{
			printf("Region,id=%d,name=%s,start=%s,stop=%s\n"
				, (*prIter).id, (*prIter).name.c_str()
				, (*prIter).startKey.c_str(), (*prIter).endKey.c_str());
		}

		client.getColumnDescriptors(columnsInProduct, productTable);
		map<Text, ColumnDescriptor>::iterator proIter = columnsInProduct.begin();
		for ( ; proIter != columnsInProduct.end(); ++proIter)
		{
			LOG_TRACE("product column key="<<proIter->first<<",v_name="<<proIter->second.name<<",v_ver="<<proIter->second.maxVersions);
		}

		transport->close();
	}
	catch(TException &tx)
	{
		HUMMER_ASSERT(false);
	}

	return 0;
}

hbase_test_t hbase_parm = 
{
	"199.155.122.90", 9090
};

test_instance_t hbase_client_test = 
{
	"cluster_api_test", 
	100, 
	0, 
	{
		{0, HBaseClientTest, (void*)(&hbase_parm), "hbase api test1"},
		{END_TASK, NULL, NULL, NULL}
	}
};


