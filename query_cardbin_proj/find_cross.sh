#!/usr/bin/env bash
#coding: utf-8

# filename:     find_cross.sh
# description:  数据库区间查重
# note:         Fgcms_product_id, Fissuer_acc_range_low复合唯一，排序由mysql做好，awk只要比较前后两行即可。

HOST=localhost
PORT=3306
USER=public
PASSWORD=public
DATABASE=ia_gcms_2014
TABLE=t_IP0040T1
mysql -h${HOST} -P${PORT} -u${USER} -p${PASSWORD} ${DATABASE} \
    -e "select Fgcms_product_id, Fissuer_acc_range_low, Fissuer_acc_range_high \
        from ${TABLE} \
        order by Fgcms_product_id, Fissuer_acc_range_low" \
    | awk 'NR==1{print}; \
        NR>1{\
            if(id==$1 && high>$2) \
                {print ">>> two";print pre; print;} \
            pre=$0; id=$1; low=$2; high=$3; \
        }'
