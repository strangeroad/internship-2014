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
 * ��Ȩ���Ǽǽӿ�
 */
class QueryCardBinServer
{
public:

    QueryCardBinServer();
    ~QueryCardBinServer();

    /**
     * �����������
     */
    void parseInputMsg(TRPC_SVCINFO* pRequst) throw(CException);

    /**
     * ��װ������Ϣ
     */
    void packReturnMsg(TRPC_SVCINFO* pRequst);

    /**
     * ִ�в���
     */
    void excute() throw(CException);

protected:

    /**
      * ������
      */
    void checkParam();
    /**
      * ��Ȩ�Ǽ�
      */
    void find();

private:

    /**
      * ��Ϣ����
      */
    CParams m_Params;

    void loadShm();
    void unloadShm();

    const s_card_info_t *m_pCard;
    int m_nShmId;
    void *m_pMem;
    /*
     * ���Թ���long long unsigned ��Ϊ���Ա�� ʹ��sscanf����ֵ����ʧ�ܵġ�
     * ������Ϊ�ֲ�����long long unsignedȷ�ɹ�����C++���Ա����й���
     * long long unsigned m_nCardnum;
     */
};

#endif

