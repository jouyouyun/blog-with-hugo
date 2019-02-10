+++
title = "Bluetooth FAQ"
author = ["Wen"]
date = 2017-11-20T21:38:21+08:00
lastmod = 2019-02-09T23:00:09+08:00
tags = ["bluetooth"]
categories = ["BLOG"]
draft = false
+++

收集遇到的 `bluetooth` 问题，使用的是 `deepin` 。

---


## 控制中心不显示蓝牙模块 {#控制中心不显示蓝牙模块}

蓝牙模块不显示时，可以按照以下步骤来查找原因：

1.  执行 `sudo dmesg|grub bluetooth` 查看输出结果
    -   输出里有 `timeout`

        此时再用 `systemctl status bluetooth.service` 看下服务有没有正常启动，错误信息是什么，然后根据错误去网上搜索答案。

        如果蓝牙是由网卡提供的，此时可以试着开关一下网卡的硬件开关。因为内核在初始化网卡之前初始化蓝牙，就会导致蓝牙设备不可用，重新初始化一次就好了。
        如果还是无效那就重启进入 `window=，再 =window` 里开关一次蓝牙再重启到 `linux` 。

    -   输出里无 `timeout`

        这就表明初始化没问题，继续下一步

---

1.  执行 `sudo hciconfig` 查看设备当前的状态
    -   状态为 `DOWN`

        则执行 =sudo hciconfig hci<number> up=，其中 **number** 为前面输出显示的。
        如果命令执行失败，参照第一步。

    -   状态为 `UP`

        继续下一步

---

1.  执行 `sudo rfkill list` 查看设备是否被 `block`
    -   被 `block`

        则执行 `sudo rfkill <dev> unblock`

如果尝试完上述步骤，蓝牙仍不可用，那就反馈吧，提供日志及 `lsusb` 输出，等待问题被解决。

---


## 蓝牙鼠标使用中频繁断开 {#蓝牙鼠标使用中频繁断开}

蓝牙鼠标在使用过程中频繁出现了断开问题，应在断开后执行 `sudo dmesg` 来查看是否有错误，如果没有错误信息，那就反馈吧。
如果出现 `firmware` 加载失败，记住这个固件名。试着从 `window` 那里获取最新的驱动，然后转成 =linux firmware=，方法如下:

1.  查找并下载本机蓝牙适配器的驱动，一般可在笔记本或蓝牙适配器的官网下载到

2.  然后解压，如果是 `.exe` 文件就安装 `innoextract` 来解压

3.  然后查看驱动信息的 `.inf` 文件，比如 `thinkpad x230 win8 64` 的文件就是 `app/Win64/bcbtums-win8x64-brcm.inf`

4.  执行 `lsusb` 获取蓝牙设备的 `ID=。如 =x230` 即是 `ID 0a5c:21e6 Broadcom Corp. BCM20702 Bluetooth 4.0 [ThinkPad]` ，然后搜索 `0a5c 21e6` 相关的内容，找到对应的 `.hex` 文件。如 `x230` 即是 `BCM20702A1_001.002.014.1443.1453.hex`

5.  使用 `hex2hcd xxx.hex` 将 `xxx.hex` 转为 `.hcd` 文件，然后重命名为缺失的固件名，复制到 `/lib/firmware/` 下对应的位置

---


## 蓝牙音箱/耳机连上就断开 {#蓝牙音箱-耳机连上就断开}

蓝牙音箱如果断开，先按照蓝牙耳机的方法试一下。如果不是，那就考虑是不是电脑的蓝牙不支持蓝牙高保真输出，因为 `deepin` 默认开启高保真。
试着安装 `pavucontrol` 将蓝牙的 `profile(配置)` 切换为其它的，不过这样会导致音效很差，建议换掉电脑的蓝牙设备吧。
