#ifndef _COMMON_H_
#define _COMMON_H_

#include <string>
#include <vector>
#include "error.h"
#include <map>
#include <limits>
#include "sqlapi.h"
#include "trpcwrapper.h"
#include "cftlog.h"
#include "globalconfig.h"
#include "parameter.h"
#include "runinfo.h"
#include <sstream>
#include  "tinystr.h"
#include  "tinyxml.h"
#include "decode.h"



using std::map;
using std::string;
using std::vector;
using std::stringstream;


/**
 * ��ѯ�����ֵ�ĺ궨��
 */
#define ICPY(A, B) A = ((B) ? atoi(B) : 0);
#define LCPY(A, B) A = ((B) ? atoll(B) : 0);
#define SCPY(A, B) strncpy(A, ((B) == NULL) ? "" : (B), sizeof(A) - 1)

// SPID��󳤶�
const int MAX_SP_LEN = 10;

// ��Ϣ��󳤶�
const int MAX_MSG_LEN = 1024 * 10;

// SQL��󳤶�
const int MAX_SQL_LEN = 1024 * 10;

// ����ֵ��󳤶�
const int MAX_PARAM_LEN = 1024 * 4;

// �������ֵ
const int MAX_INTEGER = std::numeric_limits<int>::max();
const int MIN_INTEGER = std::numeric_limits<int>::min();

// ���������ֵ
const LONG MAX_LONG = std::numeric_limits<LONG>::max();
const LONG MIN_LONG = std::numeric_limits<LONG>::min();

#define MAX_TRPC_LENGTH          10240 * 32

/**
 * ��YYYY-MM-DD HH:MM:SSת��Ϊϵͳʱ��
 */
time_t toSysTime(const char* szTime);

/**
 * ��ȡ����ʱ��������
 */
int getTimeInterval(const char* szLTime, const char* szRTime);

/**
 * TRPC �澯
 */
void alert(int e, const char* message);

/**
 * ����ԭʼ��Ϣ
 */
void getDecodeMsg(TRPC_SVCINFO* pRequst, char* szBuf, char* szSpId) throw (CException);


/**
 * �ж��Ƿ��������ַ���
 */
bool isDigitString(const char *str);

/**
 * ������ת��Ϊ�ַ���
 */
string toString(int value);

/**
 * ������ת��Ϊ�ַ���
 */
string toString(LONG value);

/**
 * ���ַ���ת��Ϊ����
 */
int toInt(const char* value);

/**
 * ���ַ���ת��Ϊ������
 */
LONG toLong(const char* value);

/**
 * ���ַ���ת��ΪСд
 */
string& toLower(string& str);

/**
 * ���ַ���ת��ΪСд
 */
char* toLower(char* sz);

/**
 * ���ַ���ת��Ϊ��д
 */
string& toUpper(string& str);

/**
 * ���ַ���ת��Ϊ��д
 */
char* toUpper(char* sz);

/**
 * ��ȡ��ǰ����IP
 */
string getLocalHostIp();

/**
* ����Ƿ�Ϊ��ȷ��ʱ���ʽYYYY-MM-DD HH:MM:SS
*/
bool isValidDateTime(const string& strDateTime);

/**
 * ��ʱ��ת��Ϊϵͳʱ��
 */
time_t toUnixTime(const string& strTime);

/**
 * @input YYYYMMDD
 * @output YYYY-MM-DD
 */
string toDate(const string& strDate);

/**
 * ��ȡϵͳʱ��
 */
string getSysTime();

/**
 * ��ȡϵͳʱ��
 */
string getSysTime(time_t t);

/**
 * ȡʱ����겿��
 */
int year(const string& str);

/**
 * ȡʱ����²���
 */
int month(const string& str);

/**
 * ȡʱ����ղ���
 */
int day(const string& str);

/**
 * ȡ��ǰ����
 *@output:   string   YYYYMM
 */
string nowdate(const string& str);

/**
 * ȡ��ǰ����
 *@output:   string   YYYYMM
 */
string lastmonth(const string& str);

/**
 * ȡ��һ������
 *@output:   string   YYYYMM
 */
string nextdate(const string& str);

/**
 * ��ȡMD5ժҪ
 */
char* getMd5(const char *key, int len, char *szRes);

/**
 * ����Ƿ�Ϊ����
 */
int isNumString(const char *str);

/**
 * ָ��ת��
 */
char *ValiStr(char *str);

/**
 *�ַ����۳���������
 */
void split(vector <int > & list, const char * sp);

/**
 * ���ַ�����׺һ���±�
 */
string add_suffix(const char* name, int n);

/**
 * ���ַ���ȥ���ո�
 */
string& strTrim(string& str);

/**
 * ���ַ���ȥ���ո������ַ���ֻ�������ֺ���ĸ
 */
string strTrimSpecial(const string& src);

/**
 * ���㿪ʼʱ��ͽ���ʱ���·ݼ��
 */
int monthInterval(const string& s_time , const string& e_time) ;


/**
 * TRPC �澯
 */
void alert(int e, const char* message);

//������
string SubNumWithChar(const string & str,const int & iIndex,const int iCnt,const char & c);

#endif

