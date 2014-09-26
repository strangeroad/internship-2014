#include "appcomm.h"
#include "decode.h"
#include "globalconfig.h"
#include "bill_no.h"
#include <math.h>

/**
 * 全局变量声明
 */
extern CftLog* gPtrAppLog;
extern GlobalConfig* gPtrConfig; 




/**
 * 对字符串进行SQL转义
 * @input:  str  待转义字符串
 * @return: 转移后字符串
 */
string escapeString(const string& str)
{
    static char szTmp[4096 + 1];
    memset(szTmp, 0, sizeof(szTmp));
    mysql_escape_string(szTmp, str.c_str(), str.length());

    return szTmp;
}

