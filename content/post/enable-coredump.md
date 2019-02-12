+++
title = "Enable Coredump"
date = 2016-04-25T19:04:21+08:00
lastmod = 2019-02-12T14:32:12+08:00
tags = ["coredump"]
categories = ["BLOG"]
draft = false
+++

## 简介 {#简介}

当一个程序崩溃时会产生一个 `core` 文件, 里面记录了这个程序崩溃的原因, 这些信息对于开发者是很有用的.

但是 `core dumps` 在大多数 `linux` 上是被禁用的, 下面就介绍下如何开启这个功能.


## 开启 {#开启}

可以通过 `ulimit -c` 的输出知道此功能是否开启, 输出为 `0` 表示禁用, 否则为开启.


### 临时开启 {#临时开启}

通过 `ulimit -c unlimited` 命令来开启 `coredump` 功能, 它会程序运行的当前目录生成一个 `core` 文件, 新的 `core` 会覆盖旧的.

通过可以修改 `/proc/sys/kernel/core_uses_pid` 文件内容为 `1` 来生成以 `pid` 作为扩展名的 `core` 文件.

通过修改 `/proc/sys/kernel/core_pattern` 可以修改 `core` 文件的保存位置和格式. 如把内容改为 `/tmp/core-%e-%p-%t`, 就表示把 `core` 文件保存到 `/tmp` 目录下, 文件名的格式为 `core-命令名-pid-时间戳` .

<!--more-->


### 永久开启 {#永久开启}

首先把开启 `coredump` 功能的命令 `ulimit -c unlimited >/dev/null 2>&1` 放到 `/etc/profile` 中.

然后修改 `core` 文件的保存位置和格式, 在 `/etc/sysctl.conf` 文件中添加一下内容:

```shell
kernel.core_uses_pid = 1
kernel.core_pattern = /tmp/core-%e-%s-%u-%g-%p-%t
```

修改完 `/etc/sysctl.conf` 后, 执行 `sudo sysctl -p` 重新加载配置.

`core pattern` 说明:

```shell
%% – A single % character
%p – PID of dumped process
%u – real UID of dumped process
%g – real GID of dumped process
%s – number of signal causing dump
%t – time of dump (seconds since 0:00h, 1 Jan 1970)
%h – hostname (same as ’nodename’ returned by uname(2))
%e – executable filename
```


## 参考 {#参考}

-   [HOWTO enable core-dumps](<http://en.linuxreviews.org/HOWTO%5Fenable%5Fcore-dumps>)

-   [HowTo: Debug Crashed Linux Application Core Files](<http://www.cyberciti.biz/tips/linux-core-dumps.html>)
