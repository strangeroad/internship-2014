#ifndef _CFT_LOG_H_
#define _CFT_LOG_H_

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdarg.h>

using std::string;

const int ERR_SIZE = 1024;  // 错误信息长度
const int MAX_BUF = 10240; // 单行日志最大长度
const int SHIFT_FREQ = 16;  // 日志切换频度

/**
 * 日志类
 */
class CftLog
{
public:
    
    /**
     * 日志级别
     */
    enum LOG_LEVEL
    {
        _NONE = 1, 
        _ERROR = 2,  
        _WARNING = 3,  
        _NORMAL = 4, 
        _DEBUG = 5 
    };

    /**
     * 日志切换模式
     */
    enum SHIFT_MODE
    {
        _SIZE_MODE = 1,  // 按大小切换
        _DATE_MODE = 2  // 按日期切换
    };
    
public:
    
    /**
     * 构造函数
     */
    CftLog(const char* path, int max_size, int max_file = 10, SHIFT_MODE=_SIZE_MODE);

    /**
     * 析构函数
     */
    virtual ~CftLog();

    /**
     * 设置日志级别
     */
    inline void setLogLevel(LOG_LEVEL level)    { _level = level; }

    /**
     * 设置消息序列号
     */
    inline void setMsgId(const string& strMsg)      { _msg_id = strMsg; }

    /**
     * 设置文件名后缀
     */
    inline void setSuffix(const string& suffix) { _suffix = suffix; }

    /**
     * 获取日志系统错误信息
     */
    inline const char* getErrorInfo()    { return _szErrInfo; }

    /**
     * 打印错误log
     */
    int error(const char *fmt, ...);
    
    /**
     * 打印告警log
     */
    int warning(const char *fmt, ...);
    
    /**
     * 打印正常log
     */
    int normal(const char *fmt, ...);

    /**
     * 打印调试log
     */
    int debug(const char *fmt, ...);

    /**
     * 直接写日志
     */
    int raw(const char *fmt, ...);

protected:
    
    /**
     * 打开文件
     */
    int _open();

    /**
     * 关闭
     */
    void _close();

    /**
     * 切换日志文件
     */
    int _shift();

    /**
     * 记录日志
     */
    int _write(const char *szLog, int len);

    /**
     * 确定日志文件名
     */
    string _file_name(int index=0);

protected:

    /**
     * 切换模式
     */
    SHIFT_MODE _shift_mode;
    
    /**
     * 文件指针
     */
    int  _fd;

    /**
     * 文件路径
     */
    string _path;

    /**
     * 最大文件大小
     */
    int _max_size;

    /**
     * 进程ID
     */
    int _proc_id;

    /**
     * 当前系统时间
     */
    struct tm _tm_now;

    /**
     * 消息序列号
     */
    string _msg_id;

    /**
     * 文件名后缀
     */
    string _suffix;
    
    /**
     * 最大文件个数
     */
    int _max_file;

    /**
     * 日志级别
     */
    LOG_LEVEL _level;

    /**
     * 获取错误信息
     */
    char _szErrInfo[ERR_SIZE + 1];
};

#endif

