#include "hbasewrapper.h"
#include "Logger.h"

CHbaseConnection::CHbaseConnection(){}

CHbaseConnection::~CHbaseConnection(){}

void CHbaseConnection::Init(const string& host, int port, int timeout_ms)
{
	m_Host = host;
	m_Port = port;
	m_Timeout = timeout_ms;
	LOG_TRACE("hbase session,host="<<m_Host<<",port="<<m_Port<<",timeout="<<m_Timeout);
	return;
}

void CHbaseConnection::UnInit()
{
	LOG_TRACE("hbase session stop");
	if (!m_Transport.get())
	{
		return;
	}

	try
	{
		m_Transport->close();
	}
	catch(TException &tx)
	{
		LOG_ERROR("Catch thrift hbase exception");
	}
}

bool CHbaseConnection::Run()
{
	try
	{
		boost::shared_ptr<TSocket> socket(new TSocket(m_Host, m_Port));
		socket->setConnTimeout(m_Timeout);
		socket->setRecvTimeout(m_Timeout);
		socket->setSendTimeout(m_Timeout);

		boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
		boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
		boost::shared_ptr<HbaseClient> clientptr(new HbaseClient(protocol));
		if (!socket.get() 
			|| !transport.get() 
			|| !protocol.get()
			|| !clientptr.get())
		{
			LOG_ERROR("create object failed");		
			return false;
		}
		
		transport->open();
		m_Transport = transport;
		m_Client = clientptr;
	}
	catch(TException &tx)
	{
		LOG_ERROR("Catch thrift hbase exception");
		return false;
	}

	return true;
}

void CHbaseConnection::GetRegionSplit(const string& table, vector<CRegionSplit>& splits)
{
	if (!m_Client.get())
	{
		LOG_ERROR("hbase client is null");
		return;
	}

	try
	{
		vector<Text> families;
		map<Text, ColumnDescriptor> columnFamilies;
		m_Client->getColumnDescriptors(columnFamilies, table);
		map<Text, ColumnDescriptor>::iterator cfIter = columnFamilies.begin();
		for ( ; cfIter != columnFamilies.end(); ++cfIter)
		{
			//LOG_TRACE("column family:"<<cfIter->first);
			families.push_back(cfIter->first);
		}
		//LOG_TRACE("family size="<<families.size()<<", cF:"<<columnFamilies.size());

		vector<TRegionInfo> regions;
		m_Client->getTableRegions(regions, table);
		vector<TRegionInfo>::iterator tIter = regions.begin();
		for (; tIter != regions.end(); ++tIter)
		{
			CRegionSplit split;
			split.Setup(table, (*tIter).id, (*tIter).startKey
				, (*tIter).endKey, families);
			//split.Print();	
			splits.push_back(split);
			//split.Print();	
		}
		LOG_TRACE("region info:"<<regions.size()<<",split num="<<splits.size());
	}
	catch(TException &tx)
	{
		LOG_ERROR("get split exception");
	}
}

bool CHbaseConnection::CreateRegionScanner(ScannerID& id, CRegionSplit& rs)
{
	if (!m_Client.get())
	{
		return false;
	}

	try
	{
		LOG_TRACE("RegionScanner,table="<<rs.Table()<<",ks="<<rs.KeyStart()<<",ke="<<rs.KeyEnd()<<",fn="<<rs.GetFamilys().size());
		id = m_Client->scannerOpenWithStop(rs.Table()
			, rs.KeyStart(), rs.KeyEnd(), rs.GetFamilys());
		LOG_TRACE("RegionScanner, id="<<id);
	}
	catch(TException &tx)
	{
		LOG_ERROR("hbase exception");
		return false;
	}
	return true;
}

bool CHbaseConnection::CreateRegionScanner(ScannerID& id
			, CRegionSplit& rs, vector<Text>& columns)
{
	if (!m_Client.get())
	{
		return false;
	}

	try
	{
		id = m_Client->scannerOpenWithStop(rs.Table()
				, rs.KeyStart(), rs.KeyEnd(), columns);
		LOG_TRACE("RegionScanner,id="<<id<<","<<rs.KeyStart()<<":"<<rs.KeyEnd()<<",table="<<rs.Table());
	}
	catch(TException &tx)
	{
		LOG_ERROR("hbase exception");
		return false;
	}
	return true;
}

