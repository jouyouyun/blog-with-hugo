+++
title = "Linux 休眠自动唤醒"
date = 2018-10-21T21:38:21+08:00
lastmod = 2019-02-12T14:34:50+08:00
tags = ["suspend", "wakeup", "rtc"]
categories = ["BLOG"]
draft = false
+++

最近做无线网络在系统多次休眠唤醒后是否正常的测试，但每次休眠后都需要手动点击键盘来唤醒系统，显而易见的这种方法需要人来值守，很不放便。

此时便想是否有做自动唤醒的程序，毕竟这种需求也挺正常的，应该有人已经做过了。因此就开始查找自动唤醒的方法，果然就发现了 `rtcwake` 这个程序，这个程序是让系统进入休眠模式并在指定的时间后唤醒。立刻便觉得这个程序很神奇，然后就去看它的源码来找出它的实现方式，这边发现了本文的主题 `RTC` 子系统。


## `RTC` 子系统 {#rtc-子系统}

`RTC` 子系统在目录 `/sys/class/rtc/` 下，会根据设备创建对应的目录，需要在内核中开启支持，配置如下：

<!--more-->

```shell
CONFIG_HPET_EMULATE_RTC=y
CONFIG_PM_TRACE_RTC=y
CONFIG_RTC_LIB=y
CONFIG_RTC_CLASS=y
CONFIG_RTC_HCTOSYS=y
CONFIG_RTC_HCTOSYS_DEVICE="rtc0"
# CONFIG_RTC_DEBUG is not set
# RTC interfaces
CONFIG_RTC_INTF_SYSFS=y
CONFIG_RTC_INTF_PROC=y
... (略)
```

`debian/ubuntu` 的系统可以在 `/boot/config-$(uname -r)` 查看内核的编译选项，=archlinux= 可以在 `/proc/config.gz` 查看。

`RTC` 目录下的文件含义可以在 [sysfs-class-rtc](https://www.kernel.org/doc/Documentation/ABI/testing/sysfs-class-rtc) 中查看。


### `wakealarm` {#wakealarm}

`RTC` 目录中的 `wakealarm` 文件内容便是下次触发唤醒时间的时间，默认这个文件是没有值的。文件的内容需要设置是时间的秒数，可以向下面这样设置：

```shell
# echo $(date +%s --date 'now + 1 minutes') > /sys/class/rtc/rtc0/wakealarm
```

这就表示在 `1 minute` 后触发唤醒事件。

所以做休眠唤醒测试的流程如下：

1.  设置下次唤醒的时间到 `wakealarm` 中
2.  休眠系统
3.  被 `wakealarm` 唤醒后，开始做一些期望的事情(如检查系统是否正常)，然后继续执行步骤 =1=，就这样一直循环，直至满足条件后终止


## FAQ {#faq}

-   设置无效？
    首先检查内涵是否开启了 `rtc` 支持，如果支持就检查时间标准是否是 `UTC=，=localtime` 时间标准时设置 `wakealarm` 是不生效的，内容一直是空的。通过 `timedatectl` 命令可以查询和设置时间标准。
