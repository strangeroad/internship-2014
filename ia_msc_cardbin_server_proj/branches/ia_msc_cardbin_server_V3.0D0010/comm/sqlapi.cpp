#include    "sqlapi.h"
#include "cftlog.h"
#include "runinfo.h"

//#include    "sql_log.h"
//#include    "service_base.h"

/**
 * 全局变量声明
 */
extern CftLog* gPtrAppLog;

/**
 * Description: 构造函数
 * Input:         host         数据库主机IP地址
 *                   user         数据库用户ID
 *                   passwd     数据库用户密码
 *                   iOverTime 连接超时时长
 *                  db_name   暂时只在日志里面提现,不打印实际db
 */
CMySQL::CMySQL(const char* host, const char* user, const char* passwd, const int port, int iOverTime, const char*  charset, const char * db_name)
    : 
    m_strHost(host), 
    m_iPort(port), 
    m_strUser(user), 
    m_strPasswd(passwd), 
    m_strDbName(db_name),
    m_SqlHandle(NULL),
    m_iOverTime(iOverTime),
    m_strCharSet(charset),
    m_connFlag(false),
    m_isTrans(false)
{
}

CMySQL::CMySQL(const std::string& host, const std::string& user, const std::string& passwd, const int port, int iOverTime, const std::string& charset, const std::string& db_name)
    : 
    m_strHost(host), 
    m_iPort(port), 
    m_strUser(user), 
    m_strPasswd(passwd), 
    m_strDbName(db_name),
    m_SqlHandle(NULL),
    m_iOverTime(iOverTime),
    m_strCharSet(charset),
    m_connFlag(false),
    m_isTrans(false)
{
}


/**
 * Description: 构造函数
 * Input:         host         数据库主机IP地址
 *                   user         数据库用户ID
 *                   passwd     数据库用户密码
 *                   iOverTime 连接超时时长
 */
CMySQL::~CMySQL()
{
    Close();

    // 设置事务结束
    TransEnd();
}

/**
 * Description: 连接数据库
 * Return:       出错抛出异常
 */
void  CMySQL::Connect() throw(CException)
{
    // 初始化连接句柄
    if(m_SqlHandle == NULL)
    {
        m_SqlHandle =(MYSQL *)malloc(sizeof(MYSQL));
    }
    
    mysql_init(m_SqlHandle);

    // 初始化失败
    if (m_SqlHandle == NULL)
    {
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_INITIAL, "%s", "Failed to allocate mysql handle in mysql_init");
    }

    SetCharset();
    
    // 设置连接选项
    mysql_options(m_SqlHandle, MYSQL_OPT_CONNECT_TIMEOUT,  (char *)&m_iOverTime);   // 超时时长
    //my_bool reconnect = 0;
    //mysql_options(m_SqlHandle, MYSQL_OPT_RECONNECT, &reconnect);	// 禁止自动重连
    //cout << "connect port:" << m_iPort << " host:" << m_strHost << " user:" << m_strUser << " pass:" << m_strPasswd << endl;
    if (!mysql_real_connect(m_SqlHandle, m_strHost.c_str(), m_strUser.c_str(), m_strPasswd.c_str(), NULL, m_iPort, NULL, 0))
    {
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_CONNECT,  "%s", mysql_error(m_SqlHandle));
    }

    // 设置连接成功标志
    m_connFlag = true;
}


/**
 * Description: 关闭数据库
 */
void  CMySQL::Close()
{
    if (m_SqlHandle) 
    { 
        mysql_close(m_SqlHandle);
        free(m_SqlHandle); 
        m_SqlHandle = NULL;
    }        

    m_connFlag = false;
}

/**
 * Description: 检测数据库状态
 * Input:         mysql     SQL操作句柄
 */
bool CMySQL::Ping()
{
    return mysql_ping(m_SqlHandle) == 0;
}


/**
 * Description: 执行SELECT类型的SQL语句
 * Input:         query     SQL语句
 *                   length     SQL语句的长度
 * Return:       失败抛出异常
 */
void CMySQL::Query(const char *query, unsigned int length) throw(CException)
{
    // 若处于断连状态，并且不再事务中则进行重连
    if (!m_connFlag && !isTrans())
    {
        Close();
        Connect();
    }
	
    // 执行查询
    __query(query, length);
}


