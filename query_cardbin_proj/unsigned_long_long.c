/*
 * =====================================================================================
 *
 *       Filename:  unsigned_long_long.c
 *
 *    Description:  19位特殊情况
 *
 *        Version:  1.0
 *        Created:  2014年09月14日 14时17分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
    char s[20] = "9999999999999999999"; 
    unsigned long long ull = 9999999999999999999llu; //0xFFFFFFFFFFFFFFFF;//
    snprintf(s, 20, "%019llu", ull); /* 20 is ok */
    printf("%s\n", s);
    printf("%llu\n", ull);
    sscanf(s, "%lld", &ull);
    printf("%lld\n", ull);
    return 0;
}				/* ----------  end of function main  ---------- */

