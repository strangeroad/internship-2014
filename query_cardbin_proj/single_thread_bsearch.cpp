/*
 * =====================================================================================
 *
 *       Filename:  single_thread_bsearch.cpp
 *
 *    Description:  复制了single_thread_bsearch.c
 *
 *        Version:  1.0
 *        Created:  2014年09月18日 10时33分30秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wilsonwuwu(wws), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <mysql/mysql.h>

#include <algorithm>
using namespace std;

/*
 * unsigned long        最大指：4294967295          （10位）
 * unsigned long long   最大值：18446744073709551615（20位）
 * 差点接不住19位的卡号，19位不能用long long。
 *
 * %lld -> long long
 * %llu -> unsigned long long
 *
 * long long ll = 9199999999999999999;
 * unsigned long long llu = 9999999999999999999llu;
 * unsigned long long llu = 0xFFFFFFFFFFFFFFFF;
 * unsigned long long 其实应该是 long long unsigned 
 */

typedef struct s_record {
    unsigned long long   id;
    unsigned long long   low;
    unsigned long long   high;
    char        pro[4];
}s_record_t;

/* 第一个位置的high保存长度，第二个位置后才是数据 */
s_record_t* loadm();

s_record_t* binary_search(unsigned long long tag, s_record_t* arr, unsigned long long len);

void sort_arr(s_record_t *arr, unsigned long long len);

void print_arr(s_record_t *arr, int len);

int err_handle(const char* str);

/* 命令处理组件 */
void middle_handle(const char *buf_recv, char *buf, int len);

/* 数据库操作函数 */
MYSQL* connectdb();
void initdb();
void exitdb(MYSQL* mysql_handler);
int query(MYSQL* mysql_handler, const char* sql);


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
    /* unit test */
    s_record_t *pv = loadm(),
               *precord = pv + 1;
    unsigned long long 
        len = pv->high,
        ull_test[5] = {
            4022143900000000000,
            4022260000000000000, 
            7602000000000000000, 
            9933399999900000000llu, 
            6771348015000000000,
        };
    int i;
    for (i=0;i<5;i++){
        s_record_t *fin = binary_search(ull_test[i], precord, len);
        if (fin)
            printf("%llu in %llu:[%llu, %llu]\n", ull_test[i], fin->id, fin->low, fin->high);
        else
            printf("%llu not found\n", ull_test[i]);
    }
    free(pv);


    /* unit test */
    s_record_t *pv2 = loadm();
    pv = pv2;
    precord = pv + 1;
    len = pv->high;
    for (i=0;i<5;i++){
        ull_test[i] += 2222222222222222222;
        s_record_t *fin = binary_search(ull_test[i], precord, len);
        if (fin)
            printf("%llu in %llu:[%llu, %llu]\n", ull_test[i], fin->id, fin->low, fin->high);
        else
            printf("%llu not found\n", ull_test[i]);
    }
    free(pv);
    return 0;
}				/* ----------  end of function main  ---------- */
 
int err_handle(const char* str){
    printf("%s error: %s [%d]\n", str, strerror(errno), errno);
    exit(-1);
}

/* 数据库操作函数 */
MYSQL* connectdb(){
    MYSQL *mysql_handler = mysql_init(NULL);
    /* 
    MYSQL *mysql_real_connect(MYSQL *connection,
                const char *server_host,
                const char *sql_user_name,
                const char *sql_password,
                const char *db_name,
                unsigned int port_number,//置0连接默认端口，一般为3306
                const char *unix_socket_name,//NULL
                unsigned int flags);//无另外属性时置0
     */
    if (!mysql_real_connect(mysql_handler, "localhost", "public", "public", "ia_gcms_2014", 0, NULL, 0))
        err_handle("mysql_real_connect()");
    return mysql_handler;
}

void exitdb(MYSQL* mysql_handler){
    mysql_close(mysql_handler);
}


/* 封装一下mysql_query */
int query(MYSQL* mysql_handler, const char* sql){
    int err = mysql_query(mysql_handler, sql);
    if (err == 0)
        return err;
    printf("'%s' sql error: %s [%d]\n", sql, mysql_error(mysql_handler), mysql_errno(mysql_handler));
    exit(-1);
}


s_record_t* loadm(){
    /* 第一个位置的high保存长度，第二个位置后才是数据 */
    MYSQL *mysql_handler = connectdb();
    // const char *sql = "select Fid, Fgcms_product_id, Fissuer_acc_range_low, Fissuer_acc_range_high from t_IP0040T1 order by Fissuer_acc_range_low, Fgcms_product_id;";
    const char *sql = "select Fid, Fgcms_product_id, Fissuer_acc_range_low, Fissuer_acc_range_high from t_IP0040T1;";
    query(mysql_handler, sql);
    /* not mysql_use_result */
    MYSQL_RES *result = mysql_store_result(mysql_handler);

    unsigned long long size = mysql_num_rows(result);
    s_record_t 
        *precord = (s_record_t*)malloc((size+1)*sizeof(s_record_t)),
        *pnow = precord;
    memset(precord, 0, (size+1)*sizeof(s_record_t));
    pnow->high = size;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))){                                    /* 不停读行 */
        pnow += 1;
        sscanf(row[0], "%llu", &pnow->id);
        strncpy(pnow->pro, row[1], 3);
        sscanf(row[2], "%llu", &pnow->low);
        sscanf(row[3], "%llu", &pnow->high);
    }

	mysql_free_result(result);
    exitdb(mysql_handler);

    print_arr(precord+1, 3);
    sort_arr(precord+1, size);
    print_arr(precord+10000, 3);
    return precord;
}


int cmper(unsigned long long larg, s_record_t* rarg){
    if (larg < rarg->low)
        return -1;
    else if (larg > rarg->high)
        return +1;
    else
        return 0;
}

s_record_t* binary_search(unsigned long long tag, s_record_t* arr, unsigned long long len){
    unsigned long long
        low     = 0,
        high    = len,
        mid;
    while (low < high) {
        mid = (low+high)/2;
        int cmpout = cmper(tag, arr+mid);
        if (cmpout == 0)
            return arr+mid;
        else if (cmpout == 1)
            low = mid+1;
        else
            high = mid;
    }
    return NULL;
}


int cmpbt(const s_record_t &larg, const s_record_t &rarg){
    return larg.low <= rarg.low;
}


void sort_arr(s_record_t *arr, unsigned long long len){
    sort(arr, arr+len, cmpbt);
}

void print_arr(s_record_t *arr, int len){
    int i;
    for (i=0; i<len; i++) {
        printf("[%llu, %llu]\n", arr[i].low, arr[i].high);
    }
}
