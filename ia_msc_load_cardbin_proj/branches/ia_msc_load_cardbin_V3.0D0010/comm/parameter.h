/**
  * FileName: parameter.h
  * Author: Hawkliu
  * Version :1.0
  * Date: 2007-09-07
  * Description: 本文件实现了一个读取输入参数的类
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
 * 读取输入参数的类
 */
class CParams
{
public:

    /**
     * 读取整数值(检查范围)
     */
    void readIntParam(const char* buf, const char* name, int min, int max) throw (CException);

    /**
     * 读取整数值(检查范围)
     */
    void readLongParam(const char* buf, const char* name, LONG min, LONG max) throw (CException);

    /**
     * 读取字符串参数值(检查范围)
     */
    void readStrParam(const char* buf, const char* name, int min_len, int max_len) throw (CException);

    /**
     * 判断参数是否存在
     */
    bool isExists(const char* name);

    /**
     * 取常量字符串值
     */
    const char* operator[](const char* name);

    /**
     * 取常量字符串值
     */
    const char* operator[](const string& name);
    
    /**
     * 获取字符串参数值
     */
    string getString(const char* name);

    /**
     * 获取整数参数值
     */
    int getInt(const char* name);

    /**
     * 获取大整数参数值
     */
    LONG getLong(const char* name);

    /**
     * 设置参数
     */
    void setParam(const char* name, const string& value);

    /**
     * 设置参数
     */
    void setParam(const char* name, const char* value);

    /**
     * 设置参数
     */
    void setParam(const char* name, int value);

    /**
     * 设置参数
     */
    void setParam(const char* name, LONG value);

public:
    /**
     * 参数属性值对的map
     */
    map<string, string>  mapAvps;
};


#endif
