+++
title = "chroot 脚本"
date = 2014-07-05T21:38:21+08:00
lastmod = 2019-02-12T14:30:40+08:00
tags = ["chroot"]
categories = ["NOTE"]
draft = false
+++

最近测试了以下 `chroot` ，然后把步骤写成了下面两个脚本，方便以后使用。

**使用时需要确定每条命令都执行成功了**

`Arch` 分区结构

```shell
/dev/sda5	/
/dev/sda1	/boot
/dev/sda6	/home
/dev/sda8	Swap
```

<!--more-->


## `chroot` 挂载脚本 {#chroot-挂载脚本}

```shell
#!/bin/bash

_ROOT=/dev/sda5
_BOOT=/dev/sda1
#_HOME=/dev/sda6

#创建目录结构
sudo mkdir -p /mnt/arch
sudo mkdir -p /mnt/arch/boot
#sudo mkdir -p /mnt/arch/home

#挂载分区
sudo mount $_ROOT /mnt/arch
sudo mount $_BOOT /mnt/arch/boot
#sudo mount $_HOME /mnt/arch/home

#挂载临时文件系统
cd /mnt/arch
sudo mount -t proc proc proc/
sudo mount -t sysfs sys sys/
sudo mount -o bind /dev dev/
sudo mount -t devpts pts dev/pts

#如果你已经建立了一个网络连接并且想要在chroot环境中使用它，
#你可能不得不拷贝你的DNS服务器配置让你能够解析主机名
sudo  cp -L /etc/resolv.conf etc/resolv.conf

# chroot 并指定 shell
sudo chroot /mnt/arch /usr/bin/bash
```


## `chroot`  卸载脚本 {#chroot-卸载脚本}

```shell
#!/bin/bash

#在退出 chroot 后执行此脚本

#卸载临时文件系统和已挂载的设备
#sudo umount {proc,sys,dev/pts,dev,boot,home}
sudo umount {proc,sys,dev/pts,dev,boot}

#尝试卸载根分区
cd /
sudo umount /mnt/arch
```
