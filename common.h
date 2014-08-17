/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  公用
 *
 *        Version:  1.0
 *        Created:  2014年08月17日 10时55分29秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  pythonwood (wws), 582223837@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

/* 
 * C++要求函数在使用前必须先有声明。这样可以在编译时找到更多的错误。
 * C在这一点上(还有很多其它地方)要求不太严格，如果一个函数未经声明就使用了，C编译器会认为这就是声明。这样容易通过编译，但是如果在运行的时候出现问题更让人头疼。
 * 用gcc -Wall server.c编译，会给出warning。
 * 
 * 用man找到几个函数所在的头文件，include就可以了。
 */
#include <unistd.h> // fork, close
#include <arpa/inet.h>  // inet_ntoa

#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/HTTPRedirectHeader.h"


#define MAXLINE 4096

using cgicc::Cgicc;
using std::string;

int err_handle(const char* str);

void send_reci(const char* buf_send, const int len_send, char *buf_recv, const int len_recv);

const string& getCookie(Cgicc &cgi, const string &name);

int if_login(Cgicc &cgi);
