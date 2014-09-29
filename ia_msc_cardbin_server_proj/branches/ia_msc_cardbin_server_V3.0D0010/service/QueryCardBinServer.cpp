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
 * 解析输入参数
 */
void QueryCardBinServer::parseInputMsg(TRPC_SVCINFO* pRequst) throw(CException)
{

    char szMsg[MAX_MSG_LEN + 1] = {0};
    strncpy( szMsg, (char *)pRequst->idata, MAX_MSG_LEN );    //过滤溢出攻击

    m_Params.readStrParam(szMsg, "cardnum",   1, 19);       // long long unsigned max=18000000000000000000 len=20
    m_Params.readIntParam(szMsg, "country_code_num",   0, 999);       // long long unsigned max=18000000000000000000 len=20

    // 设置当前系统时间
    m_Params.setParam( "systime", getSysTime().c_str() );
}

/**
 * 组装返回消息
 */
void QueryCardBinServer::packReturnMsg(TRPC_SVCINFO* pRequst)
{
    // 获取缓存
    char* pBuf = pRequst->odata;

    // 初始化缓存
    pBuf[0] = '\0';

    char buf[20] = "\0";


    /*
     * 测试过，long long unsigned 作为类成员， 使用sscanf设置值，是失败的。
     * 但是作为局部变量long long unsigned确成功，与C++语言本身有关吗？
     * snprintf(buf, 20, "%019llu", m_nCardnum); CUrlAnalyze::setParam(pBuf, "cardnum", buf); //debug
     */
    CUrlAnalyze::setParam(pBuf, "cardnum", m_Params.getString("cardnum").c_str());
    CUrlAnalyze::setParam(pBuf, "result", "0");

    /*
     * 宏作用
     * URL_SET_STR(VAL) :  添加 "VAL"=VAL 的返回参数，VAL是char *类型
     * URL_SET_LLU(VAL) :  添加 "VAL"=VAL 的返回参数，VAL是64位无符号型整数
     * URL_SET_INT(VAL) :  添加 "VAL"=VAL 的返回参数，VAL是整数
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

    // 设置返回结果长度
    pRequst->olen = strlen(pBuf);
}

/**
 * 执行操作
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
        // 抛出异常会执行析构函数，所以不用shmdt()
        throw CException(ERR_NOT_FOUND, "not found", __FILE__, __LINE__);
    }
}

/**
 * 检查参数
 */
void QueryCardBinServer::checkParam()
{
    string strCardNum = m_Params.getString ( "cardnum" );
    // sscanf特性：读整型时，只要首位是数字而不是字母，就不会有问题
    if ( !( '0' <= strCardNum[0] && strCardNum[0] <= '9' ) )
        throw CException(ERR_PARAMS_CARDNUM, "we need card num str", __FILE__, __LINE__);
    /*
     * 测试过，long long unsigned 作为类成员， 使用sscanf设置值，是失败的。
     * 但是作为局部变量long long unsigned确成功，与C++语言本身有关吗？
    if ( '0' <= strCardNum[0] && strCardNum[0] <= '9' )
        sscanf(strCardNum.c_str(), "%llu", &this->m_nCardnum);
    else
        throw CException(ERR_PARAMS_CARDNUM, "we need card num str", __FILE__, __LINE__);
     */
}


void QueryCardBinServer::loadShm()
{
    /*
     * 根据key值逐个测试
     * shmget获取共享内存，查看前4个字节int值，如果等于shmkey就表示已就绪
     */
     m_nShmId = shmget(gPtrConfig->m_shm_key1, 0, 666);
    if (m_nShmId != -1){
        m_pMem = shmat(m_nShmId, 0, 0);
        int *ptr = (int*)m_pMem,
            key_test = ptr[0];
        // thanks ianwu. bug:
        // 校验值，有可能更新工作者进程刚开辟了内存，还没装载数据，就被server进程shmat映射了。
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
    // 都失败
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
