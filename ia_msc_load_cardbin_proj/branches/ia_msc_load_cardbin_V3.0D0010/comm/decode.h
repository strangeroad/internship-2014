#ifndef _DECODE_H_
#define _DECODE_H_

#include <string>
#include "base64.h"
#include "qpay_encrypt_client.h"
#include "UrlAnalyze.h"

using namespace CFT;


int GenerateDigest(char* src, char* dest, int dsize);
int CheckDigest(char* src, ST_PUB_ANS* stAns);

/**
 * 解码函数
 */
void encode(const char* szSpID, const char* szFrom, char* szTo, ST_PUB_ANS& stAns);

/**
 * 解码函数
 */
void decode(const char* szSpID, const char* szFrom, char* szTo, ST_PUB_ANS& stAns);

#endif

