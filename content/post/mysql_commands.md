+++
title = "MySQL 常用命令"
date = 2019-04-05T21:04:21+08:00
lastmod = 2019-04-07T00:29:18+08:00
tags = ["sql", "mysql", "alter", "charset", "character", "join"]
categories = ["BLOG"]
draft = false
+++

大学学的数据库系统概论工作后几年没有使用都已忘了, 现在项目需要用到数据库, 但来不及细看相关书籍了, 遂将一些常用的记录下来.


## 常用类型 {#常用类型}

`MySQL` 支持多种类型, 大致可以分为三类: 数值, 日期/时间和字符串(字符)类型, 大致如下:

-   数值类型
    -   整数: `tinyint`, `smallint`, `mediumint`, `int`, `bigint`
    -   浮点数: `float`, `double`, `decimal`

-   日期和时间

    `date`, `time`, `datetime`, `timestamp`, `year`

-   字符串类型
    -   字符串: `char`, `varchar`
    -   文本: `tinytext`, `text`, `mediumtext`, `longtext`
    -   二进制字符串: `tinyblob`, `blob`, `mediumblob`, `longblob`

下面将详细给出每种类型的大小和描述.

<!--more-->


### 数值类型 {#数值类型}

| 类型      | 字节         | 描述   |
|---------|------------|------|
| tinyint   | 1            | 小整数值 |
| smallint  | 2            | 大整数值 |
| mediumint | 3            | 大整数值 |
| int       | 4            | 大整数值 |
| bigint    | 8            | 极大整数值 |
| float     | 4            | 单精度浮点数 |
| double    | 8            | 双精度浮点数 |
| decimal   | decimal(M,D) | 定点数 |

**DECIMAL(M,D) 高精度的数据类型, 常用来存储交易相关的数据**

**`M` 代表总精度, `N` 代表小数点右侧的位数. 1 < M < 254, 0 < N < 60, 存储空间变长**


### 日期和时间 {#日期和时间}

| 类型      | 字节 | 描述                              |
|---------|----|---------------------------------|
| date      | 3  | 精确到年月日, 如: 2016-09-01      |
| time      | 3  | 精确到时分秒, 如: 09:10:11        |
| datetime  | 8  | 精确到年月日时分秒, 如: 2016-09-01 09:10:11 |
| timedtamp | 8  | 精确到年月日时分秒, 如: 2016-09-01 09:10:11 |

**MySQL 5.6.4 之后, `datetime` 和 `timestamp` 支持到微秒**

**`timestamp` 会根据时区进行转换, `datetime` 则不会**

**`timestamp` 存储范围: `1970-01-01 00::00:01 ~ 2038-01-19 03:14:07`**

**`datetime` 的存储范围: `1000-01-01 00:00:00 to 9999-12-31 23:59:59`**

**一般使用 `timestamp` 国际化**

**如存时间戳使用数字类型 `BIGINT`**


### 字符串类型 {#字符串类型}

| 类型    | 大小               | 描述            |
|-------|------------------|---------------|
| char    | 最大为255字符      | 存储定长, 容易造成空间的浪费 |
| varchar | 可以超过255个字符  | 存储变长，节省存储空间 |
| text    | 总大小为65535字节，约为64KB | 长文本数据      |

---

**根据 `MySQL` 版本的不同, 类型的大小范围可能会有改变.**


## 常用命令 {#常用命令}


### 数据库 {#数据库}

-   创建

    `CREATE DATABASE <name>;`

-   删除

    `DROP DATABASE <name>;`

-   列出数据库

    `show databases;`

-   使用数据库

    `use <name>;`

-   修改 `CHARSET`

    `ALTER DATABASE <name> DEFAULT CHARACTER SET <char name>;`

-   显示创建类型

    `show create database <name>`


### 表 {#表}

-   创建

    ``CREATE TABLE <name>(`id` INT(11) AUTO_INCREMENT),`name` VARCHAR(64) NOT NULL,...)CHARACTER SET=utf8mb4;``

-   删除

    `DROP TABLE <name>;`

-   插入

    `INSERT INTO <name>(filed1, filed2,...) VALUES (value1, value2,...),(value3,value4,...),...;`

    可同时插入多条记录

-   查询

    `SELECT filed1,filed2,... FROM <name> WHERE <condition>`

    多表查询: `SELECT t1.id,t1.name,t2.name AS desc FROM t1,t2 WHERE t1.t2_id=t2.id`, 这将会返回两个表的交集

    多表查询也可使用联合查询, 联合查询详情见后文.

    使用 `LIKE` 可以模糊查询, `%` 来表示任意字符, 如: `SELECT * FROM <name> WHERE name LIKE '%oy%';`

-   显示表
    -   显示表字段

        `desc <name>`

    -   显示表创建字段

        `show create table <name>`

-   列出表

    `show tables;`

-   修改值

    `UPDATE <name> SET <filed1>=value1 WHERE <condition>`

-   添加列

    ``ALTER TABLE <name> ADD `filed1` <type>``

-   删除列

    `ALTER TABLE <name> DROP filed1`

-   修改列类型
    -   `ALTER TABLE <name> MODIFY filed1 <type>`
    -   ``ALTER TABLE <name> CHANGE `filed1` `filed1` <type>`` 可以修改列名

-   修改 `CHARSET`
    -   修改表的默认 `CHARSET`

        `ALTER TABLE <name> DEFAULT CHARACTER SET utf8mb4`

    -   修改表及所以列的字符

        `ALTER TABLE <name> CONVERT TO CHARACTER SET utf8mb4`

    -   修改列的字符集

        `ALTER TABLE <name> CHANGE filed1 CHARACTER SET utf8mb4`


### 备份/恢复 {#备份-恢复}

-   备份

    `mysqldump -h<address> --port <port> -u<username> -p<password> <datebase name> <table name> > bak.sql`

    其中 `table name` 可选

-   恢复

    `mysql -h<address> --port <port> -u<username> -p<password> <datebase name> < bak.sql`

    导入数据到指定的数据库中


## 联合查询 {#联合查询}

假设存在表: `user(id, name, email)` 和 `user_profile(id,uid,avatar)`, 几种联合方式如下:

-   左联

    `LEFT JOIN` 或 `LEFT OUTER JOIN` 返回的结果包含左表中的所有行, 若左行在右行中匹配, 则在对应的右表中显示为 `NULL`

    `SELECT * FROM user LEFT JOIN user_profile ON user.id=user_profile.uid`

-   右联

    `RIGHT JOIN` 或 `RIGHT OUTER JOIN` 返回的结果包含右表中的所有行, 若左行在右行中匹配, 则在对应的左表中显示为 `NULL`

-   全联

    `FULL JOIN` 或 `FULL OUTER JOIN` 返回左右两表中的所有行, 如果右表中某行在左表中没有匹配, 则结果中对应行右表的部分全部为 `NULL`;
    如果左表中某行在右表中没有匹配, 则结果中对应行左表的部分全部为空 `NULL`.

-   内联

    `inner join` 是比较运算符, 只返回符合条件的行, 如:

    `SELECT * FROM user INNER JOIN user_profile ON user.id=user_profile.uid`

    等同于 `SELECT * FROM user,user_profile WHERE user.id=user_profile.uid`
