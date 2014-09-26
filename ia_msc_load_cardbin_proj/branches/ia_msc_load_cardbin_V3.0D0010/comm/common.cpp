#include "common.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include "md5.h"

extern CftLog* gPtrAppLog;
extern GlobalConfig* gPtrConfig; 

/**
 * 将YYYY-MM-DD HH:MM:SS转换为系统时间
 */
time_t toSysTime(const char* szTime)
{
    struct  tm tmTime;
    
    sscanf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", 
            &tmTime.tm_year, &tmTime.tm_mon, &tmTime.tm_mday, 
            &tmTime.tm_hour, &tmTime.tm_min, &tmTime.tm_sec);
    tmTime.tm_year -= 1900;
    tmTime.tm_mon -= 1;
    
    return mktime(&tmTime);
}

/**
 * 获取两个时间间隔秒数
 * @input:  szLTime       比较的时间基准YYYY-MM-DD HH:MM:SS
 *              szRTime      用于比较的时间YYYY-MM-DD HH:MM:SS
 * @return: szRTime-szLTime的秒数
 */
int getTimeInterval(const char* szLTime, const char* szRTime)
{
    return toSysTime(szRTime) - toSysTime(szLTime);
}

/**
 * TRPC 告警
 */
void alert(int e, const char* message)
{
    char szErrCode[16] = {0};
    char szErrMsg[256] = {0};
    
    sprintf(szErrCode, "%d", e);
    snprintf(szErrMsg, sizeof(szErrMsg), "%s", message);
    
    trpc_warning("ia_rate", szErrCode, szErrMsg);
    
    // 记录报警
    gPtrAppLog->warning("ALERT:%d,%s", e, message);
}



/**
 * 解密原始消息
 * @input: pRequest 消息请求句柄          
 * @output:  szSpId 商户SPID
 *          szBuf 解密后的消息字符串
 */
void getDecodeMsg(TRPC_SVCINFO* pRequst, char* szBuf, char* szSpId) throw (CException)
{    
    // 返回结果信息
    ST_PUB_ANS stAns;
    memset(&stAns, 0 ,sizeof(stAns));

    // 临时变量
    char szEncodeMsg[MAX_MSG_LEN] = {0};
    
    // 获取消息原始加密串
    CUrlAnalyze::getParam((char*)(pRequst->idata), "sp_id", szSpId, MAX_SP_LEN);
    CUrlAnalyze::getParam((char*)(pRequst->idata), "request_text", szEncodeMsg, MAX_MSG_LEN);

    // 解密消息数据
    decode(szSpId, szEncodeMsg, szBuf, stAns);
    if(stAns.iResult != 0)
    {
        throw CException(stAns.iResult, stAns.szErrInfo, __FILE__, __LINE__);
    }
}


/**
 * 判断是否是数字字符串
 */
bool isDigitString(const char *str)
{
    const char* p = str;

    // 略过前导空格
    while(isspace(*p))  p++;

    // 略过数字
    while(isdigit(*p))  p++;

    // 略过末尾空格
    while(isspace(*p))  p++;

    return !(*p);
}

/**
 * 将整数转换为字符串
 */
string toString(int value)
{
    char szTmp[128];
    snprintf(szTmp, sizeof(szTmp), "%d", value);
    return szTmp;
}

/**
 * 将整数转换为字符串
 */
string toString(LONG value)
{
    char szTmp[128];
    snprintf(szTmp, sizeof(szTmp), "%lli", value);
    return szTmp;
}

/**
 * 将字符串转换为整数
 */
int toInt(const char* value)
{
    return value ? atoi(value) : 0;
}

/**
 * 将字符串转换为长整数
 */
LONG toLong(const char* value)
{
    return value ? atoll(value) : 0;
}

/**
 * 将字符串转换为小写
 */
string& toLower(string& str)
{
    for(string::iterator it=str.begin(); it != str.end(); ++it)
    {
        *it = tolower(*it);
    }

    return str;
}

/**
 * 将字符串转换为小写
 */
char* toLower(char* sz)
{
    for(char* p=sz; *p; p++)
    {
        *p = tolower(*p);
    }
    return sz;
}

/**
 * 将字符串转换为大写
 */
string& toUpper(string& str)
{
    for(string::iterator it=str.begin(); it != str.end(); ++it)
    {
        *it = toupper(*it);
    }

    return str;
}

/**
 * 将字符串转换为大写
 */
char* toUpper(char* sz)
{
    for(char* p=sz; *p; p++)
    {
        *p = toupper(*p);
    }
    return sz;
}

/**
 * 获取当前主机IP
 */
