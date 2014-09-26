#include <stdarg.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <time.h>
#include <errno.h>
#include "cftlog.h"

/**
 * ÿ�μ�¼����־����
 */
#define WRITE_LOG(buf, str)\
    int iBufLen = 0;\
    time_t tnow = time(NULL);\
    localtime_r(&tnow, &_tm_now);\
    iBufLen = snprintf(buf, sizeof(buf), \
    "[%04d-%02d-%02d %02d:%02d:%02d][%d][%s][%s]",\
            _tm_now.tm_year + 1900, _tm_now.tm_mon + 1,\
            _tm_now.tm_mday, _tm_now.tm_hour, _tm_now.tm_min, _tm_now.tm_sec,\
            _proc_id, str, _msg_id.c_str());\
\
    va_list ap;\
    va_start(ap, fmt);\
    iBufLen += vsnprintf(buf + iBufLen, sizeof(buf) - iBufLen, fmt, ap);\
    va_end(ap);\
    buf[iBufLen++] = '\n';\
    buf[iBufLen] = '\0';\
    return _write(buf, iBufLen);

/**
 * ���캯��
 */
CftLog::CftLog(const char *path, int max_size, int max_num, SHIFT_MODE shft_mod)
{
    // ��Ա��ʼ��
    _path = path;
    _max_size = max_size;
    _max_file = max_num;
    _level = _DEBUG;
    _shift_mode = shft_mod;
    _proc_id = getpid();
    _fd = -1;
}


/**
 * ��������
 */
CftLog::~CftLog()
{
    // �ر��ļ�
    _close();
}

/**
 * ���ļ�
 */
int CftLog::_open()
{
    string strFileName = _file_name();
    
    // �ȹر��ļ�
    _close();

    // ���ļ�
    if ((_fd = open(strFileName.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644)) < 0) 
    {
        snprintf(_szErrInfo, sizeof(_szErrInfo), "open %s error:%s", strFileName.c_str(), strerror(errno));
        fprintf(stderr, "%s", _szErrInfo);
        _fd = -1;
        return -1;
    }

    return 0;
}

/**
 * �ر��ļ�
 */
void CftLog::_close()
{
    if (_fd != -1) 
    {
        close(_fd);
        _fd = -1;
    }
}

/**
 * ��ӡ����log
 */
int CftLog::error(const char *fmt, ...)
{
    char buf[MAX_BUF];
    
    if (_level < _ERROR) 
    {
        return 0;
    }

    WRITE_LOG(buf, "ERROR");
}

/**
 * ��ӡ�澯log
 */
int CftLog::warning(const char *fmt, ...)
{
    char buf[MAX_BUF];
    
    if (_level < _WARNING) 
    {
        return 0;
    }

    WRITE_LOG(buf, "WARNING");
}

/**
 * ��ӡ����log
 */
int CftLog::normal(const char *fmt, ...)
{
    char buf[MAX_BUF];
    
    if (_level < _NORMAL) 
    {
        return 0;
    }
    
    WRITE_LOG(buf, "NORMAL");
}

/**
 * ��ӡ����log
 */
int CftLog::debug(const char *fmt, ...)
{
    char buf[MAX_BUF];
    
    if (_level < _DEBUG) 
    {
        return 0;
    }
    WRITE_LOG(buf, "DEBUG");
}

/**
 * ֱ��д��־
 */
int CftLog::raw(const char *fmt, ...)
{
    char buf[MAX_BUF];
    int iBufLen = 0;
    
    va_list ap;
    va_start(ap, fmt);
    iBufLen += vsnprintf(buf + iBufLen, sizeof(buf) - iBufLen, fmt, ap);
    va_end(ap);
    
    return _write(buf, iBufLen);
}

/**
 * ��¼��־
 */
int CftLog::_write(const char *str, int len)
{  
    // ��־����������
    static int __count = 0;

    if (_fd == -1) 
    {
        if (_open() != 0) 
        {
            snprintf(_szErrInfo, sizeof(_szErrInfo), "open %s.log error:%s", _path.c_str(), strerror(errno));
            return -1;
        }
    }

    int ret = write(_fd, str, len);
    if (ret < 0) 
    {
        _close();
        snprintf(_szErrInfo, sizeof(_szErrInfo), "puts %s.log error:%s", _path.c_str(), strerror(errno));
        return ret;
    }
    
    // �Ƿ�Ҫִ����־�л�
    if((__count++) >= SHIFT_FREQ)
    {
       _shift();
       __count = 0;
    }
        
    return 0;
}

/**
 * �л���־�ļ�
 */
int CftLog::_shift()
{
    struct stat stStat;
    string strNewFile, strOldFile;
    
    // ���´��ļ�
    _open();
    
    // ���Ե�ǰ��־�ļ���С
    if(fstat(_fd, &stStat) < 0) 
    {
        snprintf(_szErrInfo, sizeof(_szErrInfo), "stat file %s.log error:%s", _path.c_str(), strerror(errno));
        return -1;
    }

    // ����ǰ�ļ���СС�����ֵ
    if (stStat.st_size < _max_size) 
    {
        return 0;
    }

    // ɾ�����һ����־�ļ�
    strNewFile = _file_name(_max_file - 1);
    if (access(strNewFile.c_str(), F_OK) == 0) 
    {
        if(remove(strNewFile.c_str()) != 0)  return -1;
    }

    // �ۼ��ļ����(�л��ļ���)
    for(int i = _max_file - 2; i >= 0; i--) 
    {
        strOldFile = _file_name(i);
        strNewFile = _file_name(i+1);

        if(access(strOldFile.c_str(), F_OK) == 0) 
        {
            if(rename(strOldFile.c_str(), strNewFile.c_str()) != 0)  break;
        }
    }

    // �ر��ļ�
    _close();  
    
    return 0;
}

/**
 * ȷ����־�ļ���
 * @input:  index  �ļ��������
 */
string CftLog::_file_name(int index)
{
    char szSuffix[128] = {0};
    char szFile[256] = {0};

    // �ļ�����׺
    if(index == 0)
    {
        snprintf(szSuffix, sizeof(szSuffix), "%s.log", _suffix.c_str());
    }
    else
    {
        snprintf(szSuffix, sizeof(szSuffix), "%s%d.log", _suffix.c_str(), index);
    }

    if(_shift_mode == _DATE_MODE)
    {    
        snprintf(szFile, sizeof(szFile), "%s_%04d%02d%02d%s", _path.c_str(), _tm_now.tm_year + 1900, _tm_now.tm_mon + 1,  _tm_now.tm_mday, szSuffix);
    }
    else
    {
        snprintf(szFile, sizeof(szFile), "%s%s", _path.c_str(), szSuffix);
    }

    return szFile;
}

