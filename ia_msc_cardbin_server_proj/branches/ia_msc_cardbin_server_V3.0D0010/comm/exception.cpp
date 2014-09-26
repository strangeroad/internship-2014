#include "exception.h"

/**
 * �쳣��Ĺ��캯��
 * @input:  errNum       ������
 *              szErrInfo     ����������Ϣ
 *              szFile    �쳣���ֵ��ļ���
 *              iLine     �׳��쳣���к�
 */
CException::CException(int errNum, const char* szErrInfo, const char* szFile, const int iLine) throw() 
                       : _errno(errNum), _errinfo(szErrInfo), _file(szFile ? szFile : ""), _line(iLine)
{
}

/**
 * �쳣��Ĺ��캯��
 * @input:  errNum       ������
 *              strErrInfo    ����������Ϣ
 *              szFile    �쳣���ֵ��ļ���
 *              iLine     �׳��쳣���к�
 */
CException::CException(int errNo, const string& strErrInfo, const char* szFile, const int iLine) throw()
                        : _errno(errNo), _errinfo(strErrInfo), _file(szFile ? szFile : ""), _line(iLine)
{
}

/**
 * �쳣�����������
 */
CException::~CException() throw()
{
}

/**
 * ��ȡ�쳣�Ĵ�����
 * Return: ������
 */
int CException::error() const throw()
{
    return _errno;
}

/**
 * ��ȡ�쳣������Ϣ
 * Return: ����������Ϣ
 */
const char* CException::what() const throw()
{
    return _errinfo.c_str();
}

/**
 * ��ȡ�쳣���ļ���
 * Return: �쳣�ļ���
 */
const char* CException::file() const throw()
{
    return _file.c_str();
}

/**
 * ��ȡ�쳣���к�
 * Return: ȡ�쳣�к�
 */
int CException::line() const throw()
{
    return _line;
}

