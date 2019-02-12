+++
title = "Leanote Installation"
date = 2017-07-17T21:38:21+08:00
lastmod = 2019-02-12T14:34:41+08:00
tags = ["leanote"]
categories = ["BLOG"]
draft = false
+++

`Leanote` 是一款开源的笔记软件, 支持使用 `markdown` 来编辑笔记, 并可选择是否将笔记导出成博客, 这对于知识整理及写博客都是非常方便的, 故来尝试下.


## 安装 {#安装}

`Leanote` 是使用 `Golang` 编写的, 可以直接使用官方编译好的可执行文件.
另外它使用的是 `mongodb` 数据库, 可以从源里安装, 也可以从官网下载.
以下的安装和配置都是基于最新的版本, 如果中途出错, 请看官方文档.

-   `Leanote` 下载地址: [见这里](http://leanote.org/#download)

-   `mongodb` 下载地址: [见这里](https://www.mongodb.com/download-center)

<!--more-->


## 配置 {#配置}


### `Leanote` {#leanote}

解压下载的文件, 然后进入 `Leanote` 解压后的目录, 编辑 `conf/app.conf`, 按需修改, 如 `site url, db user/passwd` 等.
不过必须更改 `app.secret`, 随意更改其中的几个字符即可.


### `mongodb` {#mongodb}

解压下载的文件后进入 `bin` 目录, 然后执行 `./mongod --dbpath <data path>` 来启动数据库, `data path` 需要是绝对路径，要保证目录结构存在.

然后另开一个终端来导入 `Leanote` 的初始设置 `./mongorestore -h localhost -d leanote --dir <your leanote path>/mongodb_backup/leanote_install_data/`.
导入完成后，为了安全性, 为 `leanote` 数据库创建用户认证, 命令如下:

```shell
$>./mongo
> show dbs #　查看数据库
leanote	0.203125GB
local	0.078125GB
> use leanote # 切换到 leanote
switched to db leanote
> # 添加一个用户 root, 密码是 abc123
> db.createUser({
    user: 'root',
    pwd: 'abc123',
    roles: [{role: 'dbOwner', db: 'leanote'}]
});
# 测试下是否正确
> db.auth("root", "abc123");
1 # 返回 1 表示正确
```

其中密码中不要含有 `@` 符号, 不然会连接数据库失败, 原因未知;

创建了用户后, 需要在 `leanote/conf/app.conf` 中填入. 这样就配置完成了, 可以执行 `leanote` 下的 `bin/run.sh` 来启动 `leanote` 了.


## 账户管理 {#账户管理}

默认提供了 `admin` 和 `demo` 账户, 使用 `conf/app.conf` 里指定的管理员账户可以更改 `leanote` 的设置(如: 首页设置, 开放注册等)以及更改用户的密码.

另外可以直接操作数据库来管理用户, 下面介绍下如何创建, 更改以及删除用户, 更多操作请查看 [mongodb 文档](http://docs.mongoing.com/manual-zh/crud.html).
数据库名为 `leanote`, 账户表名为 `users` , 使用 `db.users.find()` 可以看到表中所有的记录 .


### 创建账户 {#创建账户}

新建的账户名为 `test1`, 邮箱为 `test1@gmail.com`, 密码由于是加密的, 就不在这里设置, 通过管理员到界面上去重置密码.

```shell
> use leanote
> db.users.insert({"Username": "test1", "Email": "test1@gmail.com"})
```


### 更改 {#更改}

如更改 `admin` 名为 `root`, 更改了 `admin` 后也需要到 `<leanote path>/conf/app.conf` 去更改管理员的名称, 如果设置了的话.

```shell
> use leanote
> db.users.updateOne({"Username": "admin"}, {$set: {"Username": "root"}})
```


### 删除 {#删除}

如删除 `demon` 用户

```shell
> use leanote
> db.users.remove({"Username": "demo"})
```
