/**
  * FileName: exception.h
  * Author: Hawkliu
  * Version :1.0
  * Date: 2007-03-05
  * Description: 业务操作异常类的基类，从STL标准异常继承
  *                   而来，如果有新的需要，可以从该类继承。
  */
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <exception>
#include <iostream>

using std::string;
using std::ostream;

/**
 * 业务异常基类
 */
class CException : public std::exception
{
public:
    CException(int errNo, const char* szErrInfo, const char* szFile=NULL, const int iLine= 0) throw();
    CException(int errNo, const string& strErrInfo, const char* szFile=NULL, const int iLine= 0) throw();
    
    virtual ~CException() throw();

    const char* file() const throw(); // 取异常文件名
    int line() const throw(); // 取异常行号
    int error() const throw(); // 获取错误码
    virtual const char* what() const throw(); // 获取错误描述信息

protected:
    int  _errno;
    string _errinfo;
    string _file;
    int _line;
};

/**
 * 打印异常信息的操作
 */
inline ostream& operator << (ostream& os, const CException& e)
{
    os<<e.error()<<":"<<e.what();
    
    return os;
}

#endif

