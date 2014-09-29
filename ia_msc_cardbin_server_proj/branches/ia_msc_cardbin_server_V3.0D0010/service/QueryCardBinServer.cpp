/*
 * =====================================================================================
 *
 *       Filename:  QueryCardBinServer.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014/9/14 16:19:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wilsonwuwu (wws), 
 *   Organization:  tencent
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#include "QueryCardBinServer.h"
#include "globalconfig.h"

extern GlobalConfig* gPtrConfig;
extern CftLog * gPtrAppLog;

QueryCardBinServer::QueryCardBinServer()
    :m_pCard(NULL), m_nShmId(-1), m_pMem(NULL)
{
}

QueryCardBinServer::~QueryCardBinServer()
{
    unloadShm();
}


/**
 * �����������
 */
void QueryCardBinServer::parseInputMsg(TRPC_SVCINFO* pRequst) throw(CException)
{

    char szMsg[MAX_MSG_LEN + 1] = {0};
    strncpy( szMsg, (char *)pRequst->idata, MAX_MSG_LEN );    //�����������

    m_Params.readStrParam(szMsg, "cardnum",   1, 19);       // long long unsigned max=18000000000000000000 len=20
    m_Params.readIntParam(szMsg, "country_code_num",   0, 999);       // long long unsigned max=18000000000000000000 len=20

    // ���õ�ǰϵͳʱ��
    m_Params.setParam( "systime", getSysTime().c_str() );
}

/**
 * ��װ������Ϣ
 */
