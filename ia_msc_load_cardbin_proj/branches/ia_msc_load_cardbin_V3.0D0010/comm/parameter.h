/**
  * FileName: parameter.h
  * Author: Hawkliu
  * Version :1.0
  * Date: 2007-09-07
  * Description: ���ļ�ʵ����һ����ȡ�����������
  */
#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include <map>
#include "exception.h"
#include "error.h"
#include "UrlAnalyze.h"
#include "common.h"

using std::map;
using std::string;
using CFT::CUrlAnalyze;


/**
 * ��ȡ�����������
 */
class CParams
{
public:

    /**
     * ��ȡ����ֵ(��鷶Χ)
     */
    void readIntParam(const char* buf, const char* name, int min, int max) throw (CException);

    /**
     * ��ȡ����ֵ(��鷶Χ)
     */
    void readLongParam(const char* buf, const char* name, LONG min, LONG max) throw (CException);

    /**
     * ��ȡ�ַ�������ֵ(��鷶Χ)
     */
    void readStrParam(const char* buf, const char* name, int min_len, int max_len) throw (CException);

    /**
     * �жϲ����Ƿ����
     */
    bool isExists(const char* name);

    /**
     * ȡ�����ַ���ֵ
     */
    const char* operator[](const char* name);

    /**
     * ȡ�����ַ���ֵ
     */
    const char* operator[](const string& name);
    
    /**
     * ��ȡ�ַ�������ֵ
     */
    string getString(const char* name);

    /**
     * ��ȡ��������ֵ
     */
    int getInt(const char* name);

    /**
     * ��ȡ����������ֵ
     */
    LONG getLong(const char* name);

    /**
     * ���ò���
     */
    void setParam(const char* name, const string& value);

    /**
     * ���ò���
     */
    void setParam(const char* name, const char* value);

    /**
     * ���ò���
     */
    void setParam(const char* name, int value);

    /**
     * ���ò���
     */
    void setParam(const char* name, LONG value);

public:
    /**
     * ��������ֵ�Ե�map
     */
    map<string, string>  mapAvps;
};


#endif
