/*
 * =====================================================================================
 *
 *       Filename:  QueryCardBinServer.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2014/9/14 16:19:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wilsonwuwu (wws),
 *   Organization:  tencent
 *
 * =====================================================================================
 */
#ifndef _IA_AUTH_REG_SERVICE_
#define _IA_AUTH_REG_SERVICE_

#include "appcomm.h"
#include "sqlapi.h"
#include "cardinfo.h"

/**
 * 授权单登记接口
 */
class QueryCardBinServer
{
public:

    QueryCardBinServer();
    ~QueryCardBinServer();

    /**
     * 解析输入参数
     */
    void parseInputMsg(TRPC_SVCINFO* pRequst) throw(CException);

    /**
     * 组装返回消息
     */
    void packReturnMsg(TRPC_SVCINFO* pRequst);

    /**
     * 执行操作
     */
    void excute() throw(CException);

protected:

    /**
      * 检查参数
      */
    void checkParam();
    /**
      * 授权登记
      */
    void find();

private:

    /**
      * 消息参数
      */
    CParams m_Params;

    void loadShm();
    void unloadShm();

    const s_card_info_t *m_pCard;
    int m_nShmId;
    void *m_pMem;
    /*
     * 测试过，long long unsigned 作为类成员， 使用sscanf设置值，是失败的。
     * 但是作为局部变量long long unsigned确成功，与C++语言本身有关吗？
     * long long unsigned m_nCardnum;
     */
};

#endif

