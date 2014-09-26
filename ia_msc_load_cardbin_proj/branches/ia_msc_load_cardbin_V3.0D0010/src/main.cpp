#include "ia_msc_cardbin_server.h"

#include "cardinfo.h"

using namespace std;

CftLog* gPtrAppLog = NULL; // Ӧ����־���
CftLog* gPtrSysLog = NULL; // ϵͳ��־���
GlobalConfig* gPtrConfig = NULL; // ȫ�����þ��




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
		// ��ʼ��
        initial();

        // ��������
        
        CMySQL *pMysql = new CMySQL(gPtrConfig->m_DbCfg.host, gPtrConfig->m_DbCfg.user,
                                 gPtrConfig->m_DbCfg.pswd, gPtrConfig->m_DbCfg.port,
                                 gPtrConfig->m_DbCfg.overtime, gPtrConfig->m_DbCfg.charset, "ia_msc_db");
        reflash_shms(pMysql, gPtrConfig->m_shm_key1, gPtrConfig->m_shm_key2);

        delete pMysql;


        // ��ӡ������־
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
        throw;//Ϊ�˵��Է��㣬����������catchס������Ҳ�ǽ������������׳����ڹ۲������Ϣ��
    }

    // �ͷ���Դ
    clear();

    return 0;
}				/* ----------  end of function main  ---------- */


void initial()
{
    // ��ʼ�������ļ�
    gPtrConfig = new GlobalConfig("../conf/ia_msc_cardbin_server.xml");

    // ��ʼ����־ָ��
    gPtrAppLog = new CftLog((gPtrConfig->m_SysCfg.log_path + "/ia_msc_cardbin_server").c_str(),
                            gPtrConfig->m_SysCfg.log_size * 1024 * 1024, gPtrConfig->m_SysCfg.log_num,
                            CftLog::_DATE_MODE);
    gPtrAppLog->setSuffix("_app");

    gPtrSysLog = new CftLog((gPtrConfig->m_SysCfg.log_path + "/ia_msc_cardbin_server_sys").c_str(),
                            gPtrConfig->m_SysCfg.log_size * 1024 * 1024, gPtrConfig->m_SysCfg.log_num);
    
}


void clear()
{
    // ��ӡ�ر���־
    gPtrSysLog->debug("Stop ia_msc_cardbin_server ok!");

    // ��������ļ�
    delete gPtrConfig;

    // �ر���־
    delete gPtrAppLog;
    delete gPtrSysLog;
}
