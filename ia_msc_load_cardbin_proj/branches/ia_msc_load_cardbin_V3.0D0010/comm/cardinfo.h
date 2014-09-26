/*
 * =====================================================================================
 *
 *       filename:  cardinfo.h
 *
 *    Description:  处理从数据库读到内存卡信�?
 *
 *        Version:  1.0
 *        Created:  2014/9/14 15:18:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wilsonwuwu (wws),
 *   Organization:  tencent 
 *
 * =====================================================================================
 */

#include "sqlapi.h"
#include "stdlib.h"


// װ�����ݿ��¼�Ľṹ��
struct s_card_info_t
{
    char effective_time[10+1];
 // char active_inactive_code[1+1];
    unsigned long long issuer_acc_range_low;  // char issuer_acc_range_low[19+1];
 // char issuer_acc_range_prefix[4+1];
    char gcms_product_id[3+1];
    unsigned long long issuer_acc_range_high; // char issuer_acc_range_high[19+1];
    char card_program_id[3+1];
    char issuer_card_prog_id[2+1];
    char number_id[11+1];
    char product_type_id[1+1];
    char endpoint[7+1];
    char country_code_alpha[3+1];
    int  country_code_num;                     // char country_code_num[3+1];
    char region[1+1];
    char product_class[3+1];
    char trans_routing_id[1+1];
    char present_reassign_switch[1+1];
    char product_reassign_switch[1+1];
    char pwcb_optin_switch[1+1];
    char licensed_product_id[3+1];
    char map_service_id[1+1];
    char acc_level_man_part_id[1+1];
    char acc_level_man_active_date[6+1];
    char cardholder_bill_cur_def[3+1];
    char cardholder_bill_cur_exp_def[1+1];
    char cardholder_bill_pri_cur[28+1];
    char chip_to_mag_conv_sev_id[1+1];
    char floor_exp_date[6+1];
    char cobrand_part_switch[1+1];
    char spend_con_switch[1+1];
    char mer_clean_sev_part[3+1];
    char mer_clean_act_date[6+1];
    char paypass_enable_id[1+1];
    char reg_rate_type_id[1+1];
    char psn_route_id[1+1];
    char cashback_without_pur_id[1+1];
 // char create_time[19+1];
 // char modify_time[19+1];
};

// ���ҿ����õıȽϺ���
int cmper(unsigned long long larg, s_card_info_t* rarg, int country_code_num=0);

// ���ַ���������Ľṹ������
s_card_info_t* binary_search(unsigned long long tag, s_card_info_t* arr, int len, int country_code_num=0);

// �ṹ�����������õıȽϺ���
int cmp_struct(const s_card_info_t &larg, const s_card_info_t &rarg);



// װ�����ݿ⵽ʹ��shm_key���ٵĹ����ڴ���
int load_shm(CMySQL *pMysql, int shm_key);

// ���¹����ڴ棬��֤���ú�ֻ��һ��key��Ӧ�����ڴ����
int reflash_shms(CMySQL *pMysql, int shm_key1, int shm_key2);

// ɾ�����й����ڴ�
int clean_shms(int shm_key1, int shm_key2);

