/*
 * =====================================================================================
 *
 *       Filename:  _server.c
 *
 *    Description:  服务端 命令式商店CS实现，list,buy,add,del命令操作数据库，exit关闭会话。
 *
 *        Version:  1.1
 *        Created:  2014年07月31日 09时47分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wilsonwuwu (吴伟顺), 582223837@qq.com
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
#include <mysql.h>

#define MAXLINE 4096

int err_handle(const char* str);

/* 命令处理组件 */
void middle_handle(const char *buf_recv, char *buf, int len);

/* 数据库操作函数 */
MYSQL* connectdb();
void initdb();
void exitdb(MYSQL* mysql_handler);
int query(MYSQL* mysql_handler, const char* sql);
void list(const char *recv, char *buf, int len);
void buy(const char *recv, char *buf, int len);
void add(const char *recv, char *buf, int len);
void del(const char *recv, char *buf, int len);




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
    if      (0==strncmp(buf_recv, "list", 4))   list(buf_recv, buf, len);
    else if (0==strncmp(buf_recv, "buy", 3))    buy(buf_recv, buf, len);
    else if (0==strncmp(buf_recv, "add", 3))    add(buf_recv, buf, len);
    else if (0==strncmp(buf_recv, "del", 3))    del(buf_recv, buf, len);
    else
    {
        const char *s = "echo command if unknow: ";
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
    mysql_real_connect(mysql_handler, "localhost", "public", "public", "test", 0, NULL, 0);
    return mysql_handler;
}
void initdb(){
    MYSQL* mysql_handler = connectdb();
    const char 
        *sql_dr_product = "drop table if exists product;",
        *sql_tb_product = "create table if not exists product (id int auto_increment,\
                                                    name varchar(64) not NULL,\
                                                    price double not NULL,\
                                                    unit varchar(32) not NULL,\
                                                    primary key (id));",
        *sql_fl = "insert into product (name, price, unit)\
                   values\
                                            ('apple', 12, '元/斤'),\
                                            ('qqnum', 8,  '美元/个'),\
                                            ('tshirt', 32, '元/件');",
        *sql_dr_deal = "drop table if exists deal;",
        *sql_tb_deal = "create table if not exists deal (id int auto_increment,\
                                                    pid int not NULL,\
                                                    amount int not NULL,\
                                                    time datetime not NULL,\
                                                    primary key (id));";
    query(mysql_handler, sql_dr_product);
    query(mysql_handler, sql_tb_product);
    query(mysql_handler, sql_fl);
    query(mysql_handler, sql_dr_deal);
    query(mysql_handler, sql_tb_deal);
    printf("initdb() exec ok...\n");
    exitdb(mysql_handler);
}
void exitdb(MYSQL* mysql_handler){
    mysql_close(mysql_handler);
}
int query(MYSQL* mysql_handler, const char* sql){
    int err = mysql_query(mysql_handler, sql);
    if (err == 0)
        return err;
    printf("'%s' sql error: %s [%d]\n", sql, mysql_error(mysql_handler), mysql_errno(mysql_handler));
    exit(-1);
}
/* 命令：list  -->  返回商品数据 */
void list(const char *recv, char *buf, int len){        /* 使用strncat防止过界 */
    MYSQL* mysql_handler = connectdb();
    const char *sql = "select * from product";
    query(mysql_handler, sql);
    MYSQL_RES *result = mysql_use_result(mysql_handler);
    unsigned i,ncol = mysql_field_count(mysql_handler);   /* 列数 */
    MYSQL_FIELD *field = mysql_fetch_field(result);

    /* 表头 */
    strncat(buf, "items:\n", 7);
    while (field){
        strncat(buf, field->name, len-strlen(buf)-2);
        strncat(buf, " ", 1);
        field = mysql_fetch_field(result);
    }
    strncat(buf, "\n", 1);

    /* 表 */
    MYSQL_ROW *row = mysql_fetch_row(result);
    while (row){                                        /* 不停读行 */
        for (i=0; i<ncol; i++){
            if (row[i])
                strncat(buf, row[i], len-strlen(buf)-2);
            else
                strncat(buf, "NULL", len-strlen(buf)-2);
            strncat(buf, " ", 1);
        }
        if (strlen(buf) + 1 >= len)                 /* 缓冲区已满 */
            break;
        strncat(buf, "\n", 1);
        row = mysql_fetch_row(result);
    }
    exitdb(mysql_handler);
}
/* 命令：buy 3 20  -->  购买 3号产品 20份*/
void buy(const char *recv, char *buf, int len){
    MYSQL* mysql_handler = connectdb();
    int pid, amount;
    sscanf(recv, "buy %d %d", &pid, &amount);
    char sql[MAXLINE] = "\0";
    sprintf(sql, "insert into deal (pid, amount, time) values (%d, %d, now());", pid, amount);
    query(mysql_handler, sql);
    exitdb(mysql_handler);
}
/* 命令：add Q币 1 元/个  -->  增加商品 */
void add(const char *recv, char *buf, int len){
    MYSQL* mysql_handler = connectdb();
    double price;
    char name[32]="\0", unit[32]="\0", sql[MAXLINE] = "\0";
    sscanf(recv, "add %s %lf %s", name, &price, unit);
    sprintf(sql, "insert into product (name, price, unit) values ('%s', %f, '%s');", name, price, unit);
    query(mysql_handler, sql);
    exitdb(mysql_handler);
}
/* 命令：del 1  -->  删除 1号商品 */
void del(const char *recv, char *buf, int len){
    MYSQL* mysql_handler = connectdb();
    int id;
    sscanf(recv, "del %d", &id);
    char sql[MAXLINE] = "\0";
    sprintf(sql, "delete from product where id=%d;", id);
    query(mysql_handler, sql);
    exitdb(mysql_handler);
}

