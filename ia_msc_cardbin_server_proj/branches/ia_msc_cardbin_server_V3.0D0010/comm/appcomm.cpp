#include "appcomm.h"
#include "decode.h"
#include "globalconfig.h"
#include "bill_no.h"
#include <math.h>

/**
 * ȫ�ֱ�������
 */
extern CftLog* gPtrAppLog;
extern GlobalConfig* gPtrConfig; 




/**
 * ���ַ�������SQLת��
 * @input:  str  ��ת���ַ���
 * @return: ת�ƺ��ַ���
 */
string escapeString(const string& str)
{
    static char szTmp[4096 + 1];
    memset(szTmp, 0, sizeof(szTmp));
    mysql_escape_string(szTmp, str.c_str(), str.length());

    return szTmp;
}

