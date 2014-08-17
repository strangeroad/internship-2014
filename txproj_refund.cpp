/*
 * =====================================================================================
 *
 *       Filename:  txproj_refund.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年08月17日 20时07分11秒
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
        const_form_iterator 
            amount  = cgi.getElement("amount"),
            dealid  = cgi.getElement("dealid"),
            end     = cgi.getElements().end();
        if (type!="" && name!="" && token!="" && id!="" &&
                amount!=end && dealid!=end){
            string cmd = "refund|" + dealid->getValue() + '|' + amount->getValue() + '|' + id + '|' + token;
            send_reci(cmd.c_str(), cmd.size()+10, buf_recv, MAXLINE);
            int err = -1;
            char note[32] = "\0", strid[32]="\0";
            sscanf(buf_recv, "error:%d,note:%[^|]|%s", &err, strid, note);
            if (err==0){    // 登录后重定向
                cout << "Content-Type:application/json" << "\r\n\r\n";
                // 格式必须json正确，不如$.get[JSON]回调函数不执行，大坑！！！
                cout << "{\"error\":0}" << "\r\n";
                return 0;
            }
        } 
        // 其他情况，失败
        cout << HTTPHTMLHeader();
        cout << "login " << buf_recv << "\r\n";
        cout << "<script type=\"text/javascript\"> \
                alert(\"token error\");\
                location.href='/txproj_index.html';\
                </script>" << "\r\n";
    }
    catch(exception& e) {
       // handle any errors - omitted for brevity
    }
    return 0;
}
