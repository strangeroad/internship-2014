#include <stdarg.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <time.h>
#include <errno.h>
#include "cftlog.h"

/**
 * 每次记录的日志缓存
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
 * 构造函数
 */
CftLog::CftLog(const char *path, int max_size, int max_num, SHIFT_MODE shft_mod)
{
    // 成员初始化
    _path = path;
    _max_size = max_size;
    _max_file = max_num;
    _level = _DEBUG;
    _shift_mode = shft_mod;
    _proc_id = getpid();
    _fd = -1;
}


/**
 * 析构函数
 */
CftLog::~CftLog()
{
    // 关闭文件
    _close();
}

/**
 * 打开文件
 */
int CftLog::_open()
{
    string strFileName = _file_name();
    
    // 先关闭文件
    _close();

    // 打开文件
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
 * 关闭文件
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
 * 打印错误log
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
 * 打印告警log
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
 * 打印正常log
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
 * 打印调试log
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
 * 直接写日志
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
 * 记录日志
 */
int CftLog::_write(const char *str, int len)
{  
    // 日志次数计数器
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
    
    // 是否要执行日志切换
    if((__count++) >= SHIFT_FREQ)
    {
       _shift();
       __count = 0;
    }
        
    return 0;
}

/**
 * 切换日志文件
 */
int CftLog::_shift()
{
    struct stat stStat;
    string strNewFile, strOldFile;
    
    // 重新打开文件
    _open();
    
    // 测试当前日志文件大小
    if(fstat(_fd, &stStat) < 0) 
    {
        snprintf(_szErrInfo, sizeof(_szErrInfo), "stat file %s.log error:%s", _path.c_str(), strerror(errno));
        return -1;
    }

    // 若当前文件大小小于最大值
    if (stStat.st_size < _max_size) 
    {
        return 0;
    }

    // 删除最后一个日志文件
    strNewFile = _file_name(_max_file - 1);
    if (access(strNewFile.c_str(), F_OK) == 0) 
    {
        if(remove(strNewFile.c_str()) != 0)  return -1;
    }

    // 累加文件序号(切换文件名)
    for(int i = _max_file - 2; i >= 0; i--) 
    {
        strOldFile = _file_name(i);
        strNewFile = _file_name(i+1);

        if(access(strOldFile.c_str(), F_OK) == 0) 
        {
            if(rename(strOldFile.c_str(), strNewFile.c_str()) != 0)  break;
        }
    }

    // 关闭文件
    _close();  
    
    return 0;
}

/**
 * 确定日志文件名
 * @input:  index  文件索引编号
 */
string CftLog::_file_name(int index)
{
    char szSuffix[128] = {0};
    char szFile[256] = {0};

    // 文件名后缀
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

