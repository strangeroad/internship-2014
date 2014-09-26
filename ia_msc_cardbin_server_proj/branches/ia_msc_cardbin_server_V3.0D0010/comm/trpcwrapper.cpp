#include <sstream>
#include <sys/time.h>
#include "trpcwrapper.h"
#include "error.h"
#include "cftlog.h"
#include "runinfo.h"
#include "common.h"

/**
 * ȫ�ֱ�������
 */
extern CftLog* gPtrAppLog;

/**
 *  ���캯��
 * @input       host        middle������ַ
 *                 port         middle�����˿�
 *                 user         middle�����û���
 *                 pswd        middle��������
 *                 ovtm        middle���ӳ�ʱʱ��
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
 * ����Զ������
 */
void CRpcWrapper::connect() throw (CException)
{
    // �ȶϿ�����
    disconnect();

    try
    {
        // ������������
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
        // �Ͽ�����
        disconnect();
        
        throw;
    }
}

/**
 * �Ͽ�Զ������
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
 * ִ��Զ�̵���
 * @input       service         Զ�̹�������
 *                  param         ���ò���
 *                  paramlen     ��������
 *                  reslen          ��������С
 * @output     result           ���ؽ��
 *                  reslen          ���ؽ������
 */
void CRpcWrapper::excute(const string& service, const char* param, int paramlen, char* result, int& reslen) throw (CException)
{
    struct timeval begin, end;

    try
    {
        // ���������Ƚ�������
        if(!m_pRpc)
        {
            connect();
        }
        
        // ��ȡ���ÿ�ʼʱ��
        gettimeofday(&begin, NULL);   

        // Զ�̹��̵���
        int iRet = trpc_call(m_pRpc, service.c_str(), param, paramlen, result, (size_t*)&reslen);    

        // ��ȡ���ý���ʱ��
        gettimeofday(&end, NULL);   

        // ��ӡ��־
        gPtrAppLog->debug("|rpc|%s|%s|%s|%s|%d|%s|%dms|%d|%s | %s|",
                                        SRC_IP, HOST_IP, SERVICE,
                                        m_strHost.c_str(), m_iPort, service.c_str(), 
                                        (end.tv_sec  - begin.tv_sec) * 1000 + (end.tv_usec- begin.tv_usec)/1000,
                                        iRet, param, result);
        
        // �жϷ��ؽ��
        if (iRet != 0)
        {  
            // �׳������쳣
            throw CException(ERR_FAIL_CALL_MIDDLE, trpc_error(m_pRpc), __FILE__, __LINE__);
        }
    }
    catch(CException& e)
    {
        // ����
        disconnect();

        // �׳��쳣���ϲ�
        throw;
    }
}

