#ifndef _ERROR_H_
#define _ERROR_H_


#define SYS_ERROR(var)  10211##var  //需申请
#define APP_ERROR(var)  10212##var

/*
  *系统错误
  */
const int ERR_SYSTEM_UNKNOW_ERROR           = SYS_ERROR(0001); // 系统未知错误
const int ERR_NULL_XML_NODE                 = SYS_ERROR(0002); // 指定的XML节点不存在
const int ERR_LOAD_XML_CFG                  = SYS_ERROR(0003); // 读取XML配置文件失败


/**
 * 数据库操作错误
 */
const int ERR_DB_INITIAL                    = SYS_ERROR(0101);     // 分配数据库连接失败
const int ERR_DB_CONNECT                    = SYS_ERROR(0102);     // 连接数据库失败
const int ERR_DB_LOST                       = SYS_ERROR(0103);     // 和数据库断开连接
const int ERR_DB_NULL_RESULT                = SYS_ERROR(0104);     // 获取到空数据库结果集
const int ERR_DB_AFFECT_ROW                 = SYS_ERROR(0105);     // 获取到空数据库结果集
const int ERR_DB_AUTO_CMMIT                 = SYS_ERROR(0106);     // 设置自动提交模式失败
const int ERR_DB_BEGIN                      = SYS_ERROR(0107);     // 开始事务失败
const int ERR_DB_COMMIT                     = SYS_ERROR(0108);     // 提交事务失败
const int ERR_DB_ROLLBACK                   = SYS_ERROR(0109);     // 回滚事务失败
const int ERR_DB_NONE_ROW                   = SYS_ERROR(0110);     // 没有取到任何结果
const int ERR_DB_MULTI_ROW                  = SYS_ERROR(0111);     // 取到结果多余一行
const int ERR_DB_FETCH_ROW                  = SYS_ERROR(0112);     // 取结果行错误
const int ERR_DB_UNKNOW_FAULT               = SYS_ERROR(0113);     // 数据库错误
const int ERR_DB_ROW_LENGTH                 = SYS_ERROR(0114);     // 结果行数过长
const int ERR_DB_SQL_NOVALUE                = SYS_ERROR(0115);     // 拼接SQL语句错误,没有值
const int ERR_DB_DUP_TRANS                  = SYS_ERROR(0116);     // 重复开始事务
const int ERR_DB_NO_TRANS                   = SYS_ERROR(0117);     // 不在事务中,调用事务处理语句
const int ERR_DB_UNKNOW                     = SYS_ERROR(0118); // 意外错误
const int ERR_DB_ZEROCOLUMN                 = SYS_ERROR(0119); //查询结果列为0
const int ERR_DB_NOTUNIQ                    = SYS_ERROR(0120); //查询结果行不为1
const int ERR_DB_DEL_NOT_UNIQ               = SYS_ERROR(0121); //删除记录不惟一
const int ERR_DB_AFFECTED                   = SYS_ERROR(0122); //影响行不惟一
const int ERR_UNKNOWN_DB_TYPE               = SYS_ERROR(0123); 


/**
 * MIDDLE连接业务错误
 */
const int ERR_FAIL_CONNECT_MIDDLE           = SYS_ERROR(0201);
const int ERR_FAIL_CALL_MIDDLE              = SYS_ERROR(0202);


/**
 * 业务级别错误
 */
 
const int ERR_BAD_PARAM                     = APP_ERROR(0100); // 参数错误
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


const int ERR_NO_MEMERY_READY               = APP_ERROR(0200);  // 内存没准备好，请运行批跑
const int ERR_NOT_FOUND                     = APP_ERROR(0201);  // 没有找到记录
const int ERR_PARAMS_CARDNUM                = APP_ERROR(0202);  // 参数格式出错


#endif

