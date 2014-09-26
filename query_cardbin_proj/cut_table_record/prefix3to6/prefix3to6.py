#!/usr/bin/env python
#-*- coding: utf-8 -*-
'''
filename:       prefix3to6.py
description:    修改sql文件里的insert语句，智能分拆记录，由3位前导符修改成5位前导符的
'''
import os, sys


def sql_line_prefix3to6(line):      # with '\n'
    '''
    line : "('1328814','A','0424100000000000000','042','CIR','0424309999999999999',...),"
    返回 : "('1328814','A','0424100000000000000','04241','CIR','0424109999999999999',...),"
           "('1328814','A','0424200000000000000','04242','CIR','0424209999999999999',...),"
           "('1328814','A','0424300000000000000','04243','CIR','0424309999999999999',...),"

    line : "('1328814','A','0424100000000000000','042','CIR','0424309999999999999',...);"
    返回 : "('1328814','A','0424100000000000000','04241','CIR','0424109999999999999',...),"
           "('1328814','A','0424200000000000000','04242','CIR','0424209999999999999',...),"
           "('1328814','A','0424300000000000000','04243','CIR','0424309999999999999',...);"
    '''
    rows = []
    # line end with '\n'
    line_end = line[-2:] # 行尾可以是,或;
    tuple_row = eval(line[:-2])
    card_number_low = long(tuple_row[2])
    card_number_high = long(tuple_row[5])
    low_prefix = int(tuple_row[2][:6])
    low_suffix = tuple_row[2][6:]
    high_prefix = int(tuple_row[5][:6])
    high_suffix = tuple_row[5][6:]

    for i in range(low_prefix, high_prefix+1):
        new_list_row = list(tuple_row)
        prefix = '%06d' % i     # 注意，前导0要保留！
        new_list_row[2] = prefix + ('0000000000000' if i!=low_prefix else low_suffix)
        new_list_row[5] = prefix + ('9999999999999' if i!=high_prefix else high_suffix)
        new_list_row[3] = prefix
        rows.append(tuple(new_list_row))

    return ',\n'.join(str(i) for i in rows) + line_end

def sql_file_prefix3to6(filename, tofilename='prefix3to6.sql'):
    infile = open(filename, 'r')
    outfile = open(tofilename, 'w')
    for line in infile:
        # print repr(line);
        if len(line)>4 and line[0]=='(' and line[-3]==')' and line[-2] in (',',';'):
            outfile.write(sql_line_prefix3to6(line))
        else:
            outfile.write(line)
    infile.close()
    outfile.close()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "<py> <sql_src> [sql_to]"
        exit(2)
    if len(sys.argv) == 2:
        sql_file_prefix3to6(sys.argv[1])
    else:
        sql_file_prefix3to6(sys.argv[1], sys.argv[2])
