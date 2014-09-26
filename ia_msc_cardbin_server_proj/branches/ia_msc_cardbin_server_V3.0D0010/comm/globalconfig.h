/**
  * FileName: globalconfig.h
  * Author: Hawkliu
  * Version :1.0
  * Date: 2007-07-31
  * Description: ���ļ����ڱ�����Ŀ�й��õ�ȫ��������
  */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "exception.h"
//#include "common.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>

using std::map;
using std::set;
using std::string;
using std::vector;
using std::ostream;

/**
 * �������������
 */
typedef int64_t LONG;


/**
 * ���ȫ�����õ���
 */
struct GlobalConfig
{   

    /**
     * Զ�̻���������Ϣ
     */
    struct HostCfg
    {
        string  host; // IP��ַ
        string  user; //�û���
        string  pswd; // ����
        int     port; // �˿�
        int     overtime; // ��ʱʱ��
        string  charset; // �ַ���
    };        

    /**
     * ҵ��������Ϣ
     */
    struct AppCfg
    {
        int default_bank_type;     //ȱʡ���ʷ�����������
    };



    /**
        * ϵͳ������Ϣ
        */
    struct SysCfg
    {
        int mid_no; // middle�������
        string log_path; // ��־�ļ�·��
        int log_num; // ��־�ļ�����
        int log_size; // ��־�ļ���С
        string encrypt_sp_id; // ���ڼ��ܵ�spid ����
    };



public:

    /**
     * ��ȡ�����ļ�
     */
    GlobalConfig(const string& strCfgFile) throw(CException);      // ��ȡ�����ļ�

    /**
     * ��ӡ��������
     */
    ostream& dump(ostream& os) const;

public:
    
    /**
        * ���ݿ�����
        */
    HostCfg  m_DbCfg;

    /**
        * Middle��������
        */
    map<string, HostCfg> m_mapMidCfg;

    /**
        * ҵ��������Ϣ
        */
    AppCfg m_AppCfg;


    /**
        * ϵͳ������Ϣ
        */
    SysCfg m_SysCfg;


    int m_shm_key1;
    int m_shm_key2;
};

/**
 * ��ӡ��������
 */
ostream& operator<<(ostream& os, const GlobalConfig& Cfg);

#endif

