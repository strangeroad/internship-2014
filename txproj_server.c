/*
 * =====================================================================================
 *
 *       Filename:  txproj_server.c
 *
 *    Description:  connect mysql 注意：需要防注入，密码不使用明文
 *
 *        Version:  1.0
 *        Created:  2014年08月13日 17时05分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <mysql/mysql.h>
#include <time.h>
#include <unistd.h> // fork, close
#include <arpa/inet.h>  // inet_ntoa

#define MAXLINE 4096
enum person_type
{
    SELLER  = 1,
    BUYER   = 2
};


int err_handle(const char* str);

/* 命令处理组件 */
void middle_handle(const char *buf_recv, char *buf, int len);

/* 数据库操作函数 */
MYSQL* connectdb();
void initdb();
void exitdb(MYSQL* mysql_handler);
int query(MYSQL* mysql_handler, const char* sql);


/* 使用命令通信，命令处理函数 */
/* 命令：token type name token 返回 */
void token(const char *recv, char *buf, int len);
/* 命令：login type name password 返回 */
void login(const char *recv, char *buf, int len);
/* 命令：pay deal_id buyer_id seller_id amount 返回 */
void pay(const char *recv, char *buf, int len);
/* 命令：refund dealid buyer seller amount */
void refund(const char *recv, char *buf, int len);
/* 命令：list type name token */
void list(const char *recv, char *buf, int len);

int check_token_with_id(MYSQL* mysql_handler, int type, int id, char *token);
int check_token_with_name(MYSQL* mysql_handler, int type, char *name, char *token);

/* 释放当前查询的所有结果集. 否则下次查询将会出错. */
void release_after_select(MYSQL* mysql_handler, MYSQL_RES *result);



int main(int argc, char* argv[]){
    initdb();
    /* socket bind listen accept close */

    int port = 8000;
    if (argc >=2 )
        sscanf(argv[1], "%d", &port);

    int socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server == -1)
        err_handle("create");
    
    struct sockaddr_in  serv_addr, client_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    /* 字节序： 人左到有，CPU低位到高位，四字决“大同小异”，网络序是小端 */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    if (bind(socket_server, (struct sockaddr*)&serv_addr, sizeof(serv_addr))< 0)
        err_handle("bind");
    /* 出现bind error情况：服务断强制结束后一小段时间内重开  bind error: Address already in use [98] */

    if (listen(socket_server, 10000) == -1)
        err_handle("listen");

    printf("listen at port %d ...\n", port);

    while (1){

        socklen_t addrlen = 0;
        /* 
        int socket_client = accept(socket_server,
                (struct sockaddr*)&client_addr,
                &addrlen);
         */
        int socket_client = accept(socket_server,
                (struct sockaddr*)NULL,
                NULL);
        printf("new session [socket:%d]\n", socket_client); 

        char buf_recv[MAXLINE]="\0", buf_send[MAXLINE]="\0";
        int len = 0;
        while (1){
            len = recv(socket_client, buf_recv, MAXLINE, 0);
            if (len < 0)    err_handle("recv");
            /* CTRL+D时，len>0 */
            if (len == 0 || buf_recv[0]=='\0')   break;

            /* 不用buf_recv[len] = '\0' 测试显示 自动就有了 */
            buf_recv[MAXLINE] = '\0'; /* safe */
            printf("recv: %s [bytes:%d]\n", buf_recv, len);
            if (0 == strcmp(buf_recv, "exit"))
                break;

            /* just echo */
            /* strncpy(buf_send, buf_recv, MAXLINE); */
            middle_handle(buf_recv, buf_send, MAXLINE);
            /* printf("send:%s\n", buf_send); */

            buf_send[MAXLINE] = '\0'; /* safe */
            if (send(socket_client, buf_send, MAXLINE, 0) < 0)
                err_handle("send");
        }

        close(socket_client);
    }

    return 0;
}


int err_handle(const char* str){
    printf("%s error: %s [%d]\n", str, strerror(errno), errno);
    exit(-1);
}


