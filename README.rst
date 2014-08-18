

v1.0        2014-07-29

 * CS socket 编程 ，简单命令式交互，仅实现echo功能。exit或CTRL+D关闭会话。

 * 目标：重拾C/C++编程能力


v2.0        2014-08-01

 * 命令式商店实现，增加数据库功能(product和deal两表)，增加4个命令list, buy, add, del。

 * 目标：数据库编程，之前看了mysql++，最后没有用到，至此都是使用 linux C。

v3.0        2014-08-15

 * 实现登录模块了。cgicc的cookie功能太少。

 * /txproj_index.html -> /cgi-bin/txproj_login -> (txproj_seller.html or txproj_buyer.html)

v4.0        2014-08-18

 * 登录，cookie & seesion
 
 * 使用jquery和bootstrap，既有表单提交，也有ajax提交。

 * 查看功能，分别对应txproj_buyer.html和txproj_seller.html两页面

 * init.sql             : 初始化mysql数据库的sql脚本。

 * common.h             : 项目公用头文件。

 * common.cpp           : 项目公用库，封装来cs通信的c端。

 * txproj_buyer.html    : 买家登录后掉页面。
   
 * txproj_seller.html   : 卖家登录后的页面。

 * txproj_login.cpp     : 登录功能cgi，解析post数据，成功则设置客户端cookie，cookie用于后面操作。

 * txproj_list.cpp      : 买家卖家订单信息查询cgi，返回json格式的数据。

 * txproj_pay.cpp       : 买家支付cgi，根据cookie和post过来的password_pay支付订单。

 * txproj_refund.cpp    : 卖家退款cgi，根据cookie等处理。`

 * txproj_seller.c      : 实际处理数据库的后台，cgi 不直接出来数据库。

 * Makefile             : 项目Makefile，可改进。

 * txproj_test.sh       : 项目自动化测试脚本，可改进。

 * _client.c            : 读取标准输入，向s端发指令交互。调试S端常用。

 * _server.c            : 简单的server端，实验性质。

 * doc/                 : 项目需求文档，设计文档等。

 * demo/                : 前期html 做的demo，实验性质。