string getLocalHostIp()
{
    int fd, intrface;
    long ip = -1;
    char szBuf[128] = {0};
    struct ifreq buf[16];
    struct ifconf ifc;

    if((fd=socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof buf;
        ifc.ifc_buf = (caddr_t) buf;
        if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc))
        {
            intrface = ifc.ifc_len / sizeof(struct ifreq);
            while(intrface-- > 0)
            {
                if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
                {
                    ip=inet_addr(inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
                    break;
                }
            }
        }
        close (fd);
    }

    // 转换为.格式
    unsigned char* pIp = (unsigned char*)(&ip);
    snprintf(szBuf, sizeof(szBuf), "%u.%u.%u.%u", pIp[0], pIp[1], pIp[2], pIp[3]);

    return szBuf;
}

/**
* 检查是否为正确的时间格式YYYY-MM-DD HH:MM:SS
*/
bool isValidDateTime(const string& strDateTime)
{
	if(strDateTime.length() != 19)  
    {
        return false;
    }
	
    int iYear = atoi(strDateTime.substr(0, 4).c_str());
	int iMonth = atoi(strDateTime.substr(5, 2).c_str());
	int iDay = atoi(strDateTime.substr(8, 2).c_str());
	int iHour = atoi(strDateTime.substr(11, 2).c_str());
	int iMinute = atoi(strDateTime.substr(14, 2).c_str());
	int iSecond = atoi(strDateTime.substr(17, 2).c_str());

	//检查分隔符
	if(strDateTime.substr(4, 1) != "-" || strDateTime.substr(7, 1) != "-"
		|| strDateTime.substr(10, 1) != " " || strDateTime.substr(13, 1) != ":"
		|| strDateTime.substr(16, 1) != ":")
	{
	    return false;
	}

	//检查具体时间
    if (!(iHour >= 0 && iHour <= 23))
    {
        return false;
    }
    if (!(iMinute >= 0 && iMinute <= 59))
    {
        return false;
    }
    if (!(iSecond >= 0 && iSecond <= 59))
    {
        return false;
    }
    
    if (iYear <= 1970) 
    {
        return false;
    }
    if (iMonth <= 0 || iMonth > 12)
    {
        return false;
    }
    if ((iDay <= 0) || (iDay > 31))
    {
        return false;
    }

    switch (iMonth)
    {
    case 2:
        if (((iYear % 4 == 0) && (iYear % 100 != 0)) || (iYear % 400 == 0))
        {
            if (iDay > 29)
            {
                return false;
            }
        }
        else if (iDay > 28)
        {
            return false;
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        if (iDay > 30)
        {
            return false;
        }
        break;
    default:
        if (iDay > 31)
        {
            return false;
        }
        break;
    }

    return true;
}


/**
 * 将时间转换为系统时间
 * @input       strTime     YYYY-MM-DD HH:MM:SS
 */
time_t toUnixTime(const string& strTime)
{
    // 取年、月、日段
    int year=0, month=0, day=0, hour=0, minute=0, second=0;
    sscanf(strTime.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &year, &month, &day, &hour, &minute, &second);

    // 若日期小于1900，返回0
    if(year < 1900)     return 0;

    // 转换为当地时间
    struct  tm tm_date;
    memset(&tm_date, 0, sizeof(tm));

    tm_date.tm_year =  year - 1900;
    tm_date.tm_mon = month - 1;
    tm_date.tm_mday = day;
    tm_date.tm_hour = hour;
    tm_date.tm_min = minute;
    tm_date.tm_sec = second;

    // 转换为系统时间
    return  mktime(&tm_date);
}

/**
 * @input YYYYMMDD
 * @output YYYY-MM-DD
 */
string toDate(const string& strDate)
{
    int year, month, day;
    sscanf(strDate.c_str(), "%04d%02d%02d", &year, &month, &day);
    
    char szTmp[11];
    sprintf(szTmp, "%04d-%02d-%02d", year, month, day);

    return szTmp;
}

/**
 * 获取系统时间: YYYY-MM-DD HH:MM:SS
 */
string getSysTime()
{
    return getSysTime(time(NULL));
}

/**
 * 获取系统时间: YYYY-MM-DD HH:MM:SS
 */
string getSysTime(time_t t)
{
    struct  tm tm_now;
    localtime_r(&t, &tm_now);

    char szTmp[256];
    snprintf(szTmp, sizeof(szTmp), "%04d-%02d-%02d %02d:%02d:%02d",
                tm_now.tm_year + 1900, tm_now.tm_mon + 1, tm_now.tm_mday,
                tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);

    return szTmp;
}

/**
 * 取时间的年部分
 *@input:   str   YYYY-MM-DD HH:MM:SS
 */
int year(const string& str)
{
    int year, month, day;
    sscanf(str.c_str(), "%04d-%02d-%02d", &year, &month, &day);
    return year;
}

/**
 * 取时间的月部分
 *@input:   str   YYYY-MM-DD HH:MM:SS
 */
int month(const string& str)
{
    int year, month, day;
    sscanf(str.c_str(), "%04d-%02d-%02d", &year, &month, &day);
    return month;
}

/**
 * 取时间的日部分
 *@input:   str   YYYY-MM-DD HH:MM:SS
 */
int day(const string& str)
{
    int year, month, day;
    sscanf(str.c_str(), "%04d-%02d-%02d", &year, &month, &day);
    return day;
}

/**
 * 取当前日期
 *@output:   string   YYYYMMDD
 */
string nowdate(const string& str)
{
    char szTmp[9];    //当天日期
    memset(szTmp, 0, sizeof(szTmp));

    int year, month, day;
    sscanf(str.c_str(), "%04d-%02d-%02d", &year, &month, &day);

    sprintf(szTmp, "%04d%02d%02d", year, month, day);

    return szTmp;
}

/**
 * 取上一月日期
 *@output:   string   YYYYMM
 */
string lastmonth(const string& str)
{
    char szTmp[9];    //当天日期
    int year, month;

    memset(szTmp, 0, sizeof(szTmp));

    year = atoi(str.substr(0, 4).c_str());
    month = atoi(str.substr(4, 2).c_str());

    month-=1;
    if (month == 0)
    {
        month = 12;
        year-=1;
    }

    sprintf(szTmp, "%04d%02d", year, month);
    return szTmp;
}

/**
 * 取下一月日期
 *@output:   string   YYYYMM
 */
string nextdate(const string& str)
{
    char szTmp[9];    //当天日期
    memset(szTmp, 0, sizeof(szTmp));

    int year, month;

    year = atoi(str.substr(0, 4).c_str());
    month = atoi(str.substr(4, 2).c_str());

    month = (month+1)%(12+1);
    if (month == 1)
    {
        year+=1;
    }

    sprintf(szTmp, "%04d%02d", year, month);
    return szTmp;
}

/**
 * 获取MD5摘要
 */
char* getMd5(const char *key, int len, char *szRes)
{
    struct MD5Context md5c;
    unsigned char digest[64];

    MD5Init(&md5c);
    MD5Update(&md5c, (unsigned char *) key, len);
    MD5Final(digest, &md5c);

    for (int i = 0; i < 16; i++)
    {
        sprintf(szRes + i * 2, "%02x", digest[i]);
    }

    szRes[32] = '\0';
    return szRes;
}

/**
 * 将字符串中的a字符替换为b字符
 */
char* replace(char* str, char a, char b)
{
    std::replace(str, str + strlen(str), a, b);

    return str;
}

/**
 * 检查是否为数字
 */
int isNumString(const char *str)
{
    const char * p = str;

    if (p == NULL)
    {
        return 0;
    }

    while (*p != '\0')
    {
        if (! isdigit(*p))
        {
            return 0;
        }
        p++;
    }

    return 1;
}

/**
 * 指针转换
 */
char *ValiStr(char *str)
{
        if (str == NULL)
                return "";
        else
                return str;
}

/**
 *字符串折成整数数组
 */
void split(vector <int > & list, const char * sp)
{
    char szSource[1024];
    snprintf(szSource, sizeof(szSource), "%s", sp);
    char * p = (char *)szSource;
    int iLen = strlen(sp);

    for(int iEnd = 0;iEnd < iLen; iEnd ++)
    {
        if(szSource[iEnd] == '|')
        {
            szSource[iEnd] = '\0';


            int itmp = atoi(p);
            if(itmp != 0)
            {
                list.push_back(itmp);
            }
            p = szSource + iEnd + 1;
        }
    }

    int itmp = atoi(p);
    if(itmp != 0)
    {
       list.push_back(itmp);
    }
    return ;
}

/**
 * 给字符串后缀一个下标
 */
string add_suffix(const char* name, int n)
{
    char szItem[128];
    snprintf(szItem, sizeof(szItem), "%s%d", name, n);

    return szItem;
}

/**
 * 将字符串去除空格
 */
string& strTrim(string& str)
{
    while (string::npos != str.find_first_of(' '))
    {
        str.erase(str.find_first_of(' '), 1);
    }

    return str;
}

/**
 * 将字符串去除空格特殊字符，只保留数字和字母
 */
string strTrimSpecial(const string& src)
{
    string result;
    string::const_iterator iter;

    for(iter = src.begin(); iter != src.end(); ++iter)
    {
        char ch = *iter;
        if ((ch >= '0' && ch <= '9') ||
            (ch >= 'A' && ch <= 'Z') ||
            (ch >= 'a' && ch <= 'z'))
        {
            result += ch;
        }
    }

    return result;
}


int monthInterval(const string& s_time , const string& e_time) 
{
	int s_year = year(s_time) ;
	int s_month = month(s_time) ;
	int e_year = year(e_time) ;
	int e_month = month(e_time) ;

	return 12 * (e_year - s_year) + (e_month - s_month) + 1 ;
}

string SubNumWithChar(const string & str,const int & iIndex,const int iCnt,const char & c)
{
	int iLen = str.length() ;
	if(iIndex + iCnt > iLen)
	{
		return "" ;
	}
	string result = str ;

	for(int iStart = iIndex ;iStart < iIndex + iCnt ; iStart++)
	{
		result[iStart] = c ;
	}

	return result ;
}