void CHbaseConnection::DestoryRegionScanner(ScannerID id)
{
	if (!m_Client.get())
	{
		return;
	}

	try
	{
		m_Client->scannerClose(id);
	}
	catch(TException &tx)
	{
		return;
	}
}

int CHbaseConnection::Next(ScannerID id
		, vector<TRowResult>& results)
{
	if (!m_Client.get())
	{
		LOG_TRACE("Client is null");
		return SCAN_ERR;
	}

	try
	{
		m_Client->scannerGet(results, id);
	}
	catch(TException &tx)
	{
		return SCAN_EXCEPTION;
	}
	return  SCAN_OK;
}

int CHbaseConnection::Next(ScannerID id, int32_t rowNum, vector<TRowResult>& results)
{
	if (!m_Client.get())
	{
		LOG_TRACE("Client is null");
		return SCAN_ERR;
	}

	try
	{
		m_Client->scannerGetList(results, id, rowNum);
	}
	catch(TException &tx)
	{
		LOG_ERROR("exception");
		return SCAN_EXCEPTION;
	}

	return SCAN_OK;
}

void CHbaseConnection::GetRow(const string& table, const string& rkey, vector<TRowResult>& results)
{
	if (!m_Client.get())
	{
		return;
	}

	try
	{
		m_Client->getRow(results, table, rkey);
	}
	catch(TException &tx)
	{
		return;		
	}
}

void CHbaseConnection::GetCell(const string& table
		, const string& rkey, const string& ckey
		, vector<TCell>& results)
{
	if (!m_Client.get())
	{
		return;
	}

	try
	{
		m_Client->get(results, table, rkey, ckey);
	}
	catch(TException &tx)
	{
		return;
	}
}

void CHbaseConnection::GetRows(const string& table
		, const vector<string>& rows, vector<TRowResult>& results)
{
	if (!m_Client.get())
	{
		return;
	}

	try
	{
		m_Client->getRows(results, table, rows);
	}
	catch(TException &tx)
	{
		return;
	}
} 

/*****************CRegionSplit*****************/
CRegionSplit::CRegionSplit():m_id(0){}

CRegionSplit::~CRegionSplit(){}

const CRegionSplit& CRegionSplit::operator=(const CRegionSplit& split)
{
	m_id = split.m_id;
	m_KeyStart = split.m_KeyStart;
	m_KeyStop = split.m_KeyStop;
	m_Table = split.m_Table;
	m_ColumnFamilys = split.m_ColumnFamilys;
	return *this;
}

void CRegionSplit::Setup(const string& table, int64_t id
		, const string& keyStart, const string& keyend
		, vector<Text>& families)
{
	m_id = id;
	m_KeyStart = keyStart;
	m_KeyStop = keyend;
	m_ColumnFamilys = families;
	m_Table = table;
	//Print();
	return;
}

void CRegionSplit::Clean()
{
	m_id = 0;
	m_KeyStart.clear();
	m_KeyStop.clear();
	m_ColumnFamilys.clear();
}

void CRegionSplit::Format(const string& id, const string& Text)
{
	(void)id;
	(void)Text;
}

bool CRegionSplit::Parse(const string& id, const string& Text)
{
	(void)id;
	(void)Text;
	return false;
}
	
const string& CRegionSplit::Table()const
{
	return m_Table;
}

int64_t CRegionSplit::ID()
{
	return m_id;
}

const string& CRegionSplit::KeyStart()const
{
	return m_KeyStart;
}

const string& CRegionSplit::KeyEnd()const
{
	return m_KeyStop;
}

const vector<Text>& CRegionSplit::GetFamilys()const
{
	return m_ColumnFamilys;
}

void CRegionSplit::Print()
{
	LOG_TRACE("SPLIT INFO:id="<<m_id<<",kS="<<m_KeyStart<<",kE="<<m_KeyStop<<",table="<<m_Table<<",cfn="<<m_ColumnFamilys.size());
	vector<Text>::iterator cIter = m_ColumnFamilys.begin();
	for (; cIter != m_ColumnFamilys.end(); ++cIter)
	{
		LOG_TRACE("Split column family:"<<(*cIter));
	}
}



