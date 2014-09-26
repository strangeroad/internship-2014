#include    "sqlapi.h"
#include "cftlog.h"
#include "runinfo.h"

//#include    "sql_log.h"
//#include    "service_base.h"

/**
 * ȫ�ֱ�������
 */
extern CftLog* gPtrAppLog;

/**
 * Description: ���캯��
 * Input:         host         ���ݿ�����IP��ַ
 *                   user         ���ݿ��û�ID
 *                   passwd     ���ݿ��û�����
 *                   iOverTime ���ӳ�ʱʱ��
 *                  db_name   ��ʱֻ����־��������,����ӡʵ��db
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
 * Description: ���캯��
 * Input:         host         ���ݿ�����IP��ַ
 *                   user         ���ݿ��û�ID
 *                   passwd     ���ݿ��û�����
 *                   iOverTime ���ӳ�ʱʱ��
 */
CMySQL::~CMySQL()
{
    Close();

    // �����������
    TransEnd();
}

/**
 * Description: �������ݿ�
 * Return:       �����׳��쳣
 */
void  CMySQL::Connect() throw(CException)
{
    // ��ʼ�����Ӿ��
    if(m_SqlHandle == NULL)
    {
        m_SqlHandle =(MYSQL *)malloc(sizeof(MYSQL));
    }
    
    mysql_init(m_SqlHandle);

    // ��ʼ��ʧ��
    if (m_SqlHandle == NULL)
    {
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_INITIAL, "%s", "Failed to allocate mysql handle in mysql_init");
    }

    SetCharset();
    
    // ��������ѡ��
    mysql_options(m_SqlHandle, MYSQL_OPT_CONNECT_TIMEOUT,  (char *)&m_iOverTime);   // ��ʱʱ��
    //my_bool reconnect = 0;
    //mysql_options(m_SqlHandle, MYSQL_OPT_RECONNECT, &reconnect);	// ��ֹ�Զ�����
    //cout << "connect port:" << m_iPort << " host:" << m_strHost << " user:" << m_strUser << " pass:" << m_strPasswd << endl;
    if (!mysql_real_connect(m_SqlHandle, m_strHost.c_str(), m_strUser.c_str(), m_strPasswd.c_str(), NULL, m_iPort, NULL, 0))
    {
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_CONNECT,  "%s", mysql_error(m_SqlHandle));
    }

    // �������ӳɹ���־
    m_connFlag = true;
}


/**
 * Description: �ر����ݿ�
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
 * Description: ������ݿ�״̬
 * Input:         mysql     SQL�������
 */
bool CMySQL::Ping()
{
    return mysql_ping(m_SqlHandle) == 0;
}


/**
 * Description: ִ��SELECT���͵�SQL���
 * Input:         query     SQL���
 *                   length     SQL���ĳ���
 * Return:       ʧ���׳��쳣
 */
void CMySQL::Query(const char *query, unsigned int length) throw(CException)
{
    // �����ڶ���״̬�����Ҳ������������������
    if (!m_connFlag && !isTrans())
    {
        Close();
        Connect();
    }
	
    // ִ�в�ѯ
    __query(query, length);
}


void  CMySQL::Query(const std::string & query) throw(CException)
{
    Query(query.c_str(), query.length());
}

/**
 * Description: ִ������ʼ���
 * Return:       ʧ���׳��쳣
 */
void CMySQL::Begin() throw (CException)
{
    // �����ڶ���״̬�����������
    if (!m_connFlag)
    {
        Close();
        Connect();
    }

    if (isTrans())
    {
        // �ϸ�����δ����
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_DUP_TRANS, "%s", "Begin: last transaction unfinished ");
    }

    // ��ʼ����
    __query("BEGIN", sizeof("BEGIN"));

    TransBegin();
}


/**
 * Description: ִ�������ύ
 * Return:       ʧ���׳��쳣
 */
