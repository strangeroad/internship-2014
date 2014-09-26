#define TIXML_USE_STL
#include "globalconfig.h"
#include "error.h"
#include "tinyxml.h"

/**
 * 取子节点
 */
TiXmlNode*  NODE_CHILD(TiXmlNode* pNode, const char* child=NULL)
{
    if(!pNode)
    {
        throw CException(ERR_NULL_XML_NODE, string("Get node failed:") + string(child), __FILE__, __LINE__);
    }

    TiXmlNode* pChild = child ?  pNode->FirstChild(child) : pNode->FirstChild();
    if (!pChild)
    {
        throw CException(ERR_NULL_XML_NODE, string("Get child node failed:") + string(child), __FILE__, __LINE__);
    }

    return pChild;
}

/**
 * 取相邻节点
 */
TiXmlNode* NODE_NEXT(TiXmlNode* pNode)
{
    return pNode->NextSibling();
}

/**
 * 取结点名称
 */
string NODE_NAME(TiXmlNode* pNode)
{
    return pNode->Value() ? pNode->Value() : "";
}

/**
 * 取结点值
 */
string NODE_STR_VALUE(TiXmlNode* pNode)
{
    return pNode->ToElement()->GetText() ? pNode->ToElement()->GetText() : "";
}

/**
 * 取结点值
 */
int NODE_INT_VALUE(TiXmlNode* pNode)
{
    return pNode->ToElement()->GetText() ? atoi(pNode->ToElement()->GetText()) : 0;
}

/**
 * 取结点值
 */
LONG NODE_LONG_VALUE(TiXmlNode* pNode)
{
    return pNode->ToElement()->GetText() ? atoll(pNode->ToElement()->GetText()) : 0;
}

/**
 * 取结点属性
 */
string NODE_ATTR(TiXmlNode* pNode, const char* attribute)
{
    return pNode->ToElement()->Attribute(attribute) ? pNode->ToElement()->Attribute(attribute) : "";
}

/**
 * 读取配置文件
 * @input       strCfgFile      配置文件
 */
GlobalConfig::GlobalConfig(const string& strCfgFile) throw(CException)
{
    // 解析配置文件
    TiXmlDocument  Doc(strCfgFile.c_str());
    if(!Doc.LoadFile())
    {
        throw CException(ERR_LOAD_XML_CFG, "Load xml config file error:" + strCfgFile);
    }

     /**
       * 获取DB配置信息
       */
    TiXmlNode* pDbNode = Doc.FirstChild("root")->FirstChild("database");
    m_DbCfg.host = NODE_STR_VALUE(NODE_CHILD(pDbNode, "host"));
    m_DbCfg.user = NODE_STR_VALUE(NODE_CHILD(pDbNode, "user"));
    m_DbCfg.pswd = NODE_STR_VALUE(NODE_CHILD(pDbNode, "pswd"));
    m_DbCfg.port = NODE_INT_VALUE(NODE_CHILD(pDbNode, "port"));
    m_DbCfg.overtime = NODE_INT_VALUE(NODE_CHILD(pDbNode, "overtime"));
    m_DbCfg.charset = NODE_STR_VALUE(NODE_CHILD(pDbNode, "charset"));

    /**
     * 获取Middle连接配置信息
     */
    /*TiXmlNode* pMidNode = Doc.FirstChild("root")->FirstChild("middle")->FirstChild("connection");
    for(; pMidNode; pMidNode = NODE_NEXT(pMidNode))
    {
        string strServer = NODE_ATTR(pMidNode, "name");

        HostCfg Item;

        Item.host = NODE_STR_VALUE(NODE_CHILD(pMidNode, "host"));
        Item.user = NODE_STR_VALUE(NODE_CHILD(pMidNode, "user"));
        Item.pswd = NODE_STR_VALUE(NODE_CHILD(pMidNode, "pswd"));
        Item.port = NODE_INT_VALUE(NODE_CHILD(pMidNode, "port"));
        Item.overtime = NODE_INT_VALUE(NODE_CHILD(pMidNode, "overtime"));

        m_mapMidCfg[strServer] = Item;
    }*/
    

    /*
     * 获取业务配置信息
     */
    TiXmlNode* pAppNode = Doc.FirstChild("root")->FirstChild("app");
    

    /*
     * 获取系统配置信息
     */
    TiXmlNode* pSysNode = Doc.FirstChild("root")->FirstChild("system");
    m_SysCfg.mid_no = NODE_INT_VALUE(NODE_CHILD(pSysNode, "mid_no"));
    m_SysCfg.log_path = NODE_STR_VALUE(NODE_CHILD(pSysNode, "log_path"));
    m_SysCfg.log_num = NODE_INT_VALUE(NODE_CHILD(pSysNode, "log_num"));
    m_SysCfg.log_size = NODE_INT_VALUE(NODE_CHILD(pSysNode, "log_size"));



    TiXmlNode *pObjKeyNode = Doc.FirstChild ( "root" )->FirstChild ( "obj_key" );
    m_shm_key1 = NODE_INT_VALUE ( NODE_CHILD ( pObjKeyNode, "shm_key1" ) );
    m_shm_key2 = NODE_INT_VALUE ( NODE_CHILD ( pObjKeyNode, "shm_key2" ) );
}
 
/**
 * 打印配置内容
 */
ostream& GlobalConfig::dump(ostream& os) const
{
    // 打印Db连接配置信息
    os<<"Db configuration:"<<std::endl;
    os<<"\t\t"<<m_DbCfg.host<<":"<<m_DbCfg.port<<":"<<m_DbCfg.user<<":"
            <<m_DbCfg.pswd<<":"<<m_DbCfg.overtime
            <<std::endl;
    
    // 打印Middle连接配置信息
    os<<"Mid configuration:"<<std::endl;
    for(map<string, HostCfg>::const_iterator it=m_mapMidCfg.begin(); it != m_mapMidCfg.end(); ++it)
    {
        os<<"\t\t"<<it->first<<":"<<it->second.host<<":"<<it->second.port<<":"
            <<it->second.user<<":"<<it->second.pswd<<":"<<it->second.overtime
            <<std::endl;
    }

    // 打印系统配置信息
    os<<"System configuration:"<<std::endl
        <<"\t\tmid_no="<<m_SysCfg.mid_no
        <<"\t\tlog_path="<<m_SysCfg.log_path
        <<"\t\tlog_num="<<m_SysCfg.log_num
        <<"\t\tlog_size="<<m_SysCfg.log_size
        <<std::endl;
  
    return os;
}

/**
 * 打印配置内容
 */
ostream& operator<<(ostream& os, const GlobalConfig& Cfg)
{
    return Cfg.dump(os);
}

