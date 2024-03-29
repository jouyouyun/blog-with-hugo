+++
title = "在 VPS 中搭建 VPN 服务器"
date = 2014-07-04T21:38:21+08:00
lastmod = 2019-02-12T14:30:30+08:00
tags = ["vps", "vpn", "ssh"]
categories = ["BLOG"]
draft = false
+++

最近 `Google` 被封，而用 `Goagent` 做代理进行访问的效果而不是很理想，所以就想去买 `VPN/VPS` . 几经考虑后，就去买了个便宜 `VPS` , 这样也就可以分享给家人用了。

`VPS` 中安装的是 `Ubuntu=，所以搭建 =VPN` 服务器也就是在 `Ubuntu` 上面搭建，具体方法如下。

注意：下面所有的操作都是以 `root` 用户进行的，非 `root` 用户请在命令前加上 `sudo`

**安装 `pptp`**

`apt-get install pptpd`

配置 `IP` 地址范围及服务器 `IP`

`vi /etc/pptpd.conf`

取消下面内容的注释

<!--more-->

```shell
option /etc/ppp/pptpd-options
localip 192.168.0.1
remoteip 192.168.0.234-238,192.168.0.245
```

**添加用于登录的账户**

`vi /etc/ppp/chap-secrets`

格式如下

`用户名	pptpd	"密码"	*`

密码需要用英文双引号，最后的星号(\*)代表允许接入的 `IP` 可以是任意 `IP`

**设置 `DNS`**

`vi /etc/ppp/pptpd-options`

找到 `ms-dns=，取消掉注释，并修改 =DNS` 地址，推荐使用 `Google DNS`

```shell
8.8.8.8
8.8.4.4
```

**打开内核 `IP` 转发**

`vi /etc/sysctl.conf`

取消以下内容的注释：

`net.ipv4.ip_forward=1`

使其生效

```shell
sysctl -p
/etc/init.d/procps restart
```

**安装 `iptables` 并设置**

开放主机防火墙 `VPN` 端口（默认为 `1723` ）

`iptables -I INPUT -p tcp --dport 1723 -j ACCEPT`

立刻让内核支持 `NAT`

`iptables --table nat --append POSTROUTING --jump MASQUERADE`

为了让每次重启都生效，还需要在 `/etc/rc.local` 文件添加命令

`iptables --table nat --append POSTROUTING --jump MASQUERADE`


## SSH 代理 {#ssh-代理}

如要使用 `ssh` 作为代理，则需要创建用户。若此用户只作为 `ssh` 代理用，则不要登录 `shell` 及主目录，使用以下命令

`useradd -s /bin/false  test`

在客户端(`linux`)中使用以下命令连接

`ssh -qTfnN -D 7070 test@xxx.xxx.xxx.xxx`