void QueryCardBinServer::packReturnMsg(TRPC_SVCINFO* pRequst)
{
    // ��ȡ����
    char* pBuf = pRequst->odata;

    // ��ʼ������
    pBuf[0] = '\0';

    char buf[20] = "\0";


    /*
     * ���Թ���long long unsigned ��Ϊ���Ա�� ʹ��sscanf����ֵ����ʧ�ܵġ�
     * ������Ϊ�ֲ�����long long unsignedȷ�ɹ�����C++���Ա����й���
     * snprintf(buf, 20, "%019llu", m_nCardnum); CUrlAnalyze::setParam(pBuf, "cardnum", buf); //debug
     */
    CUrlAnalyze::setParam(pBuf, "cardnum", m_Params.getString("cardnum").c_str());
    CUrlAnalyze::setParam(pBuf, "result", "0");

    /*
     * ������
     * URL_SET_STR(VAL) :  ��� "VAL"=VAL �ķ��ز�����VAL��char *����
     * URL_SET_LLU(VAL) :  ��� "VAL"=VAL �ķ��ز�����VAL��64λ�޷���������
     * URL_SET_INT(VAL) :  ��� "VAL"=VAL �ķ��ز�����VAL������
     */
#define URL_SET_STR(VAL) CUrlAnalyze::setParam(pBuf, #VAL, m_pCard->VAL);
#define URL_SET_LLU(VAL) snprintf(buf, 20, "%019llu", m_pCard->VAL);\
                        CUrlAnalyze::setParam(pBuf, #VAL, buf);
#define URL_SET_INT(VAL) snprintf(buf, 4, "%03d", m_pCard->VAL);\
                        CUrlAnalyze::setParam(pBuf, #VAL, buf);
    URL_SET_STR(effective_time              );
 // URL_SET_STR(active_inactive_code        );
    URL_SET_LLU(issuer_acc_range_low        );
 // URL_SET_STR(issuer_acc_range_prefix     );
    URL_SET_STR(gcms_product_id             );
    URL_SET_LLU(issuer_acc_range_high       );
    URL_SET_STR(card_program_id             );
    URL_SET_STR(issuer_card_prog_id         );
    URL_SET_STR(number_id                   );
    URL_SET_STR(product_type_id             );
    URL_SET_STR(endpoint                    );
    URL_SET_STR(country_code_alpha          );
    URL_SET_INT(country_code_num            );
    URL_SET_STR(region                      );
    URL_SET_STR(product_class               );
    URL_SET_STR(trans_routing_id            );
    URL_SET_STR(present_reassign_switch     );
    URL_SET_STR(product_reassign_switch     );
    URL_SET_STR(pwcb_optin_switch           );
    URL_SET_STR(licensed_product_id         );
    URL_SET_STR(map_service_id              );
    URL_SET_STR(acc_level_man_part_id       );
    URL_SET_STR(acc_level_man_active_date   );
    URL_SET_STR(cardholder_bill_cur_def     );
    URL_SET_STR(cardholder_bill_cur_exp_def );
    URL_SET_STR(cardholder_bill_pri_cur     );
    URL_SET_STR(chip_to_mag_conv_sev_id     );
    URL_SET_STR(floor_exp_date              );
    URL_SET_STR(cobrand_part_switch         );
    URL_SET_STR(spend_con_switch            );
    URL_SET_STR(mer_clean_sev_part          );
    URL_SET_STR(mer_clean_act_date          );
    URL_SET_STR(paypass_enable_id           );
    URL_SET_STR(reg_rate_type_id            );
    URL_SET_STR(psn_route_id                );
    URL_SET_STR(cashback_without_pur_id     );
 // URL_SET_STR(create_time                 );
 // URL_SET_STR(modify_time                 );

    // ���÷��ؽ������
    pRequst->olen = strlen(pBuf);
}

/**
 * ִ�в���
 */
void QueryCardBinServer::excute() throw(CException)
{
    loadShm();
    int *ptr = (int*)m_pMem,
        size = ptr[1];
    s_card_info_t* arr = (s_card_info_t*)(ptr+2);

    string strCardNum = m_Params.getString ( "cardnum" );
    long long unsigned num;
    int country_num = m_Params.getInt("country_code_num");
    sscanf(strCardNum.c_str(), "%llu", &num);
    // m_nCardnum = 4022268888888888888llu;    //debug;
    m_pCard = binary_search(num, arr, size, country_num);
    // m_pCard = arr+20;        // debug;
    if ( !m_pCard ) {
        // �׳��쳣��ִ���������������Բ���shmdt()
        throw CException(ERR_NOT_FOUND, "not found", __FILE__, __LINE__);
    }
}

/**
 * ������
 */
void QueryCardBinServer::checkParam()
{
    string strCardNum = m_Params.getString ( "cardnum" );
    // sscanf���ԣ�������ʱ��ֻҪ��λ�����ֶ�������ĸ���Ͳ���������
    if ( !( '0' <= strCardNum[0] && strCardNum[0] <= '9' ) )
        throw CException(ERR_PARAMS_CARDNUM, "we need card num str", __FILE__, __LINE__);
    /*
     * ���Թ���long long unsigned ��Ϊ���Ա�� ʹ��sscanf����ֵ����ʧ�ܵġ�
     * ������Ϊ�ֲ�����long long unsignedȷ�ɹ�����C++���Ա����й���
    if ( '0' <= strCardNum[0] && strCardNum[0] <= '9' )
        sscanf(strCardNum.c_str(), "%llu", &this->m_nCardnum);
    else
        throw CException(ERR_PARAMS_CARDNUM, "we need card num str", __FILE__, __LINE__);
     */
}


void QueryCardBinServer::loadShm()
{
    /*
     * ����keyֵ�������
     * shmget��ȡ�����ڴ棬�鿴ǰ4���ֽ�intֵ���������shmkey�ͱ�ʾ�Ѿ���
     */
     m_nShmId = shmget(gPtrConfig->m_shm_key1, 0, 666);
    if (m_nShmId != -1){
        m_pMem = shmat(m_nShmId, 0, 0);
        int *ptr = (int*)m_pMem,
            key_test = ptr[0];
        // thanks ianwu. bug:
        // У��ֵ���п��ܸ��¹����߽��̸տ������ڴ棬��ûװ�����ݣ��ͱ�server����shmatӳ���ˡ�
        if (key_test != gPtrConfig->m_shm_key1){
            shmdt(m_pMem);
            m_pMem = NULL;
        }
    }

    if (!m_pMem){
        m_nShmId = shmget(gPtrConfig->m_shm_key2, 0, 666);
        if (m_nShmId != -1){
            m_pMem = shmat(m_nShmId, 0, 0);
        }
    }
    // ��ʧ��
    if (m_nShmId==-1 || m_pMem==NULL){
        const char* str_err = "no memery ready [in constructor]";
        gPtrAppLog->error(str_err);
        throw CException(ERR_NO_MEMERY_READY, str_err, __FILE__, __LINE__);
    }
}

void QueryCardBinServer::unloadShm()
{
    if (m_nShmId != -1 && m_pMem != NULL)
        shmdt(m_pMem);
        // if( shmdt(m_pMem) < 0 )
        //     gPtrAppLog->error("shmdt(): unmount memery error!");
}
