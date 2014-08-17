/*
 * =====================================================================================
 *
 *       Filename:  common.cpp
 *
 *    Description:  公用部分
 *
 *        Version:  1.0
 *        Created:  2014年08月17日 10时55分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  pythonwood (wws), 582223837@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include "common.h"
using namespace std;
using namespace cgicc;


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

