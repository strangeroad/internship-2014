#ifndef _SER_RUN_INFO_H_
#define _SER_RUN_INFO_H_

#include <sys/time.h>
#include <vector>
#include "exception.h"
#include "error.h"
#include "trpc_service.h"
#include "cftlog.h"

/**
 * ��ϢID����󳤶�
 */
const int MIN_MSG_NO_LEN=10;
const int MAX_MSG_NO_LEN = 32;

/**
 * ��Ϣ����
 */
enum EMSG_TYPE
{
    TEXT_MSG = 1, // �ı���Ϣ
    BINARY_MSG = 2,  // ��������Ϣ
    COMMON_QUERY_FAULT_MSG =3 //common query ��ʽ
};

/**
 * �궨��
 */
#define SRC_IP SerRunInfo::getSrcIp()
#define SERVICE SerRunInfo::getService()
#define HOST_IP SerRunInfo::getHostIp()
#define PROC_NO SerRunInfo::getProcNo()
#define MSG_NO SerRunInfo::getMsgNo()

/**
 * ҵ��������Ϣ����
 */
class SerRunInfo
{  

/**
 * ��Ϣ��ԴIP
 */
static char src_ip[64+1];

/**
 * ��������
 */
static char service_name[128+1];

/**
 * ȫ����Ϣ��
 */
static char msg_no[MAX_MSG_NO_LEN];

public:

    /**
     * ��ȡ��Ϣ��Դ��IP��ַ
     */
    static inline const char* getSrcIp() { return src_ip; }

    /**
     * ��ȡ��ǰ���ڷ�������
     */
    static inline const char* getService() { return service_name; }

    /**
     * ��ȡ��������IP��ַ
     */
    static const char* getHostIp();

    /**
     * ��ȡȫ����Ϣ��
     */
    static int getProcNo();
    
    /**
     * ��ȡȫ����Ϣ��
     */
    static inline const char* getMsgNo() { return msg_no; }

public:
    
    /**
     * ���캯��
     */
    SerRunInfo(TRPC_SVCINFO* pRequst, EMSG_TYPE msgtype=TEXT_MSG);

    /**
     * ��������
     */
    ~SerRunInfo();
    
    /**
     * ����������ʱ��
     */
    inline int getTimeCost() { return (end.tv_sec  - start.tv_sec) * 1000 + (end.tv_usec- start.tv_usec)/1000; }
   
protected:
    TRPC_SVCINFO* rqst;
    EMSG_TYPE type; // ��Ϣ����
    timeval start; // ���������ʼʱ��
    timeval end; // ������ý���ʱ��   
};

#endif

