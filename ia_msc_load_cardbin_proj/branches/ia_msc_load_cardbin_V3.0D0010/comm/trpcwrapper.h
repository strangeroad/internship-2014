#ifndef _TRPC_WRAPPER_H_
#define _TRPC_WRAPPER_H_

#include <cstdlib>
#include "trpc_client.h"
#include "trpc_service.h"
#include "exception.h"

/**
 * ��MiddleԶ�̵��ý��а�װ���࣬�ڲ�ʵ������������
 */
class CRpcWrapper
{
public:
    /**
     * ���캯��
     */
    CRpcWrapper(const string& host, int port, const string& user, const string& pswd, int ovtm) throw (CException);

    /**
     * ��������
     */
    ~CRpcWrapper();

    /**
     * ����ת��
     */
    inline operator TRPC_INFO*() { return m_pRpc; }

    /**
     * ִ�е���
     */
    void excute(const string& service, const char* szParam, int iLen, char* szResult, int& oLen) throw (CException);
    
protected:

    /**
     * ����Զ������
     */
    void connect() throw (CException);

    /**
     * �Ͽ�����
     */
    void disconnect();
    
protected:
    string   m_strHost;     // middle����IP
    int  m_iPort;              // middle�����˿�
    string   m_strUser;     // middle�û���
    string   m_strPswd;    // middle�û�����
    int  m_iOverTime;     // ���ӳ�ʱʱ��
    
    TRPC_INFO* m_pRpc;  // middle���Ӿ��
};

#endif

