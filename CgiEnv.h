/*
 * =====================================================================================
 *
 *       Filename:  CgiEnv.h
 *
 *    Description:  CgiEnvironment.h太难用cookie了。
 *
 *        Version:  1.0
 *        Created:  2014年08月15日 20时23分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  pythonwood (wws), 582223837@qq.com
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef _CGIENV_H
#define _CGIENV_H_ 1

#include <algorithm>
#include "cgicc/CgiEnvironment.h"
  
//! A vector of HTTPCookie objects
typedef std::vector<cgicc::HTTPCookie>::iterator 	cookie_iterator;
//! A vector of const HTTPCookie objects
typedef std::vector<cgicc::HTTPCookie>::const_iterator const_cookie_iterator;

class CK_nameCompare : public std::unary_function<cgicc::HTTPCookie, bool>
{
public:
  
    inline explicit CK_nameCompare(const std::string& name)
      : fName(name) {}
    
    inline bool operator() (const argument_type& c) 	const
    { return c.getName()==fName;}
  
private:
    std::string fName;
};

class CgiEnv: public cgicc::CgiEnvironment
{
    inline const_cookie_iterator getCookie(std::string& name) 				const
    {
        return std::find_if(getCookieList().begin(), getCookieList().end(), CK_nameCompare(name));
    }
};

#endif /* #define _CGIENV_H_ 1 */
