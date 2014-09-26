/*
 * =====================================================================================
 *
 *       Filename:  strn.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年09月05日 15时44分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
    char buf[3] = {'a', 'b', 'c'};
    strncpy(buf, "12", 2);
    printf("%c|%c|%c\n", buf[0], buf[1], buf[2]);
    snprintf(buf, 2, "%01d", 3);
    printf("%c|%c|%c\n", buf[0], buf[1], buf[2]);
    return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
