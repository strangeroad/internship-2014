/*
 * =====================================================================================
 *
 *       Filename:  int-oct-scanf.c
 *
 *    Description:  sscanf('011', "%d", &num);   num=11 or 9
 *
 *        Version:  1.0
 *        Created:  2014年09月24日 10时36分46秒
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
    int num=0;
    sscanf("011", "%d", &num);
    printf("%d\n",num);
    return 0;
}				/* ----------  end of function main  ---------- */
