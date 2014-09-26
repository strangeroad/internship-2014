#include "runinfo.h"
#include "log.h"
#include "cftlog.h"
#include "globalconfig.h"
#include "parameter.h"

/**
 * ȫ�ֱ���
 */
extern GlobalConfig* gPtrConfig;
extern CftLog* gPtrAppLog;

/**
 * ����Ӧ����־����
 * @desc: ͨ��ƽ̨������ɲ���Ӧ����־�ļ���
 */
static void setAppLogLevel()
{
    extern CLog* g_pLog; // Middle�������־ָ��
    
    if(g_pLog)
    {
        switch(g_pLog->get_log_level())
        {
            case 1:  // ���е�log����д
            {
                gPtrAppLog->setLogLevel(CftLog::_NONE);
                break;
            }
            case 2:// ֻд����log
            {
                gPtrAppLog->setLogLevel(CftLog::_WARNING);
                break;
            }
            case 3:// д��ͨlog�ʹ���log
            {
                gPtrAppLog->setLogLevel(CftLog::_NORMAL);
                break;
            }
            case 4:// д��ͨlog������log�͵���log
            {
                gPtrAppLog->setLogLevel(CftLog::_DEBUG);
                break;
            }
        }
    }
}

/**
 * ����ȫ����Ϣ��
 * @input: buf ��Ϣ�ŵĴ������
 */
static void makeMsgNo(char* buf)
{
    static unsigned int seq = 0;
    static int proc = getpid();
    
    snprintf(buf, MAX_MSG_NO_LEN, "%03d%05d%010u%010u", gPtrConfig->m_SysCfg.mid_no, proc, (int)time(NULL), seq++);
}

/**
 * ȫ����Ϣ�Ŷ���
 */
char SerRunInfo::msg_no[MAX_MSG_NO_LEN] = {0};
char SerRunInfo::src_ip[64+1] = {0};
char SerRunInfo::service_name[128+1] = {0};

/**
 * ���캯��
 */
SerRunInfo::SerRunInfo(TRPC_SVCINFO* pRequst, EMSG_TYPE msgtype) : rqst(pRequst), type(msgtype)
{
    // ������־����
    setAppLogLevel();

    // ȡ������
    snprintf(service_name, sizeof(service_name), "%s", pRequst->name);

    // ȡ��ԴIP
    snprintf(src_ip, sizeof(src_ip), "%s", pRequst->src_entry);
    
    // ��¼���ÿ�ʼʱ��
    gettimeofday(&start, NULL);   

    // ȡ��Ϣ��
    memset(msg_no, 0, sizeof(msg_no));
    CUrlAnalyze::getParam((char*)rqst->idata, "MSG_NO", msg_no, MAX_MSG_NO_LEN);   
    if(msg_no[0] == '\0')   makeMsgNo(msg_no);

    // ������־��Ϣ���к�
    gPtrAppLog->setMsgId(MSG_NO);

    // ��շ�����Ϣ
    pRequst->olen = snprintf(pRequst->odata, sizeof(pRequst->odata), "%s", "");
    
    // ��ӡ������Ϣ
    gPtrAppLog->normal("|in|%s|%s|%s|%s|", SRC_IP, HOST_IP, SERVICE, rqst->idata);
}

/**
 * ��������
 */
SerRunInfo::~SerRunInfo()
{
    // ��ȡ���ý���ʱ��
    gettimeofday(&end, NULL);   

    // ���㻨��ʱ��
    int iCost = getTimeCost();
    
    // ��ȡ���ػ���
    char* pMsg = (char*)rqst->odata;

    // ��ӡ������־
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
 * ��ȡ����IP��ַ
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
 * ��ȡȫ����Ϣ��
 */
int SerRunInfo::getProcNo()
{
    static int proc_id= getpid();;

    return proc_id;
}

