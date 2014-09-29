/*
 * =====================================================================================
 *
 *       Filename:  cardinfo.cpp
 *
 *    Description:  卡号区间处理模块，用于ia_msc_cardbin_server和ia_msc_load_cardbin两个svn工程中
 *
 *        Version:  1.0
 *        Created:  2014/9/14 15:34:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wilsonwuwu (582223837@qq.com),
 *   Organization:  tencent
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "cardinfo.h"

#include "sqlapi.h"
#include "globalconfig.h"
#include "cftlog.h"

#include <algorithm>
#include <string.h>

extern GlobalConfig* gPtrConfig;
extern CftLog * gPtrAppLog;
extern CftLog * gPtrSysLog;


// 查找卡号用的比较函数
int cmper(unsigned long long larg, s_card_info_t* rarg, int country_code_num)
{
    if (larg < rarg->issuer_acc_range_low)
        return -1;
    else if (larg > rarg->issuer_acc_range_high)
        return +1;
    else // if (rarg->issuer_acc_range_low <= larg && larg <= rarg->issuer_acc_range_high)
    {
        if (country_code_num == 0 || country_code_num == rarg->country_code_num)
            return 0;
        else if (country_code_num < rarg->country_code_num)
            return -1;
        else // if (country_code_num > rarg->country_code_num)
            return +1;
    } // must need {}
}


// 二分法查找排序的结构体数组
s_card_info_t* binary_search(unsigned long long tag, s_card_info_t* arr, int len, int country_code_num)
{
    int
        low     = 0,
        high    = len,
        mid;
    while (low < high) {
        mid = (low+high)/2;
        int cmpout = cmper(tag, arr+mid, country_code_num);
        if (cmpout == 0)
            return arr+mid;
        else if (cmpout == 1)
            low = mid+1;
        else
            high = mid;
    }
    return NULL;
}


// 结构体数组排序用的比较函数
int cmp_struct(const s_card_info_t &larg, const s_card_info_t &rarg){
    // low, high, country
    if (larg.issuer_acc_range_low != rarg.issuer_acc_range_low)
        return larg.issuer_acc_range_low <= rarg.issuer_acc_range_low;
    if (larg.issuer_acc_range_high != rarg.issuer_acc_range_high)
        return larg.issuer_acc_range_high <= rarg.issuer_acc_range_high;
    return larg.country_code_num <= rarg.country_code_num;
}



// 装载数据库到使用shm_key开辟的共享内存中
int load_shm(CMySQL *pMysql, int shm_key)
{
    // 有些结构体没有的字段也select出来，是为了占位，好维护
    const char *sql = "select "
            "Feffective_time              ,"
            "Factive_inactive_code        ,"
            "Fissuer_acc_range_low        ,"
            "Fissuer_acc_range_prefix     ,"
            "Fgcms_product_id             ,"
            "Fissuer_acc_range_high       ,"
            "Fcard_program_id             ,"
            "Fissuer_card_prog_id         ,"
            "Fnumber_id                   ,"
            "Fproduct_type_id             ,"
            "Fendpoint                    ,"
            "Fcountry_code_alpha          ,"
            "Fcountry_code_num            ,"
            "Fregion                      ,"
            "Fproduct_class               ,"
            "Ftrans_routing_id            ,"
            "Fpresent_reassign_switch     ,"
            "Fproduct_reassign_switch     ,"
            "Fpwcb_optin_switch           ,"
            "Flicensed_product_id         ,"
            "Fmap_service_id              ,"
            "Facc_level_man_part_id       ,"
            "Facc_level_man_active_date   ,"
            "Fcardholder_bill_cur_def     ,"
            "Fcardholder_bill_cur_exp_def ,"
            "Fcardholder_bill_pri_cur     ,"
            "Fchip_to_mag_conv_sev_id     ,"
            "Ffloor_exp_date              ,"
            "Fcobrand_part_switch         ,"
            "Fspend_con_switch            ,"
            "Fmer_clean_sev_part          ,"
            "Fmer_clean_act_date          ,"
            "Fpaypass_enable_id           ,"
            "Freg_rate_type_id            ,"
            "Fpsn_route_id                ,"
            "Fcashback_without_pur_id     ,"
            "Fcreate_time                 ,"
            "Fmodify_time                  "
            "from ia_gcms_2014.t_IP0040T1 "
            "where Factive_inactive_code='A' ;";
            // "order by Fissuer_acc_range_low;";

    pMysql->Query(sql, strlen(sql));

    MYSQL_RES* pResult = pMysql->FetchResult();

    int iRow = mysql_num_rows(pResult);

    // m_pMem = (s_card_info_t*)malloc(sizeof(s_card_info_t) * iRow);

    int mem_size = sizeof(int)*2 + sizeof(s_card_info_t) * iRow;
    int shmid = shmget(shm_key, mem_size, 0666 | IPC_CREAT);
    int *pMem = (int*)shmat(shmid, 0, 0);     // need shmdt it !
    memset(pMem, 0, mem_size);           // set 0!
    pMem[1] = iRow;
    s_card_info_t *pData = (s_card_info_t *)&pMem[2];


    if (!pMem){
        mysql_free_result(pResult);
        throw "malloc memery error";
    }

    MYSQL_ROW row = NULL ;
    int i = 0;
    while ( (row = mysql_fetch_row(pResult)) )
    {
        s_card_info_t &card = pData[i];
#define COL_SAVE_STR(ROW, COL, PRE, BUF) strncpy(PRE.BUF, ROW[COL] ? ROW[COL] : "", sizeof(PRE.BUF)-1);//字符串
#define COL_SAVE_LLU(ROW, COL, PRE, BUF) sscanf(ROW[COL] ? ROW[COL] : "0", "%llu", &PRE.BUF);//无符号长长整型
#define COL_SAVE_INT(ROW, COL, PRE, BUF) sscanf(ROW[COL] ? ROW[COL] : "0", "%d", &PRE.BUF);//整型
        /*
         * 宏作用
         * COL_SAVE_STR(ROW, COL, PRE, BUF) : save ROW[COL] to PRE.BUF (type: char[])
         * COL_SAVE_LLU(ROW, COL, PRE, BUF) : save ROW[COL] to PRE.BUF (type: long long unsigned)
         * COL_SAVE_INT(ROW, COL, PRE, BUF) : save ROW[COL] to PRE.BUF (type: int)
         */
        COL_SAVE_STR(row, 0,   card, effective_time              );
     // COL_SAVE_STR(row, 1,   card, active_inactive_code        );
        COL_SAVE_LLU(row, 2,   card, issuer_acc_range_low        );
     // COL_SAVE_STR(row, 3,   card, issuer_acc_range_prefix     );
        COL_SAVE_STR(row, 4,   card, gcms_product_id             );
        COL_SAVE_LLU(row, 5,   card, issuer_acc_range_high       );
        COL_SAVE_STR(row, 6,   card, card_program_id             );
        COL_SAVE_STR(row, 7,   card, issuer_card_prog_id         );
        COL_SAVE_STR(row, 8,   card, number_id                   );
        COL_SAVE_STR(row, 9,   card, product_type_id             );
        COL_SAVE_STR(row, 10,  card, endpoint                    );
        COL_SAVE_STR(row, 11,  card, country_code_alpha          );
        COL_SAVE_INT(row, 12,  card, country_code_num            );
        COL_SAVE_STR(row, 13,  card, region                      );
        COL_SAVE_STR(row, 14,  card, product_class               );
        COL_SAVE_STR(row, 15,  card, trans_routing_id            );
        COL_SAVE_STR(row, 16,  card, present_reassign_switch     );
        COL_SAVE_STR(row, 17,  card, product_reassign_switch     );
        COL_SAVE_STR(row, 18,  card, pwcb_optin_switch           );
        COL_SAVE_STR(row, 19,  card, licensed_product_id         );
        COL_SAVE_STR(row, 20,  card, map_service_id              );
        COL_SAVE_STR(row, 21,  card, acc_level_man_part_id       );
        COL_SAVE_STR(row, 22,  card, acc_level_man_active_date   );
        COL_SAVE_STR(row, 23,  card, cardholder_bill_cur_def     );
        COL_SAVE_STR(row, 24,  card, cardholder_bill_cur_exp_def );
        COL_SAVE_STR(row, 25,  card, cardholder_bill_pri_cur     );
        COL_SAVE_STR(row, 26,  card, chip_to_mag_conv_sev_id     );
        COL_SAVE_STR(row, 27,  card, floor_exp_date              );
        COL_SAVE_STR(row, 28,  card, cobrand_part_switch         );
        COL_SAVE_STR(row, 29,  card, spend_con_switch            );
        COL_SAVE_STR(row, 30,  card, mer_clean_sev_part          );
        COL_SAVE_STR(row, 31,  card, mer_clean_act_date          );
        COL_SAVE_STR(row, 32,  card, paypass_enable_id           );
        COL_SAVE_STR(row, 33,  card, reg_rate_type_id            );
        COL_SAVE_STR(row, 34,  card, psn_route_id                );
        COL_SAVE_STR(row, 35,  card, cashback_without_pur_id     );
     // COL_SAVE_STR(row, 36,  card, create_time                 );
     // COL_SAVE_STR(row, 37,  card, modify_time                 );
        i++;
    }
    mysql_free_result(pResult);

    pMem[0] = shm_key;        // ok
    std::sort(pData, pData+iRow, cmp_struct);


    // 增加检测机制，因为包含关系的前后区间会导致bug
    for (i=1; i<iRow; i++) {
        if (pData[i-1].issuer_acc_range_high == pData[i].issuer_acc_range_high
            && pData[i-1].issuer_acc_range_low == pData[i].issuer_acc_range_low){   // 区间完全重叠，不需处理

            continue;

        }else if (pData[i-1].issuer_acc_range_high > pData[i].issuer_acc_range_high){ // 前后2个区间是包含关系

            // 是否仅区间上下限不同
            // 仅区间上下限不同情况，用大的覆盖小的区间 不需告警
            // 若不是，数据冲突了, 需要人工修复->告警或打印错误
            s_card_info_t tmpcard = {0};
            memcpy(&tmpcard, pData+i, sizeof(s_card_info_t));
            tmpcard.issuer_acc_range_low = pData[i-1].issuer_acc_range_low;
            tmpcard.issuer_acc_range_high = pData[i-1].issuer_acc_range_high;
            // 排除比较的字段
            strncpy(tmpcard.effective_time, pData[i-1].effective_time, sizeof(tmpcard.effective_time)-1);

            if (memcmp(&tmpcard, pData+i-1, sizeof(s_card_info_t)) == 0){
                gPtrAppLog->debug(" [%019llu,%019llu] vs [%019llu, %019llu] : L1<L2<H2<H1 -> delete small one",
                                  pData[i].issuer_acc_range_low, pData[i].issuer_acc_range_high,
                                  pData[i-1].issuer_acc_range_low, pData[i-1].issuer_acc_range_high );
                // 使用memcpy，将大区间复制一份，以覆盖小区间。好处：快
                memcpy(pData+i, pData+i-1, sizeof(s_card_info_t));

            }else{
                // 也有可能是这样:
                // [5,10](其他信息A),[5,10](其他信息B),[5,8](其他信息A),[5,8](其他信息B)

                // 告警, 打印错误的代码写在这里……
                gPtrAppLog->error(" [%019llu,%019llu] vs [%019llu, %019llu] : L1<L2<H2<H1 -> check if conflict data!",
                                  pData[i].issuer_acc_range_low, pData[i].issuer_acc_range_high,
                                  pData[i-1].issuer_acc_range_low, pData[i-1].issuer_acc_range_high );
                // 使用memcpy，将大区间复制一份，以覆盖小区间。好处：快
                memcpy(pData+i, pData+i-1, sizeof(s_card_info_t));
            }

        }else if (pData[i-1].issuer_acc_range_high > pData[i].issuer_acc_range_low){ // 前后2个区间是交叉关系

            gPtrAppLog->debug(" [%019llu,%019llu] vs [%019llu, %019llu] : L1<L2<H1<=H2 -> compatible-ok",
                              pData[i].issuer_acc_range_low, pData[i].issuer_acc_range_high,
                              pData[i-1].issuer_acc_range_low, pData[i-1].issuer_acc_range_high );
        }
    }


    // fix bug now
    if (pMem)
        shmdt(pMem);

    return i;

}   // end of load_shm()

// 更新共享内存，保证调用后只有一个key对应共享内存可用
int reflash_shms(CMySQL *pMysql, int shm_key1, int shm_key2)
{
    int shmid1 = shmget(shm_key1, 0, 0);
    if (shmid1 == -1) {
        int ret = load_shm(pMysql, shm_key1);
        int shmid2 = shmget(shm_key2, 0, 0);
        if (shmid2 == -1){
            gPtrAppLog->debug("both shm key unused!");
            return -1;
        }
        shmctl(shmid2, IPC_RMID, NULL);
        return ret;
    }else{
        int ret = load_shm(pMysql, shm_key2);
        shmctl(shmid1, IPC_RMID, NULL);
        return ret;
    }
}



// 删除所有共享内存
int clean_shms(int shm_key1, int shm_key2)
{
    int shmid1 = shmget(shm_key1, 0, 0);
    if (shmid1 != -1)
        shmctl(shmid1, IPC_RMID, NULL);
    int shmid2 = shmget(shm_key2, 0, 0);
    if (shmid2 != -1)
        shmctl(shmid2, IPC_RMID, NULL);
}