void middle_handle(const char *buf_recv, char *buf, int len){
    if      (0==strncmp(buf_recv, "token", 5))      token(buf_recv, buf, len);
    else if (0==strncmp(buf_recv, "login", 5))      login(buf_recv, buf, len);
    else if (0==strncmp(buf_recv, "pay", 3))        pay(buf_recv, buf, len);
    else if (0==strncmp(buf_recv, "refund", 6))     refund(buf_recv, buf, len);
    else if (0==strncmp(buf_recv, "list", 4))       list(buf_recv, buf, len);
    else
    {
        const char *s = "error:100,note:echo command if unknow ";
        strncat(buf, s, strlen(s));
        strncat(buf, buf_recv, MAXLINE-strlen(s));
    }
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
    mysql_real_connect(mysql_handler, "localhost", "public", "public", "public0", 0, NULL, 0);
    return mysql_handler;
}
void initdb(){
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
/* 命令：token type name token 返回 */
/* 影响表：
 * txproj_buyer | txproj_seller : select
 */
void token(const char *recv, char *buf, int len){
    MYSQL* mysql_handler = connectdb();
    char name[32], token[32], sql[MAXLINE];
    int type;
    sscanf(recv, "token|%d|%[^|]|%[^\n]", &type, name, token);
    if (check_token_with_name(mysql_handler, type, name, token))
        snprintf(buf, len, "error:0,note:true");
    else 
        snprintf(buf, len, "error:1,note:token error");
    exitdb(mysql_handler);
}
/* 命令：login type name password 返回 */
/* 影响表：
 * txproj_buyer | txproj_seller : select & update
 */
void login(const char *recv, char *buf, int len){
    /* 实际中绝不明文保存密码，CSDN为鉴 */
    MYSQL* mysql_handler = connectdb();
    char name[32], password[32]="\0", sql[MAXLINE], token[32];
    int type;
    sscanf(recv, "login|%d|%[^|]|%[^\n]", &type, name, password);
    if (type==BUYER)
        snprintf(sql, MAXLINE, "select id from txproj_buyer where name='%s' and password='%s'", name, password);
    else if (type==SELLER)
        snprintf(sql, MAXLINE, "select id from txproj_seller where name='%s' and password='%s'", name, password);
    else {
        snprintf(buf, MAXLINE, "error:8,note:type error");
        return;
    }
    query(mysql_handler, sql);
    MYSQL_RES *result = mysql_use_result(mysql_handler);
    MYSQL_ROW *row = (MYSQL_ROW *)mysql_fetch_row(result);
    if (!row){
        snprintf(buf, MAXLINE, "error:2,note:name or password wrong");
        return;
    }

    /* 生成token */
    srand(time(0));
    snprintf(token, 32, "%d", rand());
    strncat(token, password, 32-strlen(token));
    token[31] = '\0';

    int id=0;
    sscanf((const char*)row[0], "%d", &id);

    release_after_select(mysql_handler, result);

    if (type==BUYER)
        snprintf(sql, MAXLINE, "update txproj_buyer set token='%s' where id=%d", token, id);
    else if (type==SELLER)
        snprintf(sql, MAXLINE, "update txproj_seller set token='%s' where id=%d", token, id);
    else {
        snprintf(buf, MAXLINE, "error:8,note:type error");
        return;
    }
    query(mysql_handler, sql);
    exitdb(mysql_handler);

    snprintf(buf, MAXLINE, "error:0,note:%d|%s", id, token);
}
/* 命令：pay deal_id buyer_id seller_id amount 返回 */
/* 影响表：
 * txproj_deal : select
 * txproj_pay  : insert
 * txproj_deal : update
 */
void pay(const char *recv, char *buf, int len){
    MYSQL* mysql_handler = connectdb();
    long long amount;
    int deal_id, buyer_id, seller_id;
    char sql[MAXLINE] = "\0", token[32]="\0", password_pay[32]="\0";
    sscanf(recv, "pay|%d|%d|%d|%lld|%[^|]|%[^\n]", &deal_id, &buyer_id, &seller_id, &amount, token, password_pay);
    if (!check_token_with_id(mysql_handler, BUYER, buyer_id, token)){
        snprintf(buf, MAXLINE, "error:1,note:token error");
        return;
    }
    snprintf(sql, MAXLINE, "select status from txproj_deal where id=%d and buyer_id=%d and seller_id=%d and amount=%lld",
            deal_id, buyer_id, seller_id, amount);
    query(mysql_handler, sql);
    MYSQL_RES *result = mysql_use_result(mysql_handler);
    MYSQL_ROW *row = (MYSQL_ROW *)mysql_fetch_row(result);
    if (!row){
        snprintf(buf, MAXLINE, "error:3,note: no such deal, maybe arguments wrong");
        return;
    }
    int status=0;
    sscanf((const char*)row[0], "%d", &status);
    if (status>1){
        snprintf(buf, MAXLINE, "error:4,note: has been paid");
        return;
    }
    
    release_after_select(mysql_handler, result);

    snprintf(sql, MAXLINE, "insert into txproj_pay (deal_id, buyer_id, seller_id, amount, pay_time) values \
            (%d, %d, %d, %lld, now());", deal_id, buyer_id, seller_id, amount);
    query(mysql_handler, sql);
    snprintf(sql, MAXLINE, "update txproj_deal set status=2 where id=%d", deal_id);
    query(mysql_handler, sql);
    snprintf(buf, MAXLINE, "error:0,note:%lld", amount);
    exitdb(mysql_handler);
}
/* 命令：refund dealid buyer seller amount */
/* 影响表：
 * txproj_deal      : select
 * txproj_refund    : insert
 * txproj_deal      : update
 */
void refund(const char *recv, char *buf, int len){
    MYSQL* mysql_handler = connectdb();
    long long amount;
    int deal_id, buyer_id, seller_id;
    char sql[MAXLINE] = "\0", token[32]="\0";
    sscanf(recv, "refund|%d|%d|%d|%lld|%s", &deal_id, &buyer_id, &seller_id, &amount, token);
    if (!check_token_with_id(mysql_handler, SELLER, seller_id, token)){
        snprintf(buf, MAXLINE, "error:1,note:token error");
        return;
    }
    snprintf(sql, MAXLINE, "select status, amount, refund from txproj_deal where id=%d and buyer_id=%d and seller_id=%d",
            deal_id, buyer_id, seller_id);
    query(mysql_handler, sql);
    MYSQL_RES *result = mysql_use_result(mysql_handler);
    MYSQL_ROW *row = (MYSQL_ROW *)mysql_fetch_row(result);
    if (!row){
        snprintf(buf, MAXLINE, "error:3,note: no such deal, maybe arguments wrong");
        return;
    }
    int status=0;
    long long pay=0, refund=0;
    sscanf((const char*)row[0], "%d", &status);
    sscanf((const char*)row[1], "%lld", &pay);
    sscanf((const char*)row[2], "%lld", &refund);
    if (status==1){
        snprintf(buf, MAXLINE, "error:7,note: has not been paid");
        return;
    }
    /* 能否多次退款的开关 */
    if (status==3){
        snprintf(buf, MAXLINE, "error:5,note: has been refund");
        return;
    }
    long long left = pay - refund;
    if (amount > left){
        snprintf(buf, MAXLINE, "error:6,note:max=%lld", left);
        return;
    }

    release_after_select(mysql_handler, result);

    snprintf(sql, MAXLINE, "insert into txproj_refund (deal_id, buyer_id, seller_id, amount, refund_time) values \
            (%d, %d, %d, %lld, now());", deal_id, buyer_id, seller_id, amount);
    query(mysql_handler, sql);
    snprintf(sql, MAXLINE, "update txproj_deal set status=3, refund=refund+%lld where id=%d", amount, deal_id);
    query(mysql_handler, sql);
    snprintf(buf, MAXLINE, "error:0,note:%lld", amount);
    exitdb(mysql_handler);
}


int check_token_with_id(MYSQL* mysql_handler, int type, int id, char *token){
    char sql[MAXLINE]="\0";
    if (type==BUYER)
        snprintf(sql, MAXLINE, "select token from txproj_buyer where id=%d", id);
    else if (type==SELLER)
        snprintf(sql, MAXLINE, "select token from txproj_seller where id=%d", id);
    else {
        return 0;
    }
    query(mysql_handler, sql);
    MYSQL_RES *result = mysql_use_result(mysql_handler);
    if (!result)
        err_handle("mysql result null");
    MYSQL_ROW *row = (MYSQL_ROW *)mysql_fetch_row(result);
    int ret = (int)(row && 0==strcmp((const char*)row[0], token));

    release_after_select(mysql_handler, result);

    return ret;
}

int check_token_with_name(MYSQL* mysql_handler, int type, char *name, char *token){
    char sql[MAXLINE]="\0";
    if (type==BUYER)
        snprintf(sql, MAXLINE, "select token from txproj_buyer where name='%s'", name);
    else if (type==SELLER)
        snprintf(sql, MAXLINE, "select token from txproj_seller where name='%s'", name);
    else {
        return 0;
    }
    query(mysql_handler, sql);
    MYSQL_RES *result = mysql_use_result(mysql_handler);
    if (!result)
        err_handle("mysql result null");
    MYSQL_ROW *row = (MYSQL_ROW *)mysql_fetch_row(result);
    int ret = (int)(row && 0==strcmp((const char*)row[0], token));

    release_after_select(mysql_handler, result);

    return ret;
}

void release_after_select(MYSQL* mysql_handler, MYSQL_RES *result){
    mysql_free_result(result);
    /* 释放当前查询的所有结果集. 否则下次查询将会出错. */
    while (!mysql_next_result(mysql_handler))
    {
        result = mysql_store_result(mysql_handler);
        mysql_free_result(result);
    }
}


/* 命令：list type name token */
void list(const char *recv, char *buf, int len){
    MYSQL* mysql_handler = connectdb();
    char name[32], token[32], sql[MAXLINE];
    int type, id;
    sscanf(recv, "list|%d|%d|%s", &type, &id, token);
    if (!check_token_with_id(mysql_handler, type, id, token)){
        snprintf(buf, len, "error:1,note:token error");
        return;
    }


    const char* other;
    if (type==BUYER) {
        snprintf(sql, MAXLINE, "select a.id, b.name, a.amount, a.refund, a.create_time, a.status from txproj_deal as a, txproj_buyer as b where a.buyer_id=b.id and b.id=%d", id);
        other = "seller";
    } else if (type==SELLER) {
        snprintf(sql, MAXLINE, "select a.id, b.name, a.amount, a.refund, a.create_time, a.status from txproj_deal as a, txproj_seller as b where a.seller_id=b.id and b.id=%d", id);
        other = "buyer";
    } else {
        snprintf(buf, MAXLINE, "error:8,note:type error");
        return;
    }
    
    printf("%s\n", sql);
    int size = query(mysql_handler, sql);
    MYSQL_RES *result = mysql_use_result(mysql_handler);
    unsigned i,ncol = mysql_field_count(mysql_handler);   /* 列数 */

    char data[MAXLINE] = "\0",
         line[MAXLINE] = "\0";
    int left = sizeof(data);


    /* 表 */
    MYSQL_ROW *row = (MYSQL_ROW*)mysql_fetch_row(result);
    if (row)
        printf("%s,%s\n", (const char*)row[0], token); 
    while (row){                                        /* 不停读行 */
        snprintf(line, MAXLINE, 
                "{\"id\":%s, \"%s\":%s, \"amount\":%s, \"refound\":%s, \"create_time\":%s, \"status\":%s}",
                (const char*)row[0], other, (const char*)row[1],
                (const char*)row[2],(const char*)row[3],(const char*)row[4],(const char*)row[5]);
        if (strlen(line) >= left)                 /* 缓冲区已满 */
            break;
        strncat(data, line, left); 
        left -= strlen(line);
        strncat(data, ",", 1);
        row = (MYSQL_ROW*)mysql_fetch_row(result);
    }
    data[strlen(data)-1] = '\0';

    snprintf(buf, len, "{\"error\":0,\"type\":%d, \"len\":%d, \"data\":[%s]}", type, size, data);
    exitdb(mysql_handler);
}
