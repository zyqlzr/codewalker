#ifndef HBASE_WRAPPER_H
#define HBASE_WRAPPER_H
#include "protocol/TBinaryProtocol.h"
#include "protocol/TCompactProtocol.h"
#include "transport/TBufferTransports.h"
#include "transport/TTransportUtils.h"
#include "transport/TSocket.h"

#include "Hbase.h"
#include "hbase_types.h"
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;
using namespace apache::hadoop::hbase::thrift;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

enum ScanCode
{
	SCAN_EXCEPTION = -1,
	SCAN_OK = 0,
	SCAN_ERR
};

class CRegionSplit
{
public:
	CRegionSplit();
	~CRegionSplit();
	const CRegionSplit& operator=(const CRegionSplit& split);
	void Setup(const string& table, int64_t id
			, const string& keyStart, const string& keyend
			, vector<Text>& families);
	void Clean();
	void Format(const string& id, const string& Text);
	bool Parse(const string& id, const string& Text);
	
	const string& Table()const;
	int64_t ID();
	const string& KeyStart()const;
	const string& KeyEnd()const;
	const vector<Text>& GetFamilys()const;
	void Print();
private:
	int64_t m_id;
	string m_KeyStart;
	string m_KeyStop;
	string m_Table;
	vector<Text> m_ColumnFamilys;
};

class CHbaseConnection
{
public:
	CHbaseConnection();
	~CHbaseConnection();

	void Init(const string& host, int port, int timeout_ms);
	void UnInit();
	bool Run();

	void GetRegionSplit(const string& table, vector<CRegionSplit>& splits);

	bool CreateRegionScanner(ScannerID& id, CRegionSplit& region);
	bool CreateRegionScanner(ScannerID& id
			, CRegionSplit& region, vector<Text>& columns);
	void DestoryRegionScanner(ScannerID id);
	int Next(ScannerID id, int32_t num, vector<TRowResult>& results);
	int Next(ScannerID id, vector<TRowResult>& results);
	void GetRow(const string& table, const string& rkey
			, vector<TRowResult>& results);
	void GetCell(const string& table, const string& rkey
			, const string& ckey, vector<TCell>& results);
	void GetRows(const string& table, const vector<string>& rows
			, vector<TRowResult>& results);
private:
	string m_Host;
	int m_Port;
	int m_Timeout;
	boost::shared_ptr<TTransport> m_Transport;
	boost::shared_ptr<HbaseClient> m_Client;	
};


#endif