void  CMySQL::Query(const std::string & query) throw(CException)
{
    Query(query.c_str(), query.length());
}

/**
 * Description: 执行事务开始语句
 * Return:       失败抛出异常
 */
void CMySQL::Begin() throw (CException)
{
    // 若处于断连状态，则进行重连
    if (!m_connFlag)
    {
        Close();
        Connect();
    }

    if (isTrans())
    {
        // 上个事务未结束
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_DUP_TRANS, "%s", "Begin: last transaction unfinished ");
    }

    // 开始事务
    __query("BEGIN", sizeof("BEGIN"));

    TransBegin();
}


/**
 * Description: 执行事务提交
 * Return:       失败抛出异常
 */
void CMySQL::Commit() throw (CException)
{
    if (!m_connFlag)
    {
        // commit的时候如果断连,需要报错
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_CONNECT, "%s", "Commit: lost connection");
    }

    SureTrans();
    
	//gPtrAppLog->debug("Commit");
    // 提交事务
    __query("COMMIT", sizeof("COMMIT"));
    
    TransEnd();
	//PtrAppLog->debug("Commit end");
}

/**
 * Description: 执行事务回滚
 * Return:      失败抛出异常
 */
void CMySQL::Rollback() throw (CException)
{
    // 若处于断连状态，则进行重连
    if (!m_connFlag)
    {
        Close();
        Connect();
    }

    // 不判断是否在事务中,一般是在catch中调用此语句抛出异常无意义
    // SureTrans();
        
	//gPtrAppLog->debug("Rollback");
    // 回滚事务
    __query("ROLLBACK", sizeof("ROLLBACK"));

    TransEnd();
}

/**
 * Description: 判断这个mysql链接是否在事务中
 * Return:      
 */
bool CMySQL::isTrans()
{
    return m_isTrans;
}

void CMySQL::SureTrans()
{
    if (!isTrans())
    {
        // 未开始事务
        SQL_THROW(0, ERR_DB_NO_TRANS, "not in trans");
    }
}

void CMySQL::SetCharset()
{
    mysql_options(m_SqlHandle, MYSQL_SET_CHARSET_NAME, m_strCharSet.c_str());
}

/**
 * Description: 获取SQL影响的行数
 * Return:      失败抛出异常
 */
unsigned long CMySQL::AffectedRows() throw(CException)
{
    my_ulonglong rownum = mysql_affected_rows(m_SqlHandle);
    if (rownum == (my_ulonglong) -1)
    {
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_AFFECT_ROW, "%s", mysql_error(m_SqlHandle));
    }

    return rownum;
}

/**
 * Description: 取结果集
 * Return:      失败抛出异常
 */
MYSQL_RES* CMySQL::FetchResult() throw(CException)
{
    MYSQL_RES* pRes = mysql_store_result(m_SqlHandle);
    if (pRes == NULL)
    {
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_NULL_RESULT, "%s", "Fetch result for non-select statement");
    }
    
    return pRes;
}

int 
CMySQL::FetchRows()
{
	MYSQL_RES* pRes = mysql_store_result(m_SqlHandle);
    if (pRes == NULL)
    {
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_NULL_RESULT, "%s", "Fetch result for non-select statement");
    }

	int iRet = mysql_num_rows(pRes);
	
	mysql_free_result(pRes);
	
	return iRet;
}


