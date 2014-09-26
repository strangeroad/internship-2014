#include "parameter.h"

/**
 * 读取整数值(检查范围)
 */
void CParams::readIntParam(const char* buf, const char* name, int min, int max) throw (CException)
{
    char value[MAX_PARAM_LEN + 1] = {0};

    // 提取参数
    CUrlAnalyze::getParam(const_cast<char*>(buf), name, value, MAX_PARAM_LEN);   

    // 字符串检查
    if(!isDigitString(value))
    {
        throw CException(ERR_BAD_PARAM, string("Param is not a int value:") + name + "=" + value, __FILE__, __LINE__);
    }

    // 范围检查
    int iTmp = atoi(value);
    if (iTmp < min || iTmp > max)
    {
        throw CException(ERR_BAD_PARAM, string("Param is out of range:") + name + "=" + value, __FILE__, __LINE__);
    }

    mapAvps[name] = value;
}

/**
 * 读取整数值(检查范围)
 */
void CParams::readLongParam(const char* buf, const char* name, LONG min, LONG max) throw (CException)
{
    char value[MAX_PARAM_LEN + 1] = {0};

    // 提取参数
    CUrlAnalyze::getParam(const_cast<char*>(buf), name, value, MAX_PARAM_LEN);   

    // 字符串检查
    if(!isDigitString(value))
    {
        throw CException(ERR_BAD_PARAM, string("Param is not a int value:") + name + "=" + value, __FILE__, __LINE__);
    }
    
    // 范围检查
    LONG lTmp = atoll(value);
    if (lTmp < min || lTmp > max)
    {
        throw CException(ERR_BAD_PARAM, string("Param is out of range:") + name + "=" + value, __FILE__, __LINE__);
    }

    mapAvps[name] = value;
}


/**
 * 读取字符串参数值
 */
void CParams::readStrParam(const char* buf, const char* name, int min_len, int max_len) throw (CException)
{
    char value[MAX_PARAM_LEN + 1] = {0};

    CUrlAnalyze::getParam(const_cast<char*>(buf), name, value, MAX_PARAM_LEN);   

    // 长度检查
    int iLen = strlen(value);
    
    if (iLen < min_len || iLen > max_len)
    {
        throw CException(ERR_BAD_PARAM, string("Param's length is out of range:") + name + "=" + value, __FILE__, __LINE__);
    }

    mapAvps[name] = value;
}

/**
 * 判断参数是否存在
 */
bool CParams::isExists(const char* name)
{
    return mapAvps.find(name) != mapAvps.end();
}

/**
 * 获取字符串参数
 */
string CParams::getString(const char* name)
{
    map<string, string>::iterator it = mapAvps.find(name);

    return it == mapAvps.end() ? "" : it->second;
}

/**
 * 取常量字符串值
 */
const char* CParams::operator[](const char* name)
{
    map<string, string>::iterator it = mapAvps.find(name);

    return it == mapAvps.end() ? "" : it->second.c_str();
}

/**
 * 取常量字符串值
 */
const char* CParams::operator[](const string& name)
{
    map<string, string>::iterator it = mapAvps.find(name);

    return it == mapAvps.end() ? "" : it->second.c_str();
}
    
/**
 * 获取整数参数
 */
int CParams::getInt(const char* name)
{
    map<string, string>::iterator it = mapAvps.find(name);
    
    return it == mapAvps.end() ? 0 : atoi(it->second.c_str());
}

/**
 * 获取大整数参数
 */
LONG CParams::getLong(const char* name)
{
    map<string, string>::iterator it = mapAvps.find(name);
    
    return it == mapAvps.end() ? 0 : atoll(it->second.c_str());
}

/**
 * 设置参数
 */
void CParams::setParam(const char* name, const string& value)
{
    mapAvps[name] = value;
}

/**
 * 设置参数
 */
void CParams::setParam(const char* name, const char* value)
{
    mapAvps[name] = value;
}

/**
 * 设置参数
 */
void CParams::setParam(const char* name, int value)
{
    char szTmp[128];
    snprintf(szTmp, sizeof(szTmp), "%d", value);
    mapAvps[name] = szTmp;
}

/**
 * 设置参数
 */
void CParams::setParam(const char* name, LONG value)
{
    char szTmp[128];
    snprintf(szTmp, sizeof(szTmp), "%lli", value);
    mapAvps[name] = szTmp;
}


