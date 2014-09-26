/**
  * FileName: globalconfig.h
  * Author: Hawkliu
  * Version :1.0
  * Date: 2007-07-31
  * Description: 本文件用于保存项目中共用的全局配置项
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
 * 定义大整数类型
 */
typedef int64_t LONG;


/**
 * 存放全局配置的类
 */
struct GlobalConfig
{   

    /**
     * 远程机器配置信息
     */
    struct HostCfg
    {
        string  host; // IP地址
        string  user; //用户名
        string  pswd; // 密码
        int     port; // 端口
        int     overtime; // 超时时长
        string  charset; // 字符集
    };        

    /**
     * 业务配置信息
     */
    struct AppCfg
    {
        int default_bank_type;     //缺省汇率发布银行类型
    };



    /**
        * 系统配置信息
        */
    struct SysCfg
    {
        int mid_no; // middle机器编号
        string log_path; // 日志文件路径
        int log_num; // 日志文件数量
        int log_size; // 日志文件大小
        string encrypt_sp_id; // 用于加密的spid 配置
    };



public:

    /**
     * 读取配置文件
     */
    GlobalConfig(const string& strCfgFile) throw(CException);      // 读取配置文件

    /**
     * 打印配置内容
     */
    ostream& dump(ostream& os) const;

public:
    
    /**
        * 数据库配置
        */
    HostCfg  m_DbCfg;

    /**
        * Middle连接配置
        */
    map<string, HostCfg> m_mapMidCfg;

    /**
        * 业务配置信息
        */
    AppCfg m_AppCfg;


    /**
        * 系统配置信息
        */
    SysCfg m_SysCfg;


    int m_shm_key1;
    int m_shm_key2;
};

/**
 * 打印配置内容
 */
ostream& operator<<(ostream& os, const GlobalConfig& Cfg);

#endif

