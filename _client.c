/*
 * =====================================================================================
 *
 *       Filename:  _client.c
 *
 *    Description:  客户端socket
 *
 *        Version:  1.0
 *        Created:  2014年07月29日 09时48分59秒
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

#include <unistd.h> // fork, close
#include <arpa/inet.h>  // inet_ntoa

#define MAXLINE 4096

int err_handle(const char* str);

int main(int argc, char* argv[]){
    
    /* 默认参数 */
    /* int port = argc < 3 ? 8080 : sscanf(argv[2], "%d", &port); */
    int port = 8000;
    if (argc >=3 )
        sscanf(argv[2], "%d", &port);
    char ipstr[16] = "127.0.0.1";
    if (argc >=2 )
        strncpy(ipstr, argv[1], 16);

    int socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server == -1)
        err_handle("create");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); /* 小错误大痛苦：少了一句 */
    if ( inet_pton(AF_INET, ipstr, &serv_addr.sin_addr) <= 0 )
        err_handle("inet_pton");
    printf("connect to ip:%s, port:%d\n", ipstr, port);
    if (0 > connect(socket_server,
            (struct sockaddr*)&serv_addr,
            sizeof(serv_addr)) )
        err_handle("connect");

    char buf_recv[MAXLINE]="\0", buf_send[MAXLINE]="\0";
    while (1){
        printf("<<< ");
        /* scanf("%s", buf_send); */        /* 一、%s\n缓冲区会接纳一个\n！ 二、溢出？ */
        /* fgets(buf_send, MAXLINE, stdin); */      /* 有'\n' */
        gets(buf_send);         /* 没有'\n' */
        buf_send[MAXLINE] = '\0'; /* safe */
        if (send(socket_server, buf_send, MAXLINE, 0) < 0)
            err_handle("send");
        /* printf("send: %s ok\n", buf_send); just debug*/
        int len = recv(socket_server, buf_recv, MAXLINE, 0);
        if (len < 0)    err_handle("recv");
        if (len == 0)   break; /* 被动关闭？ */
        buf_recv[MAXLINE] = '\0'; /* safe */
        printf(">>> %s\n", buf_recv);
    }
    close(socket_server);
    return 0;
}

int err_handle(const char* str){
    printf("%s error: %s [%d]\n", str, strerror(errno), errno);
    exit(-1);
}

