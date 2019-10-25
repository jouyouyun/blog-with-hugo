+++
title = "触摸屏校正"
lastmod = 2019-10-25T13:55:59+08:00
tags = ["touchscreen", "calibration", "calibrator"]
categories = ["BLOG"]
draft = false
+++

有些设备的触摸屏不准，需要校正后才能正常使用。校正实际上就是配置设备在 `Xorg` 中的属性，具体属性名与驱动相关，后文描述。

下面开始介绍校正方法：


## 准备 {#准备}

安装需要用到的工具：

<!--more-->

-   xinput-calibrator : 触摸屏校正工具
-   xinput : 屏幕映射工具，多屏时使用

---


## 校正 {#校正}

1.  执行 `xinput_calibrator --list` 查看触摸设备列表
2.  校正设备 `xinput_calibrator -v --device <device name or id>`

    `device name or id` 从上面的  `list` 中得到

此时会出现校正界面，按照提示点击，成功后程序会输出校正后的数据。

接着根据驱动的不同选择不同的设置方法，查看设备使用的驱动可通过 `/var/log/Xorg.0.log` 文件得到。

-   evdev

    使用 `evedev` 驱动时可直接使用 `xinput_calibrator` 输出的数据，无需其它设置。

    测试时使用 `xinput` 设置 `Evdev Axis Calibration` 属性。

