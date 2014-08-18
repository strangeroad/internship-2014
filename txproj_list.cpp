/*
 * =====================================================================================
 *
 *       Filename:  txproj_list.cpp
 *
 *    Description:  返回json
 *
 *        Version:  1.0
 *        Created:  2014年08月17日 10时53分30秒
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


int main(int argc, char *argv[])
{
    try {
        Cgicc cgi;
        char buf_send[MAXLINE]="\0", buf_recv[MAXLINE]="\0";
        string
            name    = getCookie(cgi, "name"),   //"rich",           //
            type    = getCookie(cgi, "type"),   //"2",              //
            token   = getCookie(cgi, "token"),  //"1080151822rich", //
            id      = getCookie(cgi, "id");     //"1000003";        //
        if (type!="" && name!="" && token!="" && id!=""){
        // if (type=="" || name=="" || token=="" || id==""){
            string cmd = "list|" + type + '|' + id + '|' + token;
            send_reci(cmd.c_str(), cmd.size()+10, buf_recv, MAXLINE);
            // 有没有长度都行的
            // 有但不对会误导浏览器，使执行时间长达15秒！！！！
            // cout << "Content-Length:" << strlen(buf_recv)+2 << "\r\n";
            cout << "Content-Type:application/json" << "\r\n\r\n";
            // 格式必须json正确，不如$.get[JSON]回调函数不执行，大坑！！！
            cout << buf_recv << "\r\n";
            // cout << "{\"error\":0}" << "\r\n";
            // cout << "{\"error\":0,\"type\":2, \"len\":0, \"data\":[]}" << "\r\n";
            // cout << "{\"error\":0,\"type\":2, \"len\":0, \"data\":[{\"test\":1}]}" << "\r\n";
        } else {
            // 其他情况，失败
            cout << HTTPHTMLHeader();
            cout << "login " << buf_recv << "\r\n";
            cout << "<script type=\"text/javascript\"> \
                    alert(\"token error\");\
                    location.href='/txproj_index.html';\
                    </script>" << "\r\n";
        }

    }
    catch(exception& e) {
       // handle any errors - omitted for brevity
    }
    return 0;
}


