#ifndef _ERROR_H_
#define _ERROR_H_


#define SYS_ERROR(var)  10211##var  //������
#define APP_ERROR(var)  10212##var

/*
  *ϵͳ����
  */
const int ERR_SYSTEM_UNKNOW_ERROR           = SYS_ERROR(0001); // ϵͳδ֪����
const int ERR_NULL_XML_NODE                 = SYS_ERROR(0002); // ָ����XML�ڵ㲻����
const int ERR_LOAD_XML_CFG                  = SYS_ERROR(0003); // ��ȡXML�����ļ�ʧ��


/**
 * ���ݿ��������
 */
const int ERR_DB_INITIAL                    = SYS_ERROR(0101);     // �������ݿ�����ʧ��
const int ERR_DB_CONNECT                    = SYS_ERROR(0102);     // �������ݿ�ʧ��
const int ERR_DB_LOST                       = SYS_ERROR(0103);     // �����ݿ�Ͽ�����
const int ERR_DB_NULL_RESULT                = SYS_ERROR(0104);     // ��ȡ�������ݿ�����
const int ERR_DB_AFFECT_ROW                 = SYS_ERROR(0105);     // ��ȡ�������ݿ�����
const int ERR_DB_AUTO_CMMIT                 = SYS_ERROR(0106);     // �����Զ��ύģʽʧ��
const int ERR_DB_BEGIN                      = SYS_ERROR(0107);     // ��ʼ����ʧ��
const int ERR_DB_COMMIT                     = SYS_ERROR(0108);     // �ύ����ʧ��
const int ERR_DB_ROLLBACK                   = SYS_ERROR(0109);     // �ع�����ʧ��
const int ERR_DB_NONE_ROW                   = SYS_ERROR(0110);     // û��ȡ���κν��
const int ERR_DB_MULTI_ROW                  = SYS_ERROR(0111);     // ȡ���������һ��
const int ERR_DB_FETCH_ROW                  = SYS_ERROR(0112);     // ȡ����д���
const int ERR_DB_UNKNOW_FAULT               = SYS_ERROR(0113);     // ���ݿ����
const int ERR_DB_ROW_LENGTH                 = SYS_ERROR(0114);     // �����������
const int ERR_DB_SQL_NOVALUE                = SYS_ERROR(0115);     // ƴ��SQL������,û��ֵ
const int ERR_DB_DUP_TRANS                  = SYS_ERROR(0116);     // �ظ���ʼ����
const int ERR_DB_NO_TRANS                   = SYS_ERROR(0117);     // ����������,�������������
const int ERR_DB_UNKNOW                     = SYS_ERROR(0118); // �������
const int ERR_DB_ZEROCOLUMN                 = SYS_ERROR(0119); //��ѯ�����Ϊ0
const int ERR_DB_NOTUNIQ                    = SYS_ERROR(0120); //��ѯ����в�Ϊ1
const int ERR_DB_DEL_NOT_UNIQ               = SYS_ERROR(0121); //ɾ����¼��Ωһ
const int ERR_DB_AFFECTED                   = SYS_ERROR(0122); //Ӱ���в�Ωһ
const int ERR_UNKNOWN_DB_TYPE               = SYS_ERROR(0123); 


/**
 * MIDDLE����ҵ�����
 */
const int ERR_FAIL_CONNECT_MIDDLE           = SYS_ERROR(0201);
const int ERR_FAIL_CALL_MIDDLE              = SYS_ERROR(0202);


/**
 * ҵ�񼶱����
 */
 
const int ERR_BAD_PARAM                     = APP_ERROR(0100); // ��������
const int ERR_AUTH_NOT_EXISTS               = APP_ERROR(0101); //
const int ERR_SPID_NOT_EXISTS              = APP_ERROR(0102); //
const int ERR_RATE_TIME_EXPIRED             = APP_ERROR(0103); //
const int ERR_RATE_CHECK_CODE_NOT_MATCH     = APP_ERROR(0104); //
const int ERR_BANK_RATE_SAVE                = APP_ERROR(0105); // 
const int ERR_RATE_TIME_WARN                = APP_ERROR(0106); // 
const int ERR_CURRENCY_SELL_OVER_FLT_LIMIT  = APP_ERROR(0107); //
const int ERR_CURRENCY_RATE_INVALID         = APP_ERROR(0108); 
const int ERR_CURRENCY_CODE_STATE           = APP_ERROR(0109); 
const int ERR_RESULT_VALUE_ZERO             = APP_ERROR(0110); 
const int ERR_RATE_FIXED_CODE_NOT_MATCH     = APP_ERROR(0111); 


const int ERR_NO_MEMERY_READY               = APP_ERROR(0200);  // �ڴ�û׼���ã�����������
const int ERR_NOT_FOUND                     = APP_ERROR(0201);  // û���ҵ���¼
const int ERR_PARAMS_CARDNUM                = APP_ERROR(0202);  // ������ʽ����


#endif

