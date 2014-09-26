/**
  * FileName: sqlapi.h 
  * Author: Created by verminniu 
  * History: 
  */
#ifndef _SQL_API_H_
#define _SQL_API_H_

#include    <mysql.h>
#include    <errmsg.h>
#include    <mysqld_error.h>

#include    "error.h"
#include    "exception.h"

#include    <string>
#include    <map>
#include    <sstream>
#include    <iostream>
using namespace std;

typedef    map<string, string>   SqlResultSet;

static const int MAX_FILED_LENGTH = 32000;
#define	NSTR	""


#define SQL_THROW(SqlError, errNo, ...) {\
            char msg[4096]; \
            snprintf(msg, 4096, __VA_ARGS__ );\
            throw CSqlException((SqlError), errNo, msg, __FILE__, __LINE__);}

class CSqlException: public CException
{
public:
    CSqlException(int mysql_error, int errNo, const std::string& errInfo, const char* file, const int line)
        :
        CException(errNo, errInfo, file, line),
        __mysql_error(mysql_error)
        {
        }
     ~CSqlException() throw()
        {
        }

public:
    int sql_error()
        {
            return __mysql_error;
        }
    
private:
    int __mysql_error;
};

/**
  * �����MySQL�Ľӿڽ����˼򵥷�װ����Ҫ�������¹���:
  *     1���ڲ�������������Ϣ
  *     2�����ݿ��Զ���������
  *     3����SELECT�����ȡ������׳��쳣
  *     4����ȡSQLӰ����������ʱ�׳��쳣
  */
class CMySQL
{
public:
    CMySQL(const char* host , const char* user , const char* pswd , const int port = 3306, int iOverTime = 5, const char*  charset = "latin1", const char * db_name = "");
	CMySQL(const std::string& host, const std::string& user, const std::string& pswd, const int port = 3306, int iOverTime = 5, const std::string& charset = "latin1", const std::string& db_name = "");
    
	~CMySQL();
    operator MYSQL*() { return m_SqlHandle; }
    MYSQL* GetHandle() { return m_SqlHandle; }
    
    /**
     * ���ݿ�����
     */
    void Connect() throw(CException);
    void Close();
    bool Ping(); 

    /**
     * ִ��Query
     */
    void  Query(const char *query, unsigned int length) throw(CException);
    void  Query(const std::string & query) throw(CException);
    unsigned long AffectedRows() throw(CException);
    MYSQL_RES* FetchResult() throw(CException);

    /**
     * ȡ���������map��ʽ
     * 
     */
    int  FetchResultMap(SqlResultSet & objMap) throw(CException);

    // ���ز�ѯ������
    int  FetchRows();
	
    /**
     * ���������
     */
    void Begin() throw(CException);
    void Commit() throw(CException);
    void Rollback() throw(CException);
    // �ж��Ƿ���������
    bool isTrans();
    void TransBegin()
        {
            m_isTrans = true;
        }
    void TransEnd()
        {
            m_isTrans = false;
        }
    // ȷ�ϱ����������з����׳��쳣
    void SureTrans();

    void SetCharset();

    string GetReplaceSQL(SqlResultSet & objMap, const string & strTableName) throw(CException);
    string GetSelectSQL(SqlResultSet & objMap, const string & strTableName, const string & strOther = "");
    string GetInsertSQL(SqlResultSet & objMap, const string & strTableName) throw(CException);
    string GetUpdateSQL(SqlResultSet & objValueMap, SqlResultSet & objWhereMap, const string & strTableName) throw(CException);
    string GetWhere(SqlResultSet & objWhereMap);
    
    // ת��
    string EscapeStr(const string & buff);

public:
    // ��̬�������
    

    static char *ValiStr(char *str);

	static int Sdb1(int s);

	static int Stb1(int s);

    static int Sdb2(const char *s);

    static int Stb2(const char *s);

    static int GetDBAndTable(const char* szStr, int& iDbNum, int &iTbNum);
    
    // slow time
    static void SetSlowSqlTime(int time);

protected:
    void __query(const char *stmt_str, unsigned long length);
    
private:
    // ��ֹ�����͸�ֵ����
    CMySQL(const CMySQL& ref);
    CMySQL& operator=(const CMySQL& ref);

public:
    static const bool QUERY_WITH_LOCK    = true;
    static const bool QUERY_WITHOUT_LOCK = false;
    
    static const bool QUERY_EXIST        = true;
    static const bool QUERY_NOT_EXIST    = false;

protected:    
    std::string m_strHost;          // ��¼����IP
    int         m_iPort;
    std::string m_strUser;          // ��¼ID
    std::string m_strPasswd;      // ��¼����
    std::string m_strDbName;
    
    MYSQL*      m_SqlHandle;       // SQL�������
    unsigned int m_iOverTime;    // ���ӳ�ʱʱ��
    std::string m_strCharSet;
    
    bool    m_connFlag;         // ���ݿ�����״̬��־
    bool    m_isTrans;          // �Ƿ���������
};

#endif

