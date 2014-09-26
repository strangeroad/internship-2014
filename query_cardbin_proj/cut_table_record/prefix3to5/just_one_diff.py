#!/usr/bin/env python
#-*- coding: utf-8 -*-
'''
filename:       find_cross.py
description:    找出区间交叉的记录
'''
import os, sys

def just_one_diff(filename):
    '''
    区间相同的仅id不同吗？
    '''
    
    sql_file = open(filename, 'r')
    # 列表解析，一句抵10句      # with '\n'
    tuple_rows = [ eval(line[:-2]) for line in sql_file.readlines() 
            if line[0]=='(' and line[-3]==')' and line[-2] in (',',';') ]   # line[-1]=='\n' 

    tuple_rows.sort(key=lambda x:x[4])      # key=itemgetter(4)
    # 不用变成int了，等长就直接比字符串
    cmper = lambda a,b:\
                -1 if a[5]<b[2] else \
                1 if a[2]>b[5] else \
                -1 if a[4]<b[4] else \
                1 if a[4]>b[4] else \
                0
    tuple_rows.sort(cmper)

    cmpsame = lambda a,b: a[2]==b[2] and a[5]==b[5]
    begin_end = 0
    for i in range(len(tuple_rows)-1):
        if cmpsame(tuple_rows[i], tuple_rows[i+1]):
            tset = set(tuple_rows[i])
            tset.update(tuple_rows[i+1])
            if len(tset) != len(tuple_rows[i])+1:
                if begin_end == 0:
                    print '============ * ============='
                    begin_end = 1
                print tuple_rows[i]
        else:
            if begin_end == 1:
                print tuple_rows[i]
                begin_end = 0
    if begin_end == 1:
        print tuple_rows[-1]

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "<py> <filename>"
        exit(0)
    just_one_diff(sys.argv[1])
