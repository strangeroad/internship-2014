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
#include "common.h"

using namespace std;
using namespace cgicc;


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
            char note[32] = "\0", strid[32]="\0";
            sscanf(buf_recv, "error:%d,note:%[^|]|%s", &err, strid, note);
            if (err==0){    // 登录后重定向
                if (type->getValue() == "2") {
                    cout << HTTPRedirectHeader("/txproj_buyer.html", 1)
                        // .setCookie(HTTPCookie("name", name, "", "localhost", 0, "/", false))
                        .setCookie(HTTPCookie("name", name->getValue()))
                        .setCookie(HTTPCookie("token",note))
                        .setCookie(HTTPCookie("id",strid))
                        .setCookie(HTTPCookie("type",type->getValue()));
                    return 0;
                } else if (type->getValue() == "1") {
                    cout << HTTPRedirectHeader("/txproj_seller.html", 1)
                        .setCookie(HTTPCookie("name", name->getValue()))
                        .setCookie(HTTPCookie("token",note))
                        .setCookie(HTTPCookie("id",strid))
                        .setCookie(HTTPCookie("type",type->getValue()));
                    return 0;
                }
            }
        }

        /* 查cookie是否已登录 */
        if (type==end && name==end && password==end && if_login(cgi)){
            string _type = getCookie(cgi, "type");
            if (_type == "2"){
                cout << HTTPRedirectHeader("/txproj_buyer.html", 1);
                return 0;
            } else if (_type == "1") {
                cout << HTTPRedirectHeader("/txproj_seller.html", 1);
                return 0;
            }
        }

        // 其他情况，失败
        cout << HTTPHTMLHeader();
        cout << "<script type=\"text/javascript\"> \
                alert(\"login " << buf_recv << "\");\
                location.href='/txproj_index.html';\
                </script>" << "\r\n";
    }
    catch(exception& e) {
       // handle any errors - omitted for brevity
    }
    return 0;
}