void CMySQL::Commit() throw (CException)
{
    if (!m_connFlag)
    {
        // commit��ʱ���������,��Ҫ����
        SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_CONNECT, "%s", "Commit: lost connection");
    }

    SureTrans();
    
	//gPtrAppLog->debug("Commit");
    // �ύ����
    __query("COMMIT", sizeof("COMMIT"));
    
    TransEnd();
	//PtrAppLog->debug("Commit end");
}

/**
 * Description: ִ������ع�
 * Return:      ʧ���׳��쳣
 */
void CMySQL::Rollback() throw (CException)
{
    // �����ڶ���״̬�����������
    if (!m_connFlag)
    {
        Close();
        Connect();
    }

    // ���ж��Ƿ���������,һ������catch�е��ô�����׳��쳣������
    // SureTrans();
        
	//gPtrAppLog->debug("Rollback");
    // �ع�����
    __query("ROLLBACK", sizeof("ROLLBACK"));

    TransEnd();
}

/**
 * Description: �ж����mysql�����Ƿ���������
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
        // δ��ʼ����
        SQL_THROW(0, ERR_DB_NO_TRANS, "not in trans");
    }
}

void CMySQL::SetCharset()
{
    mysql_options(m_SqlHandle, MYSQL_SET_CHARSET_NAME, m_strCharSet.c_str());
}

/**
 * Description: ��ȡSQLӰ�������
 * Return:      ʧ���׳��쳣
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
 * Description: ȡ�����
 * Return:      ʧ���׳��쳣
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


// �����Ϊ0������1�������������0
int  
CMySQL::FetchResultMap(SqlResultSet & objMap) throw(CException)
{
	// �����ǰmap
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

	// ƴ������
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

// ƴ������
string 
CMySQL::GetWhere(SqlResultSet & objWhereMap)
{
	// ƴ������
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

// ת��
string
CMySQL::EscapeStr(const string & buff)
{
    // �����ڶ���״̬�����Ҳ������������������
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

// ��ȡdb��table���
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
    // ��ӡsql��־
    //CSqlLog     sqllog(stmt_str, isTrans(), m_strHost, m_iPort, m_strDbName);
    struct timeval start, end;
    
    // ��ȡ������ʼʱ��
    gettimeofday(&start, NULL);   

    // ִ�в�ѯ
    if (mysql_real_query(m_SqlHandle, stmt_str, length)  !=  0 )
    {
    	//gPtrAppLog->debug("Err mysql_real_query SQL: %s errno: %d info: %s", query, mysql_errno(m_SqlHandle), mysql_error(m_SqlHandle));
		
        // �ж��Ƿ�Ϊ���ݿ����
        if (mysql_errno(m_SqlHandle) == CR_SERVER_LOST || mysql_errno(m_SqlHandle) == CR_SERVER_GONE_ERROR)
        {
            m_connFlag = false;

            //  ǿ�ƹرպ�����
            Close();
            Connect();

            // ���û��������,�������ظ�ִ�б���sql
            if (!isTrans())
            {
                if (mysql_real_query(m_SqlHandle, stmt_str, length) !=  0)
                {
                    //  �����˻�������Ļ�,ֱ�ӱ�������
                    SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_UNKNOW_FAULT,  "%s:%s", mysql_error(m_SqlHandle), stmt_str);
                }

                // �ظ�ִ�гɹ�
                return ;
            }
            else
            {
                SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_LOST,  "%s:%s",  mysql_error(m_SqlHandle), stmt_str);
            }
        }
        else
        {
            // ���ö������
            // m_connFlag = false;
            SQL_THROW(mysql_errno(m_SqlHandle), ERR_DB_UNKNOW_FAULT, "%s:%s", mysql_error(m_SqlHandle), stmt_str);
        }
    }
    
    // ��ȡ���ý���ʱ��
    gettimeofday(&end, NULL);   
    
    // ��¼��־
    gPtrAppLog->debug("|sql|%s|%s|%s|%s|3306|%dms|%d|%s|", 
                                    SRC_IP, HOST_IP, SERVICE, m_strHost.c_str(), 
                                    (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec)/1000, 
                                    0, stmt_str);

    return;
}

