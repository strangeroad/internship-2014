/* 创建5个表
 * 插入初始数据
 */
drop table if exists txproj_buyer;
drop table if exists txproj_seller;
drop table if exists txproj_deal;
drop table if exists txproj_pay;
drop table if exists txproj_refund;


create table if not exists txproj_buyer (
    id          int auto_increment not null,
    name        varchar(32) not null,
    password    varchar(32) not null,
    password_pay varchar(32) not null,
    card        varchar(32) not null,
    balance     bigint not null,
    token       varchar(32),
    primary key (id)
)default charset=utf8 auto_increment=1000001;


create table if not exists txproj_seller (
    id          int auto_increment not null,
    name        varchar(32) not null,
    password    varchar(32) not null,
    card        varchar(32) not null,
    balance     bigint not null,
    token       varchar(32),
    primary key (id)
)default charset=utf8 auto_increment=1001;


create table if not exists txproj_deal (
    id          int auto_increment not null,
    buyer_id    int not null,
    seller_id   int not null,
    amount      bigint not null,
    refund      bigint not null,
    create_time datetime not null/* default CURRENT_TIMESTAMP */,
    status      int not null,
    /* 1：待支付
       2：已支付
       3：已退款
     */
    primary key (id)
)default charset=utf8 auto_increment=1000000001;

create table if not exists txproj_pay (
    id          int auto_increment not null,
    deal_id     int,
    buyer_id    int not null,
    seller_id   int not null,
    amount      varchar(32) not null,
    pay_time    datetime not null/* default CURRENT_TIMESTAMP */,
    primary key (id)
)default charset=utf8 auto_increment=1000000001;


create table if not exists txproj_refund (
    id          int auto_increment not null,
    deal_id     int not null,
    buyer_id    int not null,
    seller_id   int not null,
    amount      varchar(32) not null,
    refund_time datetime not null/* default CURRENT_TIMESTAMP */,
    primary key (id)
)default charset=utf8 auto_increment=1000000001;

/* insert */

insert into txproj_buyer
    (name, password, password_pay, card, balance)
    values 
    ("simple", "123456", "654321", "622202 4602079853882", 123456),
    ("jinanyuan", "jinanyuan", "jinanyuan", "622202 1608089953883", 1000),
    ("rich", "rich", "richer", "6217003320116720498", 100000000),
    ("poor", "poor", "poorer", "6217013320216530493", 100);


insert into txproj_seller
    (name, password, card, balance)
    values 
    ("simple", "123456", "622202 4602079853882", 123456),
    ("jinanyuan", "jinanyuan", "622202 1608089953883", 1000),
    ("rich", "rich", "6217003320116720498", 100000000),
    ("poor", "poor", "6217013320216530493", 100);


insert into txproj_deal
    (buyer_id, seller_id, amount, refund, create_time, status)
    values
    (1000001, 1002, 120000, 0, "2014-07-24 09:30", 2),      /* 已付款 */
    (1000003, 1002, 13300, 300, "2014-07-28 09:00", 3),     /* 已退款 */
    (1000004, 1002, 88000, 0, "2014-08-01 10:50", 1),       /* 新订单 */
    (1000001, 1002, 1400, 0, "2014-08-08 18:08", 1),        /* 新订单 */
    (1000003, 1002, 3500, 0, "2014-08-08 19:01", 1),        /* 新订单 */
    (1000004, 1002, 80100, 0, "2014-08-09 14:25", 2),       /* 已付款 */
    (1000001, 1003, 1400, 0, "2014-08-09 14:30", 2),        /* 已付款 */
    (1000002, 1001, 38300, 0, "2014-08-10 19:30", 1),       /* 新订单 */
    (1000001, 1003, 1800, 100, "2014-08-11 09:00", 3),      /* 已退款 */
    (1000003, 1002, 3700, 0, "2014-08-12 09:20", 1);        /* 新订单 */


insert into txproj_pay
    (deal_id, buyer_id, seller_id, amount, pay_time)
    values
    (1000000001, 1000001, 1002, 120000, "2014-07-24 09:35"),
    (1000000002, 1000003, 1002, 13300, "2014-07-28 09:10"),
    (1000000006, 1000004, 1002, 3400, "2014-08-09 19:10"),
    (1000000007, 1000001, 1003, 1400, "2014-08-09 19:40"),
    (1000000009, 1000001, 1003, 1800, "2014-08-11 11:10");


insert into txproj_refund
    (deal_id, buyer_id, seller_id, amount, refund_time)
    values
    (1000000002, 1000003, 1002, 300, "2014-07-29 12:00"),
    (1000000009, 1000001, 1003, 100, "2014-08-11 11:10");
