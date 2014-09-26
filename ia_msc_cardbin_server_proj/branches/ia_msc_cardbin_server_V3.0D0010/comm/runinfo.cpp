#include "runinfo.h"
#include "log.h"
#include "cftlog.h"
#include "globalconfig.h"
#include "parameter.h"

/**
 * 全局变量
 */
extern GlobalConfig* gPtrConfig;
extern CftLog* gPtrAppLog;

/**
 * 设置应用日志级别
 * @desc: 通过平台的命令即可操纵应用日志的级别
 */
static void setAppLogLevel()
{
    extern CLog* g_pLog; // Middle服务的日志指针
    
    if(g_pLog)
    {
        switch(g_pLog->get_log_level())
        {
            case 1:  // 所有的log都不写
            {
                gPtrAppLog->setLogLevel(CftLog::_NONE);
                break;
            }
            case 2:// 只写错误log
            {
                gPtrAppLog->setLogLevel(CftLog::_WARNING);
                break;
            }
            case 3:// 写普通log和错误log
            {
                gPtrAppLog->setLogLevel(CftLog::_NORMAL);
                break;
            }
            case 4:// 写普通log、错误log和调试log
            {
                gPtrAppLog->setLogLevel(CftLog::_DEBUG);
                break;
            }
        }
    }
}

/**
 * 生成全局消息号
 * @input: buf 消息号的存放区域
 */
static void makeMsgNo(char* buf)
{
    static unsigned int seq = 0;
    static int proc = getpid();
    
    snprintf(buf, MAX_MSG_NO_LEN, "%03d%05d%010u%010u", gPtrConfig->m_SysCfg.mid_no, proc, (int)time(NULL), seq++);
}

/**
 * 全局消息号定义
 */
char SerRunInfo::msg_no[MAX_MSG_NO_LEN] = {0};
char SerRunInfo::src_ip[64+1] = {0};
char SerRunInfo::service_name[128+1] = {0};

/**
 * 构造函数
 */
SerRunInfo::SerRunInfo(TRPC_SVCINFO* pRequst, EMSG_TYPE msgtype) : rqst(pRequst), type(msgtype)
{
    // 设置日志级别
    setAppLogLevel();

    // 取服务名
    snprintf(service_name, sizeof(service_name), "%s", pRequst->name);

    // 取来源IP
    snprintf(src_ip, sizeof(src_ip), "%s", pRequst->src_entry);
    
    // 记录调用开始时间
    gettimeofday(&start, NULL);   

    // 取消息号
    memset(msg_no, 0, sizeof(msg_no));
    CUrlAnalyze::getParam((char*)rqst->idata, "MSG_NO", msg_no, MAX_MSG_NO_LEN);   
    if(msg_no[0] == '\0')   makeMsgNo(msg_no);

    // 设置日志消息序列号
    gPtrAppLog->setMsgId(MSG_NO);

    // 清空返回消息
    pRequst->olen = snprintf(pRequst->odata, sizeof(pRequst->odata), "%s", "");
    
    // 打印输入消息
    gPtrAppLog->normal("|in|%s|%s|%s|%s|", SRC_IP, HOST_IP, SERVICE, rqst->idata);
}

/**
 * 析构函数
 */
SerRunInfo::~SerRunInfo()
{
    // 获取调用结束时间
    gettimeofday(&end, NULL);   

    // 计算花费时间
    int iCost = getTimeCost();
    
    // 获取返回缓存
    char* pMsg = (char*)rqst->odata;

    // 打印返回日志
    if(type == TEXT_MSG)
    {
        gPtrAppLog->normal("|out|%s|%s|%s|%s|%dms|\n", SRC_IP, HOST_IP, SERVICE, rqst->odata, iCost); 
    }
    else
    {
        int iResult = *(int*)(pMsg);

        if(iResult == 0)
        {
            gPtrAppLog->normal("|out|%s|%s|%s|len=%d,result=%d,count=%d,total=%d|%dms|\n", 
                         SRC_IP, HOST_IP, SERVICE, rqst->olen, iResult, 
                         *(int*)(pMsg+sizeof(int)), *(int*)(pMsg+sizeof(int)*2), iCost);
        }
        else
        {
            gPtrAppLog->normal("|out|%s|%s|%s|len=%d,result=%d,res_info=%s|%dms|\n", 
                         SRC_IP, HOST_IP, SERVICE, 
                         rqst->olen, iResult, (pMsg+sizeof(int)), iCost);
        }
    }
}

/**
 * 获取主机IP地址
 */
const char* SerRunInfo::getHostIp()
{
    static string host_ip;

    if(host_ip.empty())
    {
        host_ip = getLocalHostIp();
    }

    return host_ip.c_str();
}

/**
 * 获取全局消息号
 */
int SerRunInfo::getProcNo()
{
    static int proc_id= getpid();;

    return proc_id;
}

