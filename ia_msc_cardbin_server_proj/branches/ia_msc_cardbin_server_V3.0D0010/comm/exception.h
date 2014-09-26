/**
  * FileName: exception.h
  * Author: Hawkliu
  * Version :1.0
  * Date: 2007-03-05
  * Description: ҵ������쳣��Ļ��࣬��STL��׼�쳣�̳�
  *                   ������������µ���Ҫ�����ԴӸ���̳С�
  */
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <exception>
#include <iostream>

using std::string;
using std::ostream;

/**
 * ҵ���쳣����
 */
class CException : public std::exception
{
public:
    CException(int errNo, const char* szErrInfo, const char* szFile=NULL, const int iLine= 0) throw();
    CException(int errNo, const string& strErrInfo, const char* szFile=NULL, const int iLine= 0) throw();
    
    virtual ~CException() throw();

    const char* file() const throw(); // ȡ�쳣�ļ���
    int line() const throw(); // ȡ�쳣�к�
    int error() const throw(); // ��ȡ������
    virtual const char* what() const throw(); // ��ȡ����������Ϣ

protected:
    int  _errno;
    string _errinfo;
    string _file;
    int _line;
};

/**
 * ��ӡ�쳣��Ϣ�Ĳ���
 */
inline ostream& operator << (ostream& os, const CException& e)
{
    os<<e.error()<<":"<<e.what();
    
    return os;
}

#endif

