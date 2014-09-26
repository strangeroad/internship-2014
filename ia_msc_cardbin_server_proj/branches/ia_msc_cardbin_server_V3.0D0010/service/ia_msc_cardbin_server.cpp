#include "ia_msc_cardbin_server.h"
#include "QueryCardBinServer.h"

CftLog* gPtrAppLog = NULL; // 应用日志句柄
CftLog* gPtrSysLog = NULL; // 系统日志句柄
GlobalConfig* gPtrConfig = NULL; // 全局配置句柄



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
 * 打包返回信息
 *@input:   error 错误码
 *          errinfo 错误信息
 *          type 是否文本协议
 *@output:  pRequst  TRPC返回消息缓存
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
 * Description: 服务启动时初始化SO资源
 */
DECLARE_SO_INIT ()
{
    try
    {
        // 初始化配置文件
        gPtrConfig = new GlobalConfig("/usr/local/middle/ia_msc_cardbin_server/conf/ia_msc_cardbin_server.xml");

        // 初始化日志指针
        gPtrAppLog = new CftLog((gPtrConfig->m_SysCfg.log_path + "/ia_msc_cardbin_server").c_str(),
                                gPtrConfig->m_SysCfg.log_size * 1024 * 1024, gPtrConfig->m_SysCfg.log_num,
                                CftLog::_DATE_MODE);
        gPtrAppLog->setSuffix("_app");

        gPtrSysLog = new CftLog((gPtrConfig->m_SysCfg.log_path + "/ia_msc_cardbin_server_sys").c_str(),
                                gPtrConfig->m_SysCfg.log_size * 1024 * 1024, gPtrConfig->m_SysCfg.log_num);
        
        //由于批跑的辅助写内存，不必server写内存。
        /*
        CMySQL *pMysql = new CMySQL(gPtrConfig->m_DbCfg.host, gPtrConfig->m_DbCfg.user,
                                 gPtrConfig->m_DbCfg.pswd, gPtrConfig->m_DbCfg.port,
                                 gPtrConfig->m_DbCfg.overtime, gPtrConfig->m_DbCfg.charset, "ia_msc_db");
        reflash_shms(pMysql, gPtrConfig->m_shm_key1, gPtrConfig->m_shm_key2);

        delete pMysql;
        */


        // 打印启动日志
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
 * Description: 服务停止时清理SO资源
 */
DECLARE_SO_FINI()
{
    // 打印关闭日志
    gPtrSysLog->debug("Stop ia_msc_cardbin_server ok!");

    //由于批跑的辅助写内存，不必server写内存。
    // clean_shms(gPtrConfig->m_shm_key1, gPtrConfig->m_shm_key2);

    // 清除配置文件
    delete gPtrConfig;

    // 关闭日志
    delete gPtrAppLog;
    delete gPtrSysLog;
}

// Description:
IMPLEMENT_SERVICE(ia_msc_query_cardbin_service, QueryCardBinServer);









