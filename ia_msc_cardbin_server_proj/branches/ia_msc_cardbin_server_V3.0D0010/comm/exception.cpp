#include "exception.h"

/**
 * 异常类的构造函数
 * @input:  errNum       错误码
 *              szErrInfo     错误描述信息
 *              szFile    异常出现的文件名
 *              iLine     抛出异常的行号
 */
CException::CException(int errNum, const char* szErrInfo, const char* szFile, const int iLine) throw() 
                       : _errno(errNum), _errinfo(szErrInfo), _file(szFile ? szFile : ""), _line(iLine)
{
}

/**
 * 异常类的构造函数
 * @input:  errNum       错误码
 *              strErrInfo    错误描述信息
 *              szFile    异常出现的文件名
 *              iLine     抛出异常的行号
 */
CException::CException(int errNo, const string& strErrInfo, const char* szFile, const int iLine) throw()
                        : _errno(errNo), _errinfo(strErrInfo), _file(szFile ? szFile : ""), _line(iLine)
{
}

/**
 * 异常类的析构函数
 */
CException::~CException() throw()
{
}

/**
 * 获取异常的错误码
 * Return: 错误码
 */
int CException::error() const throw()
{
    return _errno;
}

/**
 * 获取异常错误信息
 * Return: 错误描述信息
 */
const char* CException::what() const throw()
{
    return _errinfo.c_str();
}

/**
 * 获取异常的文件名
 * Return: 异常文件名
 */
const char* CException::file() const throw()
{
    return _file.c_str();
}

/**
 * 获取异常的行号
 * Return: 取异常行号
 */
int CException::line() const throw()
{
    return _line;
}

