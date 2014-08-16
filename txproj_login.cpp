/*
 * =====================================================================================
 *
 *       Filename:  txproj_login.cpp
 *
 *    Description:  -c屏蔽警告
 *
 *        Version:  1.0
 *        Created:  2014年08月13日 16时08分53秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include <fstream>

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

using namespace std;
using namespace cgicc;


int err_handle(const char* str);

void send_reci(const char* buf_send, const int len_send, char *buf_recv, const int len_recv);

const string& getCookie(Cgicc &cgi, const string &name);

int if_login(Cgicc &cgi);

int main(int argc, char *argv[])
{
    try {
        Cgicc cgi;
        char buf_send[MAXLINE]="\0", buf_recv[MAXLINE]="\0";
        const_form_iterator
            name    = cgi.getElement("name"),
            type    = cgi.getElement("type"),
            password= cgi.getElement("password"),
            end     = cgi.getElements().end();
        string token, cmd; 
        // cout << HTTPHTMLHeader() << type << name << token << "\r\n";
        // return 0;

        /* 参数齐全情况，即使已登录没问题 */
        if (type!=end && name!=end && password!=end){
            cmd = "login|" + type->getValue() + '|' + name->getValue() + '|' + password->getValue();
            send_reci(cmd.c_str(), cmd.size()+10, buf_recv, MAXLINE);
            int err = -1;
            char note[32] = "\0";
            sscanf(buf_recv, "error:%d,note:%s", &err, note);
            if (err==0){    // 登录后重定向
                if (type->getValue() == "1") {
                    cout << HTTPRedirectHeader("/txproj_buyer.html", 1)
                        // .setCookie(HTTPCookie("name", name, "", "localhost", 0, "/", false))
                        .setCookie(HTTPCookie("name", name->getValue()))
                        .setCookie(HTTPCookie("token",note))
                        .setCookie(HTTPCookie("type",type->getValue()));
                    return 0;
                } else if (type->getValue() == "0") {
                    cout << HTTPRedirectHeader("/txproj_seller.html", 1)
                        .setCookie(HTTPCookie("name", name->getValue()))
                        .setCookie(HTTPCookie("token",note))
                        .setCookie(HTTPCookie("type",type->getValue()));
                    return 0;
                }
            }
        }

        /* 查cookie是否已登录 */
        if (type==end && name==end && password==end && if_login(cgi)){
            string _type = getCookie(cgi, "type");
            if (_type == "1"){
                cout << HTTPRedirectHeader("/txproj_buyer.html", 1);
                return 0;
            } else if (_type == "0") {
                cout << HTTPRedirectHeader("/txproj_seller.html", 1);
                return 0;
            }
        }

        // 其他情况，失败
        cout << HTTPHTMLHeader();
        cout << "login " << buf_recv << "\r\n";
        cout << "<script type=\"text/javascript\"> \
                alert(\"login error\");\
                location.href='/txproj_index.html';\
                </script>" << "\r\n";
    }
    catch(exception& e) {
       // handle any errors - omitted for brevity
    }
    return 0;
}


void send_reci(const char* buf_send, const int len_send, char *buf_recv, const int len_recv){
    int port = 8000;
    char ipstr[16] = "localhost";

    int socket_server = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_server == -1)
        err_handle("create");

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); /* 小错误大痛苦：少了一句 */
    if ( inet_pton(AF_INET, ipstr, &serv_addr.sin_addr) < 0 )
        err_handle("inet_pton");
    if (0 > connect(socket_server,
            (struct sockaddr*)&serv_addr,
            sizeof(serv_addr)) )
        err_handle("connect");

    if (send(socket_server, buf_send, len_send, 0) < 0)
        err_handle("send");
    int len = recv(socket_server, buf_recv, len_recv, 0);
    if (len < 0)    err_handle("recv");
    buf_recv[len_recv] = '\0'; /* safe */
    close(socket_server);
}

int err_handle(const char* str){
    printf("%s error: %s [%d]\n", str, strerror(errno), errno);
    exit(-1);
}


// 函数对象
class CK_nameCompare : public std::unary_function<cgicc::HTTPCookie, bool>
{
public:
  
    inline explicit CK_nameCompare(const std::string& name)
      : fName(name) {}
    
    inline bool operator() (const argument_type& c) 	const
    { return c.getName()==fName;}
  
private:
    std::string fName;
};

const string& getCookie(Cgicc &cgi, const string &name){
    const CgiEnvironment& env = cgi.getEnvironment();
    std::vector<cgicc::HTTPCookie>::const_iterator
        begin   = env.getCookieList().begin(),
        end     = env.getCookieList().end(),
        findit;
    findit = std::find_if(begin, end, CK_nameCompare(name));
    if (findit == end)
        return "";
    return findit->getValue();
}

int if_login(Cgicc &cgi){
    char buf_send[MAXLINE]="\0", buf_recv[MAXLINE]="\0";
    if (cgi.getEnvironment().getCookies().empty())
        return 0;
    string  type = getCookie(cgi, "type"),
            name = getCookie(cgi, "name"),
            token= getCookie(cgi, "token");
    if (type.empty() || name.empty() || token.empty())
        return 0;
    string  cmd  = "token|" + type + '|' + name + '|' + token;
    send_reci(cmd.c_str(), cmd.size()+10, buf_recv, MAXLINE);
    int err = -1;
    char note[32] = "\0";
    sscanf(buf_recv, "error:%d,note:%s", &err, note);
    return err == 0;
}
