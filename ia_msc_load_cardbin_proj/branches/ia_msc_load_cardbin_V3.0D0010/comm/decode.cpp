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

    // 获取摘要信息，如果获取失败则认为解密失败，则返回解密失败给SP，提示SP重新获取密钥
    //if ((iRet = CUrlAnalyze::getParam(src, CProtocol::ABSTRACT, szOldDigest, sizeof(szOldDigest), &sbstractIndex)) != 0) {
    //    stAns->iResult = CProtocol::DecryptError;
    if ((iRet = CUrlAnalyze::getParam(src, ABSTRACT, szOldDigest, sizeof(szOldDigest), &sbstractIndex)) != 0) {
        stAns->iResult = 1000;
        snprintf(stAns->szErrInfo, sizeof(stAns->szErrInfo), "获取摘要信息出错");
        return stAns->iResult;
    }

    // 获取摘要成功，则校验摘要是否匹配，不匹配也认为解密失败，提示SP重新获取密钥
    sbstractIndex--;
    c = *sbstractIndex;
    *sbstractIndex = 0;
    GenerateDigest(src, szNewDigest, sizeof(szNewDigest));
    *sbstractIndex = c;
	//trpc_debug_log("szOldDigest:%s\n",szOldDigest);
    if (strcmp(szOldDigest, szNewDigest) != 0) {
        //stAns->iResult = CProtocol::DecryptError;
        snprintf(stAns->szErrInfo, sizeof(stAns->szErrInfo), "验证摘要信息出错[%s])", szNewDigest);
        return stAns->iResult;
    }

    return 0;
}

/**
 * Description:         加密函数
 * Input:                 szSpID                机构代码
 *                           szFrom               待加密串        
 * Output:               szTo                   加密后串
 *                           stAns                  加密错误信息
 */
void encode(const char* szSpID, const char* szFrom, char* szTo, ST_PUB_ANS& stAns)
{
    // 初始化
    unsigned char  szTmp[MAX_TRPC_LENGTH + 1] = {0};
    unsigned char  szEncode[MAX_TRPC_LENGTH + 1] = {0};
    memset(&stAns, 0, sizeof(ST_PUB_ANS));

     // 计算摘要
    char szAbstract[1024] = {0};
    GenerateDigest((char*)szFrom, szAbstract, sizeof(szAbstract));

    // 合并摘要到临时缓存
    strncpy((char*)szTmp, szFrom, sizeof(szTmp) - 1);
    CUrlAnalyze::setParam((char*)szTmp, ABSTRACT, szAbstract);

    // MD5编码
    int iLenIn = strlen((char*)szTmp);
    int iLenOut = 0; 

    qpay_en_3des_v1(szSpID, szTmp, iLenIn, szEncode, &iLenOut, &stAns);
    if (stAns.iResult != 0)
    {
        return;
    }

    // Base64编码    
    iLenIn = iLenOut;
    Base64_Encode(szEncode, iLenIn, szTo, MAX_TRPC_LENGTH, &iLenOut);
    szTo[iLenOut] = '\0';
}

/**
 * Description:         解码函数
 * Input:                 szSpID                机构代码
 *                           szFrom               待解密串        
 * Output:               szTo                   解密后串
 *                           stAns                  解密错误信息
 */
void decode(const char* szSpID, const char* szFrom, char* szTo, ST_PUB_ANS& stAns)
{
    unsigned char  szTmp[MAX_TRPC_LENGTH + 1] = {0};
    
    int iLenIn = strlen(szFrom);
    int iLenOut = 0;

    // Base64解码
    Base64_Decode(szFrom, iLenIn, szTmp, sizeof(szTmp), &iLenOut);

    // MD5解码
    iLenIn = iLenOut;
    memset(&stAns, 0, sizeof(stAns));
    qpay_de_3des_v1(szSpID, szTmp, iLenIn, (unsigned char*)szTo, &iLenOut, &stAns);
    if (stAns.iResult != 0)
    {
        return;
    }

    // 验证数字签名
    CheckDigest(szTo, &stAns);
}


