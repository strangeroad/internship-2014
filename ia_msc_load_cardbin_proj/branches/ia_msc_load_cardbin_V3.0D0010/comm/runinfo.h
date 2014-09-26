#ifndef _SER_RUN_INFO_H_
#define _SER_RUN_INFO_H_

#include <sys/time.h>
#include <vector>
#include "exception.h"
#include "error.h"
#include "trpc_service.h"
#include "cftlog.h"

/**
 * 消息ID的最大长度
 */
const int MIN_MSG_NO_LEN=10;
const int MAX_MSG_NO_LEN = 32;

/**
 * 消息类型
 */
enum EMSG_TYPE
{
    TEXT_MSG = 1, // 文本消息
    BINARY_MSG = 2,  // 二进制消息
    COMMON_QUERY_FAULT_MSG =3 //common query 格式
};

/**
 * 宏定义
 */
#define SRC_IP SerRunInfo::getSrcIp()
#define SERVICE SerRunInfo::getService()
#define HOST_IP SerRunInfo::getHostIp()
#define PROC_NO SerRunInfo::getProcNo()
#define MSG_NO SerRunInfo::getMsgNo()

/**
 * 业务运行信息对象
 */
class SerRunInfo
{  

/**
 * 消息来源IP
 */
static char src_ip[64+1];

/**
 * 服务名称
 */
static char service_name[128+1];

/**
 * 全局消息号
 */
static char msg_no[MAX_MSG_NO_LEN];

public:

    /**
     * 获取消息来源的IP地址
     */
    static inline const char* getSrcIp() { return src_ip; }

    /**
     * 获取当前所在服务名称
     */
    static inline const char* getService() { return service_name; }

    /**
     * 获取本地主机IP地址
     */
    static const char* getHostIp();

    /**
     * 获取全局消息号
     */
    static int getProcNo();
    
    /**
     * 获取全局消息号
     */
    static inline const char* getMsgNo() { return msg_no; }

public:
    
    /**
     * 构造函数
     */
    SerRunInfo(TRPC_SVCINFO* pRequst, EMSG_TYPE msgtype=TEXT_MSG);

    /**
     * 析构函数
     */
    ~SerRunInfo();
    
    /**
     * 服务所花费时间
     */
    inline int getTimeCost() { return (end.tv_sec  - start.tv_sec) * 1000 + (end.tv_usec- start.tv_usec)/1000; }
   
protected:
    TRPC_SVCINFO* rqst;
    EMSG_TYPE type; // 消息类型
    timeval start; // 服务调用起始时间
    timeval end; // 服务调用结束时间   
};

#endif

