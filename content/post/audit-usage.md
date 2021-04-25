+++
title = "Audit 简单使用"
date = 2021-04-25T06:20:21+08:00
lastmod = 2021-04-25T15:46:23+08:00
tags = ["audit", "file", "exec", "pid", "cmd", "program"]
categories = ["BLOG"]
draft = false
+++

`audit` 是 `linux` 下的审计系统，包含内核模块和系统服务。可使用 `audit` 对文件变更、系统调用、任务创建、用户事件等进行审计。

可使用 `sudo auditctl -s` 获取内核审计模块的状态，可使用 `sudo systemctl status auditd` 获取审计服务的状态。在添加规则之前，需要确保审计内核模块和审计服务正常。

<!--more-->


## 常用操作 {#常用操作}


### 添加规则 {#添加规则}

`-a <list,action>` 参数可以添加规则，可用值分为 `list` 和 `action` 。 `list` 表示需要操作的事件队列， `action` 表示事件发生时的操作。

-   `list` 可用值为
    -   `task` 添加规则到每一个任务列表，只在任务创建(`fork/clone`)时触发
    -   `exit` 添加规则到系统调用列表，在系统调用退出时触发
    -   `user` 添加规则到用户消息过滤列表，用于内核过滤用户空间的事件
    -   `exclude` 添加到事件排除列表，用于不希望接收的事件

-   `action` 可用值为
    -   `never` 不产生审计记录
    -   `always` 纪录


### 删除规则 {#删除规则}

`-d <list,action>` 参数可以删除规则，与 `-a` 时对应。


### 指定选项 {#指定选项}

`-F` 参数用于指定规则选项


### 设置过滤标识 {#设置过滤标识}

`-k` 可为规则指定一个标识，可认为是此条规则的 `id` ，可在搜索审计日志时使用。


### 指定系统调用 {#指定系统调用}

`-S` 可指定需要审计的系统调用


### 指定文件 {#指定文件}

`-w` 参数可指定需要审计的文件


### 指定文件权限 {#指定文件权限}

`-p` 可指定文件变更时需要的属性，可用值为 `r(read), w(write), x(execute), a(attribute change)`


### 删除文件规则 {#删除文件规则}

`-W` 参数可删除添加的文件审计规则


### 查询规则列表 {#查询规则列表}

`-l` 参数可获得已添加的规则列表


## 示例场景 {#示例场景}


### 审计文件变动 {#审计文件变动}

审计 `/etc/systemd/system.conf` 文件的写入变动，添加规则：

`sudo auditctl -w /etc/systemd/system.conf -p w -k system_conf_audit`

当这个文件以写入方式打开或发生写入后，会记录相关的审计日志，可用 `ausearch` 查看：

`sudo ausearch -k system_conf_audit`

删除这条审计规则可用：

`sudo auditctl -W /etc/systemd/system.conf -p w -k system_conf_audit`


### 审计系统调用 {#审计系统调用}

审计 `kill` 调用，规则：

`sudo auditctl -a exit,always -F arch=b64 -S kill -k kill_audit`

这里的 `-F arch=b64` 是表示检讨 `64` 位的 `kill` 调用，必须指定。这样在发生 `kill` 调用时就会产生审计日志，查看方式还是使用 `ausearch`

`sudo ausearch -k kill_audit`

删除这条审计规则可用：

`sudo auditctl -d exit,always -S kill -k kill_audit`


### 审计iptables变更 {#审计iptables变更}

`iptables` 会触发 `setsockopt` 调用，因此通过这个系统调用来审计 `iptables` 变更：

`auditctl -a exit,always -F arch=b64 -S setsockopt -k iptables_audit`

这样在 `iptables` 规则发生变更时就会产生审计日志，但这样只能看到改变 `iptables` 规则的程序 `pid` ，看不到程序名，可结合 `execve` 调用来获取程序名：

`auditctl -a exit,always -F arch=b64 -S setsockopt -S execve -k iptables_audit`

查看日志、删除规则与上文相同。


## 更多资料 {#更多资料}

-   `man auditctl`
