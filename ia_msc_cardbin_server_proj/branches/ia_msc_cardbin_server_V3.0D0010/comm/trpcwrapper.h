#ifndef _TRPC_WRAPPER_H_
#define _TRPC_WRAPPER_H_

#include <cstdlib>
#include "trpc_client.h"
#include "trpc_service.h"
#include "exception.h"

/**
 * 对Middle远程调用进行包装的类，内部实现了重连机制
 */
class CRpcWrapper
{
public:
    /**
     * 构造函数
     */
    CRpcWrapper(const string& host, int port, const string& user, const string& pswd, int ovtm) throw (CException);

    /**
     * 析构函数
     */
    ~CRpcWrapper();

    /**
     * 类型转换
     */
    inline operator TRPC_INFO*() { return m_pRpc; }

    /**
     * 执行调用
     */
    void excute(const string& service, const char* szParam, int iLen, char* szResult, int& oLen) throw (CException);
    
protected:

    /**
     * 建立远程连接
     */
    void connect() throw (CException);

    /**
     * 断开连接
     */
    void disconnect();
    
protected:
    string   m_strHost;     // middle主机IP
    int  m_iPort;              // middle主机端口
    string   m_strUser;     // middle用户名
    string   m_strPswd;    // middle用户密码
    int  m_iOverTime;     // 连接超时时长
    
    TRPC_INFO* m_pRpc;  // middle连接句柄
};

#endif