// 结果集为0，返回1，其他情况返回0
int  
CMySQL::FetchResultMap(SqlResultSet & objMap) throw(CException)
{
	// 清空以前map
	objMap.clear();
	
	//char szFiledValue[MAX_FILED_LENGTH] = {0};
	int iNumberFields;

	unsigned long *lengths;
	MYSQL_ROW row;
	MYSQL_FIELD * pField;
	
	MYSQL_RES* pRes = FetchResult();

   	if (pRes == NULL)
	{
   		throw CException(ERR_DB_NULL_RESULT, "Fetch result for non-select statement");
	}
        
	try
	{
		int iRetNum = mysql_num_rows(pRes);

		if(iRetNum == 0)
        {      
            mysql_free_result(pRes);
			return 1;
        }
	
		if(iRetNum != 1)
		{
			SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_MULTI_ROW, "mysql_num_rows is: %d", iRetNum);
		}

		lengths = mysql_fetch_lengths(pRes);
		iNumberFields = mysql_num_fields(pRes);
		row = mysql_fetch_row(pRes);
		pField = mysql_fetch_fields(pRes);

		//gPtrAppLog->debug("iNumberFields : %d", iNumberFields);
	
		for(int i = 0; i < iNumberFields; i++)
		{
			//gPtrAppLog->debug("i: %d,field: %s,   value: %s ", i, pField[i].name, ValiStr(row[i]));

			objMap[pField[i].name] = ValiStr(row[i]);
		}
	}
	catch(...)
	{
		mysql_free_result(pRes);
		throw;
	}

	mysql_free_result(pRes);
	
	return 0;
}

char *
CMySQL::ValiStr(char *str)
{
        if (str == NULL)
                return NSTR;
        else
                return str;
}

string 
CMySQL::GetSelectSQL(SqlResultSet & objMap, const string & strTableName, const string & strOther /* = "" */)
{
	stringstream ssSQL;

	ssSQL << " select * from "<< strTableName << " ";
	
	ssSQL << GetWhere(objMap);

	ssSQL << strOther;

	return ssSQL.str();
}

string
CMySQL::GetInsertSQL(SqlResultSet & objMap, const string & strTableName) throw(CException)
{
	if(objMap.empty())
		SQL_THROW(0, ERR_DB_SQL_NOVALUE, "%s", "GetInsertSQL ValueMap is Empty");

	stringstream ssSQL;

	ssSQL << " insert into " << strTableName << " ";

	stringstream strKey;
	stringstream strValue;

	SqlResultSet::iterator it = objMap.begin();

	strKey << it->first ;
	strValue << " '" << EscapeStr(it->second) << "' ";

	++it;

	for(;it != objMap.end(); ++it)
	{
		strKey << "," << it->first << " ";
		strValue << ", '" << EscapeStr(it->second) << "' ";
	}

	ssSQL << "( " << strKey.str() << ") values (" << strValue.str() << ") ";

	return ssSQL.str();
}

string
CMySQL::GetUpdateSQL(SqlResultSet & objValueMap, SqlResultSet & objWhereMap, const string & strTableName) throw(CException)
{
	stringstream ssSQL;

	ssSQL << " update " << strTableName << " set ";

	SqlResultSet::iterator it = objValueMap.begin();

	ssSQL << it->first;
	ssSQL << "= '" << EscapeStr(it->second) << "' ";

	++it;

	for(;it != objValueMap.end(); ++it)
	{
		ssSQL << " , " << it->first;
		ssSQL << "= '" << EscapeStr(it->second) << "' ";
	}

	// 拼接条件
	ssSQL << GetWhere(objWhereMap);

	return ssSQL.str();
}

string
CMySQL::GetReplaceSQL(SqlResultSet & objMap, const string & strTableName) throw(CException)
{
	if(objMap.empty())
		SQL_THROW(0, ERR_DB_SQL_NOVALUE, "%s", "GetReplaceSQL ValueMap is Empty");

	stringstream ssSQL;

	ssSQL << " replace into " << strTableName << " ";

	stringstream strKey;
	stringstream strValue;

	SqlResultSet::iterator it = objMap.begin();

	strKey << it->first ;
	strValue << " '" << EscapeStr(it->second) << "' ";

	++it;

	for(;it != objMap.end(); ++it)
	{
		strKey << "," << it->first << " ";
		strValue << ", '" << EscapeStr(it->second) << "' ";
	}

	ssSQL << "( " << strKey.str() << ") values (" << strValue.str() << ") ";

	return ssSQL.str();
}

// 拼接条件
string 
CMySQL::GetWhere(SqlResultSet & objWhereMap)
{
	// 拼接条件
	if(objWhereMap.empty())
		return "";

	stringstream ssSQL;
	
	ssSQL << " where ";

	SqlResultSet::iterator it = objWhereMap.begin();

	ssSQL << it->first << "='" << EscapeStr(it->second) << "' ";

	++it;

	for(;it != objWhereMap.end(); ++it)
	{
		ssSQL << " and " << it->first << "='" << EscapeStr(it->second) << "' ";
	}
	
	return ssSQL.str();
}

