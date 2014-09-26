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
 * 查询结果赋值的宏定义
 */
#define ICPY(A, B) A = ((B) ? atoi(B) : 0);
#define LCPY(A, B) A = ((B) ? atoll(B) : 0);
#define SCPY(A, B) strncpy(A, ((B) == NULL) ? "" : (B), sizeof(A) - 1)

// SPID最大长度
const int MAX_SP_LEN = 10;

// 消息最大长度
const int MAX_MSG_LEN = 1024 * 10;

// SQL最大长度
const int MAX_SQL_LEN = 1024 * 10;

// 参数值最大长度
const int MAX_PARAM_LEN = 1024 * 4;

// 整数最大值
const int MAX_INTEGER = std::numeric_limits<int>::max();
const int MIN_INTEGER = std::numeric_limits<int>::min();

// 长整数最大值
const LONG MAX_LONG = std::numeric_limits<LONG>::max();
const LONG MIN_LONG = std::numeric_limits<LONG>::min();

#define MAX_TRPC_LENGTH          10240 * 32

/**
 * 将YYYY-MM-DD HH:MM:SS转换为系统时间
 */
time_t toSysTime(const char* szTime);

/**
 * 获取两个时间间隔秒数
 */
int getTimeInterval(const char* szLTime, const char* szRTime);

/**
 * TRPC 告警
 */
void alert(int e, const char* message);

/**
 * 解密原始消息
 */
void getDecodeMsg(TRPC_SVCINFO* pRequst, char* szBuf, char* szSpId) throw (CException);


/**
 * 判断是否是数字字符串
 */
bool isDigitString(const char *str);

/**
 * 将整数转换为字符串
 */
string toString(int value);

/**
 * 将整数转换为字符串
 */
string toString(LONG value);

/**
 * 将字符串转换为整数
 */
int toInt(const char* value);

/**
 * 将字符串转换为长整数
 */
LONG toLong(const char* value);

/**
 * 将字符串转换为小写
 */
string& toLower(string& str);

/**
 * 将字符串转换为小写
 */
char* toLower(char* sz);

/**
 * 将字符串转换为大写
 */
string& toUpper(string& str);

/**
 * 将字符串转换为大写
 */
char* toUpper(char* sz);

/**
 * 获取当前主机IP
 */
string getLocalHostIp();

/**
* 检查是否为正确的时间格式YYYY-MM-DD HH:MM:SS
*/
bool isValidDateTime(const string& strDateTime);

/**
 * 将时间转换为系统时间
 */
time_t toUnixTime(const string& strTime);

/**
 * @input YYYYMMDD
 * @output YYYY-MM-DD
 */
string toDate(const string& strDate);

/**
 * 获取系统时间
 */
string getSysTime();

/**
 * 获取系统时间
 */
string getSysTime(time_t t);

/**
 * 取时间的年部分
 */
int year(const string& str);

/**
 * 取时间的月部分
 */
int month(const string& str);

/**
 * 取时间的日部分
 */
int day(const string& str);

/**
 * 取当前日期
 *@output:   string   YYYYMM
 */
string nowdate(const string& str);

/**
 * 取当前日期
 *@output:   string   YYYYMM
 */
string lastmonth(const string& str);

/**
 * 取下一月日期
 *@output:   string   YYYYMM
 */
string nextdate(const string& str);

/**
 * 获取MD5摘要
 */
char* getMd5(const char *key, int len, char *szRes);

/**
 * 检查是否为数字
 */
int isNumString(const char *str);

/**
 * 指针转换
 */
char *ValiStr(char *str);

/**
 *字符串折成整数数组
 */
void split(vector <int > & list, const char * sp);

/**
 * 给字符串后缀一个下标
 */
string add_suffix(const char* name, int n);

/**
 * 将字符串去除空格
 */
string& strTrim(string& str);

/**
 * 将字符串去除空格特殊字符，只保留数字和字母
 */
string strTrimSpecial(const string& src);

/**
 * 计算开始时间和结束时间月份间隔
 */
int monthInterval(const string& s_time , const string& e_time) ;


/**
 * TRPC 告警
 */
void alert(int e, const char* message);

//加掩码
string SubNumWithChar(const string & str,const int & iIndex,const int iCnt,const char & c);

#endif

