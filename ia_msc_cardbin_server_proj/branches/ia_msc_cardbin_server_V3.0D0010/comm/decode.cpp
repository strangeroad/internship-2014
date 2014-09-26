/*
	FileName: decode.cpp 
	Author: richardzhao
	Version :1.0
	Date: 2005-5-3
	Description: Check Input Param  
	Function List:	GenerateDigest();
			CheckDigest();
	Called By: All Service; 
	Return: 
	History: 

*/
#include "decode.h"

#define ABSTRACT                       "abstract"
#define MAX_TRPC_LENGTH          10240 * 32
int GenerateDigest(char* src, char* dest, int dsize)
{
    int len = 0;
    unsigned char szDigest[21];
    memset(szDigest, 0x00, sizeof(szDigest));

    qpay_generate_digest_sha1(src, szDigest, &len);
#ifdef OPENSSL_BASE64
    qpay_encode_base64(szDigest, len, (unsigned char*)dest, NULL);
#else
    Base64_Encode((const unsigned char*)szDigest, len, dest, dsize, &len);
#endif

    return 0;
}


int CheckDigest(char* src, ST_PUB_ANS* stAns)
{
    int iRet = 0;
    char* sbstractIndex = NULL;
    char szOldDigest[64], szNewDigest[64], c;
    memset(szOldDigest, 0x00, sizeof(szOldDigest));
    memset(szNewDigest, 0x00, sizeof(szNewDigest));

    // ��ȡժҪ��Ϣ�������ȡʧ������Ϊ����ʧ�ܣ��򷵻ؽ���ʧ�ܸ�SP����ʾSP���»�ȡ��Կ
    //if ((iRet = CUrlAnalyze::getParam(src, CProtocol::ABSTRACT, szOldDigest, sizeof(szOldDigest), &sbstractIndex)) != 0) {
    //    stAns->iResult = CProtocol::DecryptError;
    if ((iRet = CUrlAnalyze::getParam(src, ABSTRACT, szOldDigest, sizeof(szOldDigest), &sbstractIndex)) != 0) {
        stAns->iResult = 1000;
        snprintf(stAns->szErrInfo, sizeof(stAns->szErrInfo), "��ȡժҪ��Ϣ����");
        return stAns->iResult;
    }

    // ��ȡժҪ�ɹ�����У��ժҪ�Ƿ�ƥ�䣬��ƥ��Ҳ��Ϊ����ʧ�ܣ���ʾSP���»�ȡ��Կ
    sbstractIndex--;
    c = *sbstractIndex;
    *sbstractIndex = 0;
    GenerateDigest(src, szNewDigest, sizeof(szNewDigest));
    *sbstractIndex = c;
	//trpc_debug_log("szOldDigest:%s\n",szOldDigest);
    if (strcmp(szOldDigest, szNewDigest) != 0) {
        //stAns->iResult = CProtocol::DecryptError;
        snprintf(stAns->szErrInfo, sizeof(stAns->szErrInfo), "��֤ժҪ��Ϣ����[%s])", szNewDigest);
        return stAns->iResult;
    }

    return 0;
}

/**
 * Description:         ���ܺ���
 * Input:                 szSpID                ��������
 *                           szFrom               �����ܴ�        
 * Output:               szTo                   ���ܺ�
 *                           stAns                  ���ܴ�����Ϣ
 */
void encode(const char* szSpID, const char* szFrom, char* szTo, ST_PUB_ANS& stAns)
{
    // ��ʼ��
    unsigned char  szTmp[MAX_TRPC_LENGTH + 1] = {0};
    unsigned char  szEncode[MAX_TRPC_LENGTH + 1] = {0};
    memset(&stAns, 0, sizeof(ST_PUB_ANS));

     // ����ժҪ
    char szAbstract[1024] = {0};
    GenerateDigest((char*)szFrom, szAbstract, sizeof(szAbstract));

    // �ϲ�ժҪ����ʱ����
    strncpy((char*)szTmp, szFrom, sizeof(szTmp) - 1);
    CUrlAnalyze::setParam((char*)szTmp, ABSTRACT, szAbstract);

    // MD5����
    int iLenIn = strlen((char*)szTmp);
    int iLenOut = 0; 

    qpay_en_3des_v1(szSpID, szTmp, iLenIn, szEncode, &iLenOut, &stAns);
    if (stAns.iResult != 0)
    {
        return;
    }

    // Base64����    
    iLenIn = iLenOut;
    Base64_Encode(szEncode, iLenIn, szTo, MAX_TRPC_LENGTH, &iLenOut);
    szTo[iLenOut] = '\0';
}

/**
 * Description:         ���뺯��
 * Input:                 szSpID                ��������
 *                           szFrom               �����ܴ�        
 * Output:               szTo                   ���ܺ�
 *                           stAns                  ���ܴ�����Ϣ
 */
void decode(const char* szSpID, const char* szFrom, char* szTo, ST_PUB_ANS& stAns)
{
    unsigned char  szTmp[MAX_TRPC_LENGTH + 1] = {0};
    
    int iLenIn = strlen(szFrom);
    int iLenOut = 0;

    // Base64����
    Base64_Decode(szFrom, iLenIn, szTmp, sizeof(szTmp), &iLenOut);

    // MD5����
    iLenIn = iLenOut;
    memset(&stAns, 0, sizeof(stAns));
    qpay_de_3des_v1(szSpID, szTmp, iLenIn, (unsigned char*)szTo, &iLenOut, &stAns);
    if (stAns.iResult != 0)
    {
        return;
    }

    // ��֤����ǩ��
    CheckDigest(szTo, &stAns);
}