// 转义
string
CMySQL::EscapeStr(const string & buff)
{
    // 若处于断连状态，并且不再事务中则进行重连
    if (!m_connFlag && !isTrans())
    {
        Close();
        Connect();
    }
    
    int len = buff.length();
    char szTmp[len*2 + 1];
    memset(szTmp, 0, sizeof(szTmp));
    mysql_real_escape_string(m_SqlHandle,szTmp, buff.c_str(), buff.length());
    
    return string(szTmp);
}

int
CMySQL::Sdb1(int s)
{
        if (s < 1000) return -1;
        
        char buf[8], sz[16];
        bzero(buf, 8);
        bzero(sz, 16);

        sprintf(sz, "%d", s);
        memcpy(buf, sz+strlen(sz)-2, 2);
        return atoi(buf);
}

int
CMySQL::Sdb2(const char *s)
{
        if (s == NULL) return -1;
        
        char buf[8];
        bzero(buf, 8);
        //sprintf(buf, "%s", s[strlen(s)-2]);
        memcpy(buf, s+strlen(s)-2, 2);
        return atoi(buf);
}

int
CMySQL::Stb2(const char *s)
{
        if (s == NULL) return -1;

        char buf[8];
        bzero(buf, 8);
        memcpy(buf, s+strlen(s)-3, 1);
        return atoi(buf);
}

int
CMySQL::Stb1(int s)
{
        if (s < 1000) return -1;

        char buf[8], sz[16];
        bzero(buf, 8);
        bzero(sz, 16);

        sprintf(sz, "%d", s);
        memcpy(buf, sz+strlen(sz)-3, 1);
        return atoi(buf);
}

// 获取db和table编号
int 
CMySQL::GetDBAndTable(const char* szStr, int& iDbNum, int &iTbNum)
{
	iDbNum = Sdb2(szStr);
	iTbNum = Stb2(szStr);

	return 0;
}

// slow time
void CMySQL::SetSlowSqlTime(int time)
{
    //CSqlLog::SetSlowTime(time);
}

void CMySQL::__query(const char *stmt_str, unsigned long length)
{
    // 打印sql日志
    //CSqlLog     sqllog(stmt_str, isTrans(), m_strHost, m_iPort, m_strDbName);
    struct timeval start, end;
    
    // 获取调用起始时间
    gettimeofday(&start, NULL);   

    // 执行查询
    if (mysql_real_query(m_SqlHandle, stmt_str, length)  !=  0 )
    {
    	//gPtrAppLog->debug("Err mysql_real_query SQL: %s errno: %d info: %s", query, mysql_errno(m_SqlHandle), mysql_error(m_SqlHandle));
		
        // 判断是否为数据库断连
        if (mysql_errno(m_SqlHandle) == CR_SERVER_LOST || mysql_errno(m_SqlHandle) == CR_SERVER_GONE_ERROR)
        {
            m_connFlag = false;

            //  强制关闭后重连
            Close();
            Connect();

            // 如果没在事务中,重连后重复执行本条sql
            if (!isTrans())
            {
                if (mysql_real_query(m_SqlHandle, stmt_str, length) !=  0)
                {
                    //  重连了还有问题的话,直接报错算了
                    SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_UNKNOW_FAULT,  "%s:%s", mysql_error(m_SqlHandle), stmt_str);
                }

                // 重复执行成功
                return ;
            }
            else
            {
                SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_LOST,  "%s:%s",  mysql_error(m_SqlHandle), stmt_str);
            }
        }
        else
        {
            // 不置断连标记
            // m_connFlag = false;
            SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_UNKNOW_FAULT, "%s:%s", mysql_error(m_SqlHandle), stmt_str);
        }
    }
    
    // 获取调用结束时间
    gettimeofday(&end, NULL);   
    
    // 记录日志
    gPtrAppLog->debug("|sql|%s|%s|%s|%s|3306|%dms|%d|%s|", 
                                    SRC_IP, HOST_IP, SERVICE, m_strHost.c_str(), 
                                    (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000, 
                                    0, stmt_str);

    return;
}