-   libinput

    使用 `libinput` 驱动时需要将 `xinput_calibrator` 输出的数据进行计算才可使用，计算方法参见： [Libinput breaks xinput\_calibrator](https://wiki.archlinux.org/index.php/Talk:Calibrating%5FTouchscreen#Libinput%5Fbreaks%5Fxinput%5Fcalibrator)

    在此将计算方法写成了一个脚本，内容如下：

    ```python
    #!/usr/bin/env python3
    # -*- coding: UTF-8 -*-

    import sys

    def usage():
        info = "Usage: " + sys.argv[0]
        info += " <screen_width> <screen_height>"
        info += " <click_0_x> <click_0_y>"
        info += " <click_3_x> <click_3_y>"
        print(info)
        print("\tScreen width/hight by the command 'xrandr|grep screen' got")
        print("\tClick x/y by the command 'xinput_calibrator -v' got")
        sys.exit(0)

    def convert(screen_x, screen_y, c0_x, c0_y, c3_x, c3_y):
        a = (screen_x * 6 / 8) / (c3_x - c0_x)
        c = ((screen_x / 8) - (a * c0_x)) / screen_x
        e = (screen_y * 6 / 8) / (c3_y - c0_y)
        f = ((screen_y / 8) - (e * c0_y)) / screen_y

        print("Try set 'libinput Calibration Matrix' to '%.1f, 0.0, %.1f, 0.0, %.1f, %.1f, 0.0, 0.0, 1.0'" % (a,c,e, f))

    if __name__ == "__main__":
        if len(sys.argv) != 7:
            usage()

        convert(int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3]),
        int(sys.argv[4]),int(sys.argv[5]),int(sys.argv[6]))
    ```

    测试时使用 `xinput` 设置 `libinput Calibration Matrix` 属性。

    若上面计算的值设置后仍有问题，尝试 [Calculate the Coordinate Transformation Matrix](https://wiki.archlinux.org/index.php/Calibrating%5FTouchscreen#Calculate%5Fthe%5FCoordinate%5FTransformation%5FMatrix) 这种计算方法。

    如若都不行切换到 `evdev` 驱动，即安装 `xserver-xorg-input-evdev` 然后添加 `99-touchscreen-evdev.conf` 到 `/etc/X11/xorg.conf.d` 目录, 内容如下：

    ```shell
    Section "InputClass"
          Identifier	"calibration"
          Driver	"evdev"
          MatchProduct	"<touchscreen product>"
    EndSection
    ```

    其中的 `touchscreen product` 可以从 `/var/log/Xorg.0.log` 得到。

---


### 示例 {#示例}

如在 `Huawei` 上执行校正操作的输出如下：

```shell
deepin@deepin-PC:~$ xinput_calibrator -v
DEBUG: XInputExtension version is 2.3
DEBUG: Skipping virtual master devices and devices without axis valuators.
DEBUG: Skipping device 'Virtual core XTEST pointer' id=4, does not report Absolute events.
DEBUG: Skipping device 'SYNA1D31:00 06CB:CD48 Touchpad' id=12, does not report Absolute events.
DEBUG: Selected device: SYNA2393:00 06CB:19AC
DEBUG: Not usbtouchscreen calibrator: Not a usbtouchscreen device
DEBUG: Evdev Axis Calibration not set, setting to axis valuators to be sure.
        Setting calibration data: 0, 3000, 0, 2000
DEBUG: Successfully applied axis calibration.
DEBUG: Read axes swap value of 0.
DEBUG: Read InvertX=0, InvertY=0.
Calibrating EVDEV driver for "SYNA2393:00 06CB:19AC" id=11
        current calibration values (from XInput): min_x=0, max_x=3000 and min_y=0, max_y=2000
DEBUG: Found that 'SYNA2393:00 06CB:19AC' is a sysfs name.
DEBUG: Adding click 0 (X=382, Y=246)
DEBUG: Adding click 1 (X=2647, Y=246)
DEBUG: Adding click 2 (X=376, Y=1761)
DEBUG: Adding click 3 (X=2640, Y=1764)

Doing dynamic recalibration:
        Setting calibration data: 2, 3021, -7, 2015
DEBUG: Successfully applied axis calibration.
        --> Making the calibration permanent <--
DEBUG: Found that 'SYNA2393:00 06CB:19AC' is a sysfs name.
  copy the snippet below into '/etc/X11/xorg.conf.d/99-calibration.conf' (/usr/share/X11/xorg.conf.d/ in some distro's)
Section "InputClass"
        Identifier	"calibration"
        MatchProduct	"SYNA2393:00 06CB:19AC"
        Option	"Calibration"	"2 3021 -7 2015"
        Option	"SwapAxes"	"0"
EndSection
```

`Xorg.0.log` 中设备驱动相关的日志如下：

```shell
[  7221.421] (II) config/udev: Adding input device SYNA2393:00 06CB:19AC (/dev/input/event7)
[  7221.421] (**) SYNA2393:00 06CB:19AC: Applying InputClass "evdev touchscreen catchall"
[  7221.421] (**) SYNA2393:00 06CB:19AC: Applying InputClass "libinput touchscreen catchall"
[  7221.421] (**) SYNA2393:00 06CB:19AC: Applying InputClass "calibration"
[  7221.421] (II) LoadModule: "evdev"
[  7221.421] (II) Loading /usr/lib/xorg/modules/input/evdev_drv.so
[  7221.421] (II) Module evdev: vendor="X.Org Foundation"
[  7221.421]    compiled for 1.19.1, module version = 2.10.5
[  7221.421]    Module class: X.Org XInput Driver
[  7221.421]    ABI class: X.Org XInput driver, version 24.1
[  7221.421] (II) Using input driver 'evdev' for 'SYNA2393:00 06CB:19AC'
```

---

-   evdev

执行命令 `xinput set-prop 11 Evdev Axis Calibration 2 3021 -7 2015` 测试效果

---

-   libinput

使用上面的 `click 0` 和 `click 3` 的值以及 `xrandr` 得到的 `screen width/height` 计算，得到的值如下：

```shell
$ python3 ./touchscreen_calibration_convert.py 344 193 382 246 376 1761
Try set 'libinput Calibration Matrix' to '-43.0, 0.0, 47.9, 0.0, 0.1, 0.0, 0.0, 0.0, 1.0'
```

执行命令 `xinput set-prop 11 libinput Calibration Matrix -43.0 0.0 47.9 0.0 0.1 0.0 0.0 0.0 1.0` 测试效果。


### 额外问题 {#额外问题}

某些设备按照上述步骤设置后仍然有一些问题，这里列出一些遇到过的，一旦出现下面这些情况，请先切换驱动到 `evdev` 然后进行设置。


#### 上下相反 {#上下相反}

如点击 ****左上角**** 却反馈在 ****左下角**** ，在 `90-touchscreen-calibrator.conf` 中加上 `Option InvertY "true"` 。


#### 左右相反 {#左右相反}

如点击 ****左上角**** 却反馈在 ****右下角**** ，在 `90-touchscreen-calibrator.conf` 中加上 `Option InvertX "true"` 。


#### 边缘偏移 {#边缘偏移}

如点击最右边却反馈在旁边的区域，此时可调整 `Option Calibration` 的值，其值的含有为： `"min-x max-x min-y max-y"` 。在四个边缘都试下，然后调整对应的值。


#### `XY` 相反 {#xy-相反}

如点击 ****左上角**** 却反馈在 ****右上角**** ，在 `90-touchscreen-calibrator.conf` 中加上 `Option SwapAxes "true"` 。


## 持久化 {#持久化}

上面的测试无误后，就开始集成到系统中，每次启动时都自动执行。

自动执行通过 `xorg.conf.d` 钩子执行，添加 `90-touchscreen-calibrator.conf` 到 `/etc/X11/xorg.conf.d/` 目录中。


### evdev {#evdev}

示例内容如下：

```shell
Section "InputClass"
        Identifier	"calibration"
        MatchProduct	"SYNA2393:00 06CB:19AC"
        Option	"Calibration"	"2 3021 -7 2015"
        Option	"SwapAxes"	"0"
EndSection
```


### libinput {#libinput}

示例内容如下：

```shell
Section "InputClass"
        Identifier	"calibration"
        MatchProduct	"SYNA2393:00 06CB:19AC"
        Option	"CalibrationMatrix"	"-43.0 0.0 47.9 0.0 0.1 0.0 0.0 0.0 1.0"
EndSection
```


## 设备映射 {#设备映射}

若存在多块屏幕，这时就需要指定触摸屏对应的设备，使用 `xinput --map-output-to <device id> <output name>` 。

`device name` 就是上文中得到的 `id` ， `output name` 可通过  `xrandr` 命令得到。


### 持久化 {#持久化}

可添加 `90-touchscreen-map` 到 `/etc/X11/xinit/xinitrc.d` 目录，内容就是上面的命令。


## 登录界面校正 {#登录界面校正}

目前 `deepin` 使用 `lightdm` 做为登录管理器，所以需要修改 `lightdm` 的配置文件来设置校正数据。

修改 `/etc/lightdm/lightdm.conf` 文件中 `[Seat:*]` 组的 `display-setup-script` 字段，值为指定的校正脚本路径(需要先取消注释)。

脚本文件里只能使用命令，上面的校正过程中用到的正确的 `xinput` 命令要写入这个文件，包括设备映射的命令。

如在华为上就只需映射设备，所以添加文件 `/etc/lightdm/display_setup.sh` ，内容如下：

```shell
#!/bin/bash

xinput --map-output-to "SYNA2393:00 06CB:19AC" eDP-1
```

而 `/etc/lightdm/lightdm.conf` 则修改为(只给出修改的部分)：

```shell
[Seat:*]
display-setup-script=/etc/lightdm/display_setup.sh
```


## 参考文档 {#参考文档}

-   `man 4 evdev` : 需要安装 `xserver-xorg-input-evdev`
-   `man 4 libinput` : 需要安装 `xserver-xorg-input-libinput`
