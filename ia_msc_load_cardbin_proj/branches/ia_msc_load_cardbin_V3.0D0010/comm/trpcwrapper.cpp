#include <sstream>
#include <sys/time.h>
#include "trpcwrapper.h"
#include "error.h"
#include "cftlog.h"
#include "runinfo.h"
#include "common.h"

/**
 * 全局变量声明
 */
extern CftLog* gPtrAppLog;

/**
 *  构造函数
 * @input       host        middle主机地址
 *                 port         middle主机端口
 *                 user         middle连接用户名
 *                 pswd        middle连接密码
 *                 ovtm        middle连接超时时长
 */
CRpcWrapper::CRpcWrapper(const string& host, int port, const string& user, const string& pswd, int ovtm) throw(CException)
                                                  : m_strHost(host), m_iPort(port), m_strUser(user), m_strPswd(pswd), m_iOverTime(ovtm)
{
    m_pRpc = NULL;
}

CRpcWrapper::~CRpcWrapper()
{
    disconnect();
}

/**
 * 建立远程连接
 */
void CRpcWrapper::connect() throw (CException)
{
    // 先断开连接
    disconnect();

    try
    {
        // 创建核心连接
        m_pRpc = trpc_info_create(m_strHost.c_str(), m_iPort, m_strUser.c_str(), m_strPswd.c_str(), m_iOverTime);
        
        if (m_pRpc == NULL)
        {
            throw CException(ERR_FAIL_CONNECT_MIDDLE, "Failed to excute trpc_info_create!", __FILE__, __LINE__);
        }

        if (trpc_open(m_pRpc) != 0)
        {
            throw CException(ERR_FAIL_CONNECT_MIDDLE, string("Failed to excute trpc_open:") + trpc_error(m_pRpc), __FILE__, __LINE__);
        }
    }
    catch(CException& e)
    {
        // 断开连接
        disconnect();
        
        throw;
    }
}

/**
 * 断开远程连接
 */
void CRpcWrapper::disconnect()
{
    if(m_pRpc)
    {
        trpc_close(m_pRpc);
        trpc_info_destory(m_pRpc);
        m_pRpc = NULL;
    }
}

/**
 * 执行远程调用
 * @input       service         远程过程名称
 *                  param         调用参数
 *                  paramlen     参数长度
 *                  reslen          结果缓冲大小
 * @output     result           返回结果
 *                  reslen          返回结果长度
 */
void CRpcWrapper::excute(const string& service, const char* param, int paramlen, char* result, int& reslen) throw (CException)
{
    struct timeval begin, end;

    try
    {
        // 若断连则先建立连接
        if(!m_pRpc)
        {
            connect();
        }
        
        // 获取调用开始时间
        gettimeofday(&begin, NULL);   

        // 远程过程调用
        int iRet = trpc_call(m_pRpc, service.c_str(), param, paramlen, result, (size_t*)&reslen);    

        // 获取调用结束时间
        gettimeofday(&end, NULL);   

        // 打印日志
        gPtrAppLog->debug("|rpc|%s|%s|%s|%s|%d|%s|%dms|%d|%s | %s|",
                                        SRC_IP, HOST_IP, SERVICE,
                                        m_strHost.c_str(), m_iPort, service.c_str(), 
                                        (end.tv_sec  - begin.tv_sec) * 1000 + (end.tv_usec- begin.tv_usec)/1000,
                                        iRet, param, result);
        
        // 判断返回结果
        if (iRet != 0)
        {  
            // 抛出网络异常
            throw CException(ERR_FAIL_CALL_MIDDLE, trpc_error(m_pRpc), __FILE__, __LINE__);
        }
    }
    catch(CException& e)
    {
        // 断连
        disconnect();

        // 抛出异常到上层
        throw;
    }
}

