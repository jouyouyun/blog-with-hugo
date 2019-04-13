+++
title = "InfluxDB 简介"
date = 2019-04-13T16:55:00+08:00
lastmod = 2019-04-14T01:13:09+08:00
tags = ["influxdb", "tsdb", "series", "measurement"]
categories = ["BLOG"]
draft = false
+++

`InfluxDB` 是一个时间序列数据库(`TSDB`), 被设计用来处理高写入、高查询负载，是 [TICK](https://www.influxdata.com/time-series-platform/) 的一部分。

`TSDB` 是针对时间戳或时间序列数据进行优化的数据库，专门为处理带有时间戳的度量和事件或度量而构建的。
而时间序列数据可以是随时间跟踪、监视、下采样和聚合的度量或事件，如服务器指标、应用程序性能、网络数据、传感器数据以及许多其他类型的分析数据。

<!--more-->


## 关键特性 {#关键特性}

-   能够高速读取和压缩时间序列数据
-   使用 `Go` 编写，能够但文件运行，没有依赖
-   提供了简单、高效的 `HTTP` 读写接口
-   能够使用插件支持其他的数据协议，如： `Graphite=， =collectd` 和 `OpenTSDB`
-   可轻松使用 `SQL` 语言查询聚合数据
-   能够使用 `Tag` 进行快速高效的查询
-   支持保留策略(`Retention Policy`), 能够自动清理旧数据
-   支持持续查询，能够自动定期计算聚合数据，提高了查询的效率

**注意：** 开源版本的 `InfluxDB` 只运行在单个节点上，如需更好的性能或避免单点故障，请使用企业版。


## 安装 {#安装}

-   deepin/Ubuntu/Debian

    `sudo apt install influxdb influxdb-cli`

-   Archlinux

    `yaourt -S influxdb` 或 `sudo pacman -S influxdb`

其他请参见：[Installing InfluxDB](https://docs.influxdata.com/influxdb/v1.7/introduction/installation/)


## 配置 {#配置}

`InfluxDB` 的配置文件为： `/etc/influxdb/influxdb.conf` ，选项详情请参见：[Configuration Settings](https://docs.influxdata.com/influxdb/v1.7/administration/config/)，这里就不在赘述。


## 基本操作 {#基本操作}


### 服务相关 {#服务相关}

-   启用/停止服务

    `systemctl start/stop influxdb.service`


### 数据库 {#数据库}

-   连接数据库

    使用 `influx` 命令连接数据库，参看其帮组手册了解使用方法

-   创建数据库

    `CREATE DATABASE <name>`

-   删除数据库

    `DROP DATABASE <name>`

-   列出数据库

    `SHOW DATABASES`

-   选择数据库

    `USE <name>`


### 写入查询 {#写入查询}

`InfluxDB` 中使用 `measurement` 表示表， `tags` 表示表的元数据， `fields` 表示数据。
表的 `scheme` 不用定义， `null` 值也不会被存储。

`tag` 可理解为表中需要索引的列， `field` 是不需要索引的列， `point` 表示一条记录。
`tags` 之间或 `fields` 之间使用 `','` 分割， 而 `tags` 与 `fields` 之间使用空格分割。

-   删除表

    `DROP MEASUREMENTS <name>`

-   列出表

    `SHOW MEASUREMENTS`

-   写入数据

    `point` 写入的语法如下：

    ```sql
    <measurement>[,<tag-key>=<tag-value>...] <field-key>=<field-value>[,<field2-key>=<field2-value>...] [unix-nano-timestamp]
    ```

    插入一条 `cpu load` 的数据： `INSERT cpu_load,machine=001,region=ch value=0.56` ，
    这就向名为 `cpu_load` 的 `measurement` 中添加了 `tags` 为 `machine` 和 `region` ， `fields` 为 `value` 的 `point` 。

    不指定 `timestamp` 时，默认会使用 **本地的当前时间** 作为 `timestamp` 。

-   查询数据

    查询语法：

    ```sql
    SELECT <field_key>[,<field_key>,<tag_key>] FROM <measurement_name>[,<measurement_name>]
    ```

    **查询语句中必须要有 `field` 存在** ，查询语句还支持 `Go` 风格的正则，下面给出一些例子。

    -   `SELECT * FROM cpu_load`

        查询 `cpu_load` 中的所有 `fields` 和 `tags`

    -   `SELECT *::field FROM cpu_load`

        查询 `cpu_load` 中的所有 `fields`

    -   `SELECT value,machine FROM cpu_load`

        只查询 `value` 与 `machine`

    -   `SELECT value::field,machine::tag FROM cpu_load`

        只查询 `value` 与 `machine` ，并限定了类型，如果类型错误将返回 `null` ，如果所有查询字段的类型都错误将没有 `point` 返回

    -   `SELECT * FROM /.*/`

        查询所有表中的所有字段

    **注意：** `WHERE` 语句后的值不为数字的，必须引起来。

    更多用法参见： [Data exploration using InfluxQL](https://docs.influxdata.com/influxdb/v1.7/query%5Flanguage/data%5Fexploration/) 。

-   删除 `Point`

    `InfluxDB` 不支持 `Point` 的删除操作，但可以通过 `Retention Policy` 清理 `Point` 。


### SERIES {#series}

`SERIES` 是 `measurement,<tag1>,<tag2>...` 的集合，如之前的写入的 `SERIES` 就是 `cpu_load,machine,region`

查看语法：

```sql
SHOW SERIES FROM [measurement],[tag1],[tag2]...
```

`FROM` 可以不加，如：

-   `SHOW SERIES` 显示数据库中所有的 `series`
-   `SHOW SERIES FROM cpu_load` 显示表 `cpu_load` 中的所有 `series`


#### 删除 {#删除}

-   DROP

    `DROP` 将删除所有的记录，并删除所有的索引，语法：

    ```sql
    DROP SERIES FROM <measurement> WHERE [condition]
    ```

-   DELETE

    `DELETE` 将删除所有的记录，但不会删除索引，并支持在 `WHERE` 语句中使用 =timestamp=，语法：

    ```sql
    DELETE FROM <measurement_name> WHERE [<tag_key>='<tag_value>'] | [<time interval>]~
    ```


### 持续查询 {#持续查询}

连续查询(`Continuous Queries` 简称 `CQ`)是 `InfluxQL` 对实时数据自动周期运行的查询，然后把查询结果写入到指定的 `measurement` 中。
语法如下：

```sql
CREATE CONTINUOUS QUERY <cq_name> ON <database_name>
BEGIN
  <cq_query>
END
```

删除语法： `DROP CONTINUOUS QUERY <cq_name> ON <database_name>`

`cq_query` 需要一个函数，一个 `INTO` 子句和一个 `GROUP BY time()` 子句：

```sql
SELECT <function[s]> INTO <destination_measurement> FROM <measurement> [WHERE <stuff>] GROUP BY time(<interval>)[,<tag_key[s]>]
```

**注意：** 在 `WHERE` 子句中， `cq_query` 不需要时间范围。 `InfluxDB` 在执行 `CQ` 时自动生成 `cq_query` 的时间范围。
`cq_query` 的 `WHERE` 子句中的任何用户指定的时间范围将被系统忽略。

如创建一个一分钟采样一次 `cpu_load` 并写入 `cpu_load_1min` 表的连续查询:

```sql
CREATE CONTINUOUS QUERY "cpu_load_1min" ON "learn_test"
BEGIN
    SELECT mean("value") INTO "cpu_load_1min" FROM "cpu_load" GROUP BY time(1m)
END
```

`value` 将以 `mean` 为名保存在 `cpu_load_1min` 中。

更多高级用法参加： [InfluxQL Continuous Queries](https://docs.influxdata.com/influxdb/v1.7/query%5Flanguage/continuous%5Fqueries/)


### 保留策略 {#保留策略}

`InfluxDB` 是没有提供直接删除数据记录的方法，但是提供数据保存策略，主要用于指定数据保留时间，超过指定时间，就删除这部分数据。
可以有多个 `RP` 并存，但 `default` 表明默认策略。

更多用法参见： [Database management using InfluxQL](https://docs.influxdata.com/influxdb/v1.7/query%5Flanguage/database%5Fmanagement/#retention-policy-management) 。

-   列出

    `SHOW RETENTION POLICY ON <database name>`

-   创建

    创建语法：

    ```sql
    CREATE RETENTION POLICY <retention_policy_name> ON <database_name> DURATION <duration> REPLICATION <n> [SHARD DURATION <duration>] [DEFAULT]
    ```

    `REPLICATION` 子句确定每个点在集群中存储多少个独立副本，其中 `n` 是数据节点的数量，对单节点实例无效。

    碎片持续时间子句确定碎片组覆盖的时间范围，是一个 `duration` 文字，不支持 `INF (infinite) duration` 。这个设置是可选的。
    默认情况下，碎片组的持续时间由保留策略的持续时间决定:

| RP Duration               | Shard Duration |
|---------------------------|----------------|
| < 2 days                  | 1 hour         |
| >= 2 days and <= 6 months | 1 day          |
| > 6 months                | 7 days         |

如果 `RP Duration` 大于 `0s` 小于 `1 hour` ， `Shard Duration` 仍将设置为 `1 hour` 。

-   删除

    `DROP RETENTION POLICY <rp_name>`

-   修改

    ```sql
    ALTER RETENTION POLICY <rp_name> ON <database name> DURATION <duration> REPLICATION <n> [SHARD DURATION <duration>] DEFAULT
    ```


## HTTP 接口 {#http-接口}


### `/query` {#query}

数据主要使用 `query` 接口查询，下面给出一些常见用法，而更多用法参见： [Querying data with the HTTP API](https://docs.influxdata.com/influxdb/v1.7/guides/querying%5Fdata/) 。

-   创建数据库

    `POST` 请求可用于创建数据库，如：

    ```shell
    curl -X POST http://localhost:8086/query --data-urlencode "q=CREATE DATABASE <database name>"
    ```

-   查询

    ```shell
    curl -X GET http://localhost:8086/query?pretty=true --data-urlencode 'db=<database name>' --data-urlencode 'q=SELECT "field1","tag1"... FROM <measurement> WHERE <condition>'
    ```

-   多个查询

    多个查询语句间用 `;` 分割，如:

    ```shell
    curl -X GET http://localhost:8086/query?pretty=true --data-urlencode 'db=<database name>' --data-urlencode 'q=SELECT "field1","tag1"... FROM <measurement> WHERE <condition>;SELECT fields FROM <measurement>'
    ```

    **最大行限制(`max-row-limit`)** 允许使用者限制返回结果的数目，以保护InfluxDB不会在聚合结果的时候导致的内存耗尽。

    **分块(chunking)** 可以设置参数 `chunked=true` 开启分块，使返回的数据是流式的 `batch` ，而不是单个的返回。返回结果可以按 `100` 数据点被分块，为了改变这个返回最大的分块的大小，
    可以在查询的时候加上 `chunk_size` 参数，例如返回数据点是每 `20000` 为一个批次。

    ```shell
    curl -X GET 'http://localhost:8086/query' --data-urlencode "db=<name>" --data-urlencode "chunked=true" --data-urlencode "chunk_size=100" --data-urlencode "q=SELECT * FROM cpu_load"
    ```


### `/write` {#write}

发送 `POST` 请求是写入数据的主要方式，，下面给出一些常见用法，而更多用法参见： [Writing data with the HTTP API](https://docs.influxdata.com/influxdb/v1.7/guides/writing%5Fdata/) 。

-   插入一条 `Point`

    ```shell
    curl -X POST http://localhost:8086/write?db=<database name> --data-binary "cpu_load,machine=001,region=cn value=0.56 1555164637838240795"
    ```

    必须指定 `database name`

-   插入多条 `Point`

    多条 `Point` 之间用行分割，如：

    ```shell
    curl -X POST http://localhost:8086/write?db=<database name> --data-binary "cpu_load,machine=001,region=cn value=0.56 1555164637838240795
    cpu_load,machine=001,region=cn value=0.65 1555164637838340795
    cpu_load,machine=003,region-cn value=0.6 1555164637839240795"
    ```

    如果需要写入 `Point` 过多，可以将 `Point` 放入文件中，然后通过 `POST` 请求上传。
    文件(`cpu_data.txt`)内容如：

    ```shell
    cpu_load,machine=001,region=cn value=0.56 1555164637838240795
    cpu_load,machine=001,region=cn value=0.65 1555164637838340795
    cpu_load,machine=003,region-cn value=0.6 1555164637839240795
    ```

    然后上传：

    ```shell
    curl -X POST http://localhost:8086/write?db=<database name> --data-binary @cpu_data.txt
    ```
