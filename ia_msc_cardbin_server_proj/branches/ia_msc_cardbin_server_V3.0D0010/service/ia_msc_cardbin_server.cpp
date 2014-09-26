#include "ia_msc_cardbin_server.h"
#include "QueryCardBinServer.h"

CftLog* gPtrAppLog = NULL; // Ӧ����־���
CftLog* gPtrSysLog = NULL; // ϵͳ��־���
GlobalConfig* gPtrConfig = NULL; // ȫ�����þ��



#define IMPLEMENT_SERVICE(name,class)                       \
DECLARE_SERVICE(name)                                       \
{                                                           \
    SerRunInfo SRI(rqst);                                   \
    try                                                     \
    {                                                       \
        class objService;                                   \
        objService.parseInputMsg(rqst);                     \
        objService.excute();                                \
        objService.packReturnMsg(rqst);                     \
    }                                                       \
    catch(CException& e)                                    \
    {                                                       \
        gPtrAppLog->error("[%s:%d]%d, %s",                  \
            e.file(), e.line(), e.error(), e.what());       \
        packRetunMsg(rqst, e.error(), e.what());            \
    }                                                       \
    trpc_return(rqst, 0);                                   \
}


/**
 * ���������Ϣ
 *@input:   error ������
 *          errinfo ������Ϣ
 *          type �Ƿ��ı�Э��
 *@output:  pRequst  TRPC������Ϣ����
 */
void packRetunMsg(TRPC_SVCINFO* pRequst, int error, const char* errinfo, EMSG_TYPE type=TEXT_MSG)
{
    if(type == TEXT_MSG)
    {
        pRequst->olen = snprintf(pRequst->odata, sizeof(pRequst->odata), "result=%d&res_info=%s", error, errinfo);
    }
    else if (type == COMMON_QUERY_FAULT_MSG )
    {
        pRequst->olen = snprintf(pRequst->odata, sizeof(pRequst->odata), "result=%d&res_info=%s&rec_info=<root><recode>%d</recode><retmsg>%s</retmsg></root>", error, errinfo,error, errinfo);
    }
    else
    {
        memcpy(pRequst->odata, &error, sizeof(int));
        pRequst->olen = snprintf(pRequst->odata+sizeof(int), sizeof(pRequst->odata), "%s", errinfo) + sizeof(int);
    }
}

/**
 * Description: ��������ʱ��ʼ��SO��Դ
 */
DECLARE_SO_INIT ()
{
    try
    {
        // ��ʼ�������ļ�
        gPtrConfig = new GlobalConfig("/usr/local/middle/ia_msc_cardbin_server/conf/ia_msc_cardbin_server.xml");

        // ��ʼ����־ָ��
        gPtrAppLog = new CftLog((gPtrConfig->m_SysCfg.log_path + "/ia_msc_cardbin_server").c_str(),
                                gPtrConfig->m_SysCfg.log_size * 1024 * 1024, gPtrConfig->m_SysCfg.log_num,
                                CftLog::_DATE_MODE);
        gPtrAppLog->setSuffix("_app");

        gPtrSysLog = new CftLog((gPtrConfig->m_SysCfg.log_path + "/ia_msc_cardbin_server_sys").c_str(),
                                gPtrConfig->m_SysCfg.log_size * 1024 * 1024, gPtrConfig->m_SysCfg.log_num);
        
        //�������ܵĸ���д�ڴ棬����serverд�ڴ档
        /*
        CMySQL *pMysql = new CMySQL(gPtrConfig->m_DbCfg.host, gPtrConfig->m_DbCfg.user,
                                 gPtrConfig->m_DbCfg.pswd, gPtrConfig->m_DbCfg.port,
                                 gPtrConfig->m_DbCfg.overtime, gPtrConfig->m_DbCfg.charset, "ia_msc_db");
        reflash_shms(pMysql, gPtrConfig->m_shm_key1, gPtrConfig->m_shm_key2);

        delete pMysql;
        */


        // ��ӡ������־
        gPtrAppLog->debug("Start ia_msc_cardbin_server ok!");
        gPtrSysLog->debug("Start ia_msc_cardbin_server ok!");
    }
    catch(CException& e)
    {
        if (gPtrAppLog)
        {
            gPtrAppLog->error("Failed to start ia_msc_cardbin_server: %d, %s!\n",  e.error(), e.what());
        }
        else
        {
            trpc_error_log("Failed to start ia_msc_cardbin_server: %d, %s!\n",  e.error(), e.what());
        }

        throw;
    }
}

/**
 * Description: ����ֹͣʱ����SO��Դ
 */
DECLARE_SO_FINI()
{
    // ��ӡ�ر���־
    gPtrSysLog->debug("Stop ia_msc_cardbin_server ok!");

    //�������ܵĸ���д�ڴ棬����serverд�ڴ档
    // clean_shms(gPtrConfig->m_shm_key1, gPtrConfig->m_shm_key2);

    // ��������ļ�
    delete gPtrConfig;

    // �ر���־
    delete gPtrAppLog;
    delete gPtrSysLog;
}

// Description:
IMPLEMENT_SERVICE(ia_msc_query_cardbin_service, QueryCardBinServer);









