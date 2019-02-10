+++
title = "TCP BBR"
date = 2017-07-20T21:38:21+08:00
lastmod = 2019-02-10T17:57:28+08:00
tags = ["bbr"]
categories = ["BLOG"]
draft = false
+++

## 启用 BBR {#启用-bbr}

`BBR` 是在 4.9 以后的内核才开始支持的, 可以试着执行 `sudo modprobe tcp_bbr` 来确定是否支持, 没有输出就意味着支持, 将下面两行内容添加到 `/etc/sysctl.conf` 文件里开启用它:

```shell
net.core.default_qdisc = fq
net.ipv4.tcp_congestion_control = bbr
```

使用 `sysctl net.ipv4.tcp_available_congestion_control` 查看支持的算法, 用=sysctl net.ipv4.tcp\_congestion\_control= 查看当前使用的算法.

如果这个模块不会自动加载, 需要把 `tcp_bbr_powered` 添加到 `/etc/modules-load.d/modules.conf` 中.
另外测试发现, 有时 `sysctl.conf` 里的设置并不会应用, 需要手动执行一下, 因此便写了 `reload-sysctl.service` 来做这个, `systemctl enable` 这个服务后, 一切就 OK 了.


## 使用增强版 {#使用增强版}

个人电脑不建议使用增强版, 服务器上可以使用, 当然最终还是要根据两者的效果来选择.

来源于: [Debian/Ubuntu TCP BBR 改进版/增强版](https://moeclub.org/2017/06/24/278/)

-   Build

```shell
$ ./build.sh
```

-   Install

```shell
$ ./install.sh
```

-   Clean

```shell
$ rm Makefile tcp_bbr_powered.*
```

使用 `sysctl net.ipv4.tcp_available_congestion_control` 查看支持的算法, 用=sysctl net.ipv4.tcp\_congestion\_control= 查看当前使用的算法.

如果这个模块不会自动加载, 需要把 `tcp_bbr_powered` 添加到 `/etc/modules-load.d/modules.conf` 中.
另外测试发现, 有时 `sysctl.conf` 里的设置并不会应用, 需要手动执行一下, 因此便写了 `reload-sysctl.service` 来做这个, `systemctl enable` 这个服务后, 一切就 OK 了.


## 附件 {#附件}

-   build.sh

```shell
wget --no-check-certificate -qO- 'https://moeclub.org/attachment/LinuxSoftware/bbr/tcp_bbr_powered.c.deb' >./tcp_bbr_powered.c
echo 'obj-m:=tcp_bbr_powered.o' >./Makefile
echo 'clean: ' >> ./Makefile
echo -e '\trm -r Module.symvers modules.order tcp_bbr_powered.mod.* tcp_bbr_powered.o .tcp_bbr_powered.* .tmp_versions' >> ./Makefile

make -s -C /lib/modules/$(uname -r)/build M=`pwd` modules CC=`which gcc`
make clean
```

-   install.sh

```shell
[ -f ./tcp_bbr_powered.ko ] && [ -f /lib/modules/$(uname -r)/modules.dep ] && {
cp -rf ./tcp_bbr_powered.ko /lib/modules/$(uname -r)/kernel/net/ipv4
depmod -a >/dev/null 2>&1
}

# configure
modprobe tcp_bbr_powered
[ ! -f /etc/sysctl.conf ] && touch /etc/sysctl.conf
sed -i '/net.core.default_qdisc.*/d' /etc/sysctl.conf
sed -i '/net.ipv4.tcp_congestion_control.*/d' /etc/sysctl.conf
echo "net.core.default_qdisc = fq" >>/etc/sysctl.conf
echo "net.ipv4.tcp_congestion_control = bbr_powered" >>/etc/sysctl.conf

lsmod |grep -q 'bbr_powered'
[ $? -eq '0' ] && {
sysctl -p >/dev/null 2>&1
echo "Finish! "
exit 0
} || {
echo "Error, Loading BBR POWERED."
exit 1
}
```

-   reload-sysctl.service

```shell
[Unit]
Description=Reload sysctl
Before=network.target

[Service]
Type=simple
ExecStart=/usr/bin/sysctl -p

[Install]
WantedBy=multi-user.target
```
