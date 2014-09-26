#!/usr/bin/env python
#-*- coding: utf-8 -*-

# 使用：
# mysqldump -upublic -ppublic ia_gcms_2014 t_IP0040T1 > ia_gcms_2014_t_IP0040T1.sql
# sed '/INSERT INTO.*VALUES.*;$/{s/(/\n(/g}' ia_gcms_2014_t_IP0040T1.sql ia_gcms_2014_t_IP0040T1_sed.sql
# python find_cross.py ia_gcms_2014_t_IP0040T1_sed.sql

'''
filename:       find_cross.py
description:    找出区间交叉的记录
'''
import os, sys
from operator import itemgetter

def find_across(filename):
    '''
    寻交叉区间
    '''
    sql_file = open(filename, 'r')
    # 列表解析，一句抵10句      # with '\n'
    tuple_rows = [ eval(line[:-2]) for line in sql_file.readlines() 
            if line[0]=='(' and line[-3]==')' and line[-2] in (',',';') ]   # line[-1]=='\n' 

    tuple_rows.sort(key=lambda x:x[4])      # key=itemgetter(4)
    # 不用变成int了，等长就直接比字符串
    cmper = lambda a,b:\
                -1 if a[4]<b[4] else \
                1 if a[4]>b[4] else \
                -1 if a[5]<b[2] else \
                1 if a[2]>b[5] else \
                0
    tuple_rows.sort(cmper)

    # print tuple_rows

    begin_end = 0
    for i in range(len(tuple_rows)-1):
        if cmper(tuple_rows[i], tuple_rows[i+1])==0:
            if begin_end == 0:
                begin_end = 1
                print "============== begin =============="
            # if tuple_rows[i][4]==tuple_rows[i][4]:   # 多余
            print tuple_rows[i]
        else:
            if begin_end == 1:
                print tuple_rows[i]
                # print "==============  end  =============="
                begin_end = 0
    if begin_end == 1:
        print tuple_rows[-1]
        # print "==============  end  =============="

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "<py> <filename>"
        exit(0)
    find_across(sys.argv[1])
