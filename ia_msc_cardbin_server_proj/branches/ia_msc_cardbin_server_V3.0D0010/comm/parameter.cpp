#include "parameter.h"

/**
 * ��ȡ����ֵ(��鷶Χ)
 */
void CParams::readIntParam(const char* buf, const char* name, int min, int max) throw (CException)
{
    char value[MAX_PARAM_LEN + 1] = {0};

    // ��ȡ����
    CUrlAnalyze::getParam(const_cast<char*>(buf), name, value, MAX_PARAM_LEN);   

    // �ַ������
    if(!isDigitString(value))
    {
        throw CException(ERR_BAD_PARAM, string("Param is not a int value:") + name + "=" + value, __FILE__, __LINE__);
    }

    // ��Χ���
    int iTmp = atoi(value);
    if (iTmp < min || iTmp > max)
    {
        throw CException(ERR_BAD_PARAM, string("Param is out of range:") + name + "=" + value, __FILE__, __LINE__);
    }

    mapAvps[name] = value;
}

/**
 * ��ȡ����ֵ(��鷶Χ)
 */
void CParams::readLongParam(const char* buf, const char* name, LONG min, LONG max) throw (CException)
{
    char value[MAX_PARAM_LEN + 1] = {0};

    // ��ȡ����
    CUrlAnalyze::getParam(const_cast<char*>(buf), name, value, MAX_PARAM_LEN);   

    // �ַ������
    if(!isDigitString(value))
    {
        throw CException(ERR_BAD_PARAM, string("Param is not a int value:") + name + "=" + value, __FILE__, __LINE__);
    }
    
    // ��Χ���
    LONG lTmp = atoll(value);
    if (lTmp < min || lTmp > max)
    {
        throw CException(ERR_BAD_PARAM, string("Param is out of range:") + name + "=" + value, __FILE__, __LINE__);
    }

    mapAvps[name] = value;
}


/**
 * ��ȡ�ַ�������ֵ
 */
void CParams::readStrParam(const char* buf, const char* name, int min_len, int max_len) throw (CException)
{
    char value[MAX_PARAM_LEN + 1] = {0};

    CUrlAnalyze::getParam(const_cast<char*>(buf), name, value, MAX_PARAM_LEN);   

    // ���ȼ��
    int iLen = strlen(value);
    
    if (iLen < min_len || iLen > max_len)
    {
        throw CException(ERR_BAD_PARAM, string("Param's length is out of range:") + name + "=" + value, __FILE__, __LINE__);
    }

    mapAvps[name] = value;
}

/**
 * �жϲ����Ƿ����
 */
bool CParams::isExists(const char* name)
{
    return mapAvps.find(name) != mapAvps.end();
}

/**
 * ��ȡ�ַ�������
 */
string CParams::getString(const char* name)
{
    map<string, string>::iterator it = mapAvps.find(name);

    return it == mapAvps.end() ? "" : it->second;
}

/**
 * ȡ�����ַ���ֵ
 */
const char* CParams::operator[](const char* name)
{
    map<string, string>::iterator it = mapAvps.find(name);

    return it == mapAvps.end() ? "" : it->second.c_str();
}

/**
 * ȡ�����ַ���ֵ
 */
const char* CParams::operator[](const string& name)
{
    map<string, string>::iterator it = mapAvps.find(name);

    return it == mapAvps.end() ? "" : it->second.c_str();
}
    
/**
 * ��ȡ��������
 */
int CParams::getInt(const char* name)
{
    map<string, string>::iterator it = mapAvps.find(name);
    
    return it == mapAvps.end() ? 0 : atoi(it->second.c_str());
}

/**
 * ��ȡ����������
 */
LONG CParams::getLong(const char* name)
{
    map<string, string>::iterator it = mapAvps.find(name);
    
    return it == mapAvps.end() ? 0 : atoll(it->second.c_str());
}

/**
 * ���ò���
 */
void CParams::setParam(const char* name, const string& value)
{
    mapAvps[name] = value;
}

/**
 * ���ò���
 */
void CParams::setParam(const char* name, const char* value)
{
    mapAvps[name] = value;
}

/**
 * ���ò���
 */
void CParams::setParam(const char* name, int value)
{
    char szTmp[128];
    snprintf(szTmp, sizeof(szTmp), "%d", value);
    mapAvps[name] = szTmp;
}

/**
 * ���ò���
 */
void CParams::setParam(const char* name, LONG value)
{
    char szTmp[128];
    snprintf(szTmp, sizeof(szTmp), "%lli", value);
    mapAvps[name] = szTmp;
}


