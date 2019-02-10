+++
title = "Linux 时间标准"
date = 2018-08-27T21:38:21+08:00
lastmod = 2019-02-10T17:54:51+08:00
tags = ["timezone", "localtime", "rtc", "utc"]
categories = ["BLOG"]
draft = false
+++

## 系统如何设置时间 {#系统如何设置时间}

1.  开机时根据硬件时钟来设置系统时钟.
2.  如果开启了 `NTP` 服务, 会根据网络时间来校正系统时钟，确保时间准确.
3.  关机时会根据系统时钟来设置硬件时钟(`localtime` 标准除外).


### 时钟 {#时钟}

-   硬件时钟: 即实时时钟 `RTC` 或 `CMOS` 时钟, 仅能保存年、月、日、时、分、秒这些时间数值，无法保存时间标准和是否使用夏令时调节.
-   系统时钟: 即软件时间, 与硬件时间分别维护，保存了时间、时区和夏令时设置.初始系统时钟是从硬件时间计算得来，计算时会考虑 `/etc/adjtime` 的设置.
-   `/etc/adjtime`: 这个文件中记录了时区标准和时间偏移, 根据硬件时钟设置系统时间和设置硬件时钟都会根据时间偏移来计算的.所有的时钟都是有误差的，但是一般有固定的偏移, 这种于基值的差称为“time skew”或“时间偏移”.

---


### 时间标准 {#时间标准}

-   `UTC` 是与时区无关的全球时间标准, 会根据夏令时自动调整时间, ****推荐使用****.
-   `localtime` 则依赖于当前时区, 不会根据夏令时自动调整.

通常在 `window+linux` 双系统中遇到的时间问题就是使用的时间标准不一致导致的，只要将两个系统的时间标准调成一致的，就能解决此问题，推荐将 `window` 的时间标准改为 `UTC` 。

---


### 示例 {#示例}

假设当前 `RTC` 时间为 `12:00`, 时区为 `Asia/Shanghai`, 时区的 `offset` 为8, 两种不同的时间标准的处理如下:

-   `UTC` 时间标准: 系统开机时会根据 `RTC` 时间和时区的 `offset` 将系统时间调整为 `20:00`, `UTC` 时区的时间为 `12:00`, 关机时使用 `UTC` 的时间来设置 `RTC`.
-   `localtime` 时间标准: 开始时根据 `RTC` 和时区 `offset`, 将系统时间调整为 `12:00`, `UTC` 时区的时间为 `04:00`, 关机时不再设置 `RTC`.

如果开启了 `NTP` 服务, 系统会在运行时通过网络时间校准本机的 `UTC` 时间, 本机的 `UTC` 时间改变后, 系统时钟的时间也会跟着改变. 所以建议开启 `NTP` 服务, 这能保证时间的准确性，
但如果采用的是 `localtime` 时间标准，在 `NTP` 同步后不会修改 `RTC` 时间。

---


## systemd 时间服务 {#systemd-时间服务}


### timedatectl {#timedatectl}

用来查询和设置系统时间的命令, 使用方法如下:

```shell
timedatectl [OPTIONS...] COMMAND ...

Query or change system time and date settings.

-h --help                Show this help message
--version             Show package version
--no-pager            Do not pipe output into a pager
--no-ask-password     Do not prompt for password
-H --host=[USER@]HOST    Operate on remote host
-M --machine=CONTAINER   Operate on local container
--adjust-system-clock Adjust system clock when changing local RTC mode

Commands:
status                   Show current time settings
set-time TIME            Set system time
set-timezone ZONE        Set system time zone
list-timezones           Show known time zones
set-local-rtc BOOL       Control whether RTC is in local time
set-ntp BOOL             Enable or disable network time synchronization
```

设置时区时会创建一个 `/etc/localtime` 软链接，指向 `/usr/share/zoneinfo/` 中的时区文件.

---


### systemd-timesyncd {#systemd-timesyncd}

提供了时间同步的功能, 基于 `SNTP` 实现, `SNTP` 是 `NTP` 协议的一种简单实现, 不需要长时间的存储状态, 适用与非服务器的桌面系统和嵌入式系统.

`deepin` 的时间同步功能就是用它实现的.

---


### systemd-timedated {#systemd-timedated}

`timedatectl` 的功能的 `dbus` 接口, 提供的方法和属性如下:

```shell
$ gdbus introspect --system --dest org.freedesktop.timedate1 --object-path /org/freedesktop/timedate1
node /org/freedesktop/timedate1 {
interface org.freedesktop.timedate1 {
methods:
SetTime(in  x usec_utc,
in  b relative,
in  b user_interaction);
SetTimezone(in  s timezone,
in  b user_interaction);
SetLocalRTC(in  b local_rtc,
in  b fix_system,
in  b user_interaction);
SetNTP(in  b use_ntp,
in  b user_interaction);
signals:
properties:
readonly s Timezone = 'Europe/Berlin';
readonly b LocalRTC = false;
readonly b NTP = true;
readonly b NTPSynchronized = true;
readonly t TimeUSec = 1467291739170844;
readonly t RTCTimeUSec = 1467291739000000;
};
```

---


## Timezone {#timezone}

时区列表是由 `tzdata` 这个包提供的, 安装在 `/usr/share/zoneinfo/` 下面.

有些地方为了节约能源, 在夏天时会把时间调慢. 当时区中有夏令时系统会自动调整时间.

使用 `zdump` 命令可以查看时区的详细信息, 如查询 `America/New_York` 的夏令时时区信息：

```shell
~  zdump -v -c 2016,2017 America/New_York
America/New_York  Sun Mar 13 06:59:59 2016 UT = Sun Mar 13 01:59:59 2016 EST isdst=0 gmtoff=-18000
America/New_York  Sun Mar 13 07:00:00 2016 UT = Sun Mar 13 03:00:00 2016 EDT isdst=1 gmtoff=-14400
America/New_York  Sun Nov  6 05:59:59 2016 UT = Sun Nov  6 01:59:59 2016 EDT isdst=1 gmtoff=-14400
America/New_York  Sun Nov  6 06:00:00 2016 UT = Sun Nov  6 01:00:00 2016 EST isdst=0 gmtoff=-18000
```

其中 `isdst=1` 就表示夏令时被启用.
