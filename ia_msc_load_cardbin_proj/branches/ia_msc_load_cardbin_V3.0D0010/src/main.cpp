#include "ia_msc_cardbin_server.h"

#include "cardinfo.h"

using namespace std;

CftLog* gPtrAppLog = NULL; // 应用日志句柄
CftLog* gPtrSysLog = NULL; // 系统日志句柄
GlobalConfig* gPtrConfig = NULL; // 全局配置句柄




void initial();
void clear();

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main( int argc, char *argv[] )
{
    try
    {
		// 初始化
        initial();

        // 处理任务
        
        CMySQL *pMysql = new CMySQL(gPtrConfig->m_DbCfg.host, gPtrConfig->m_DbCfg.user,
                                 gPtrConfig->m_DbCfg.pswd, gPtrConfig->m_DbCfg.port,
                                 gPtrConfig->m_DbCfg.overtime, gPtrConfig->m_DbCfg.charset, "ia_msc_db");
        reflash_shms(pMysql, gPtrConfig->m_shm_key1, gPtrConfig->m_shm_key2);

        delete pMysql;


        // 打印启动日志
        gPtrAppLog->debug("Start ia_msc_cardbin_server ok!");
        gPtrSysLog->debug("Start ia_msc_cardbin_server ok!");
    }
    catch(CException &e)
    {
        char szErrMsg[MAX_MSG_LEN];
        snprintf(szErrMsg, sizeof(szErrMsg), "[%s:%d]%d,%s", e.file(), e.line(), e.error(), e.what());        
        std::cout << szErrMsg << std::endl;
        if (gPtrAppLog)
        {
            // doWarning(PERROR, e.error(), szErrMsg);
            gPtrAppLog->error(szErrMsg);
        }
    }
    catch(...)
    {
        char *szErrMsg = "process auth_clearing exits for exception";      
        std::cout << szErrMsg << std::endl;
        if (gPtrAppLog)
        {
            //doWarning(PERROR, ERR_UNKNOWN, szErrMsg);
            gPtrAppLog->error(szErrMsg);
        }
        throw;//为了调试方便，错误到了这里catch住，程序也是结束，所以再抛出便于观察错误信息。
    }

    // 释放资源
    clear();

    return 0;
}				/* ----------  end of function main  ---------- */


void initial()
{
    // 初始化配置文件
    gPtrConfig = new GlobalConfig("../conf/ia_msc_cardbin_server.xml");

    // 初始化日志指针
    gPtrAppLog = new CftLog((gPtrConfig->m_SysCfg.log_path + "/ia_msc_cardbin_server").c_str(),
                            gPtrConfig->m_SysCfg.log_size * 1024 * 1024, gPtrConfig->m_SysCfg.log_num,
                            CftLog::_DATE_MODE);
    gPtrAppLog->setSuffix("_app");

    gPtrSysLog = new CftLog((gPtrConfig->m_SysCfg.log_path + "/ia_msc_cardbin_server_sys").c_str(),
                            gPtrConfig->m_SysCfg.log_size * 1024 * 1024, gPtrConfig->m_SysCfg.log_num);
    
}


void clear()
{
    // 打印关闭日志
    gPtrSysLog->debug("Stop ia_msc_cardbin_server ok!");

    // 清除配置文件
    delete gPtrConfig;

    // 关闭日志
    delete gPtrAppLog;
    delete gPtrSysLog;
}
