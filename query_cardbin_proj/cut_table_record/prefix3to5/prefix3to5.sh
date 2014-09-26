#!/usr/bin/env bash
#coding: utf8

# filename:     prefix3to5.sh
# description:  改变一下参数，使得数据库表自动由前3位索引，转化到5位索引。
# note:         涉及备份mysql，sed改关键字，分拆sql记录，恢复mysql数据库。

HOST=localhost
PORT=3306
USER=public
PASSWORD=public
DATABASE=ia_gcms_2014
# 来源表，表名要足够唯一，保证表名字不会被其他词命中，不然会错。
TABLE_SRC=t_IP0040T1
# 目标表，若已存在会覆盖
TABLE_TO=${TABLE_SRC}_prefix3to5

# 临时sql文件
SQLFILE=${DATABASE}_${TABLE_SRC}.sql
SQLFILE_SED=${DATABASE}_${TABLE_SRC}_sed.sql
SQLFILE_MODIFY=${DATABASE}_${TABLE_SRC}_to.sql

# 删除临时文件吗？
DEL_TEMP_FILE=True

mysqldump -h${HOST} -P${PORT} -u${USER} -p${PASSWORD} ${DATABASE} ${TABLE_SRC} > ${SQLFILE}

# 改表名
# sed "s/DROP TABLE IF EXISTS \`${TABLE_SRC}\`/DROP TABLE IF EXISTS \`${TABLE_TO}\`/;\
#     s/CREATE TABLE \`${TABLE_SRC}\`/CREATE TABLE \`${TABLE_TO}\`/;\
#     s/INSERT INTO \`${TABLE_SRC}\` VALUES/INSERT INTO \`${TABLE_to}\` VALUES/;" ${SQLFILE} > ${SQLFILE_SED}
sed "s/ \`${TABLE_SRC}\`/ \`${TABLE_TO}\`/;" ${SQLFILE} > ${SQLFILE_SED}

# 改bug
sed -i "s/\`Fissuer_acc_range_prefix\` varchar(4)/\`Fissuer_acc_range_prefix\` varchar(8)/" ${SQLFILE_SED}

# INSERT INTO `t_IP0040T1` VALUES (...), (...);
# to:
# INSERT INTO `t_IP0040T1` VALUES 
# (...), 
# (...);
sed -i '/INSERT INTO.*VALUES.*;$/{s/(/\n(/g}' ${SQLFILE_SED}

# python find_cross.py ${SQLFILE_SED} > find_cross.out
# python just_one_diff.py ${SQLFILE_SED} > find_no_one.out

python ./prefix3to5.py ${SQLFILE_SED} ${SQLFILE_MODIFY}

mysql -h${HOST} -P${PORT} -u${USER} -p${PASSWORD} ${DATABASE} < ${SQLFILE_MODIFY}

# [ $${DEL_TEMP_FILE} ] && /bin/rm ${SQLFILE} ${SQLFILE_MODIFY}
