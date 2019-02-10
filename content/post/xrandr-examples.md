+++
title = "XRandr Examples"
author = ["Wen"]
date = 2016-06-06T16:52:30+08:00
lastmod = 2019-02-09T22:27:35+08:00
tags = ["xrandr"]
categories = ["NOTE"]
draft = false
+++

由于经常会用到使用 `xrandr` 设置屏幕属性, 而每次用的时候都要去查 `man` 手册, 有时还需要上网搜索例子.
这样一来就比较麻烦, 所以就想整理下 `xrandr` 的用法, 给出一些常用的例子.


## 查询 {#查询}

使用 `xrandr -q` 命令可以查询当前的显示器状态, 使用 `xrandr --verbose` 将会显示更详细的信息.


## 设置分辨率 {#设置分辨率}

设置分辨率时需要指定设置的 `ouput` 以及 `mode`, 如将 `eDP1` 的分辨率改为 `1920x1080`, 命令就是 `xrandr --output eDP1 --mode 1920x1080`.

但是单屏时不用这么麻烦, 可以直接使用命令 `xrandr -s 1920x1080` 来更改.


## 添加分辨率 {#添加分辨率}

如果 `xrandr -q` 查询的结果中没有合适的分辨率, 则可以通过 `newmode` 来添加一个, 下面将给出为 `eDP1` 添加 `1440x900x60` 的分辨率的方法.

1.  使用 `cvt` 生成一个 `modeline`, 命令如下:

```shell
->cvt 1440 900 60
# 1440x900 59.89 Hz (CVT 1.30MA) hsync: 55.93 kHz; pclk: 106.50 MHz
Modeline "1440x900_60.00"  106.50  1440 1528 1672 1904  900 903 909 934 -hsync +vsync
```

1.  使用 `newmode` 创建一个 `mode`, 参数就是上面 `Modeline` 后的内容, 命令如下:

```shell
->xrandr --newmode "1440x900_60.00"  106.50  1440 1528 1672 1904  900 903 909 934 -hsync +vsync
```

1.  将上面的 `mode` 添加到 `eDP1` 中:

```shell
->xrandr --addmode eDP1 1440x900_60.00
```

如果执行出错, 则说明不支持此模式. 如果没有错误则添加完成, 然后就可以设置此 `mode` 为分辨率了.


## 双屏配置 {#双屏配置}

如现在有两个 `output`, 分别是 `eDP1` 和 `VGA1`.


### 设置主屏 {#设置主屏}

通过指定 `--primary` 参数来设置主屏, 如设置 `eDP1` 为主屏: `xrandr --auto --output eDP1 --primary`, `--auto` 可以自动启用关闭的屏幕.


### 复制模式 {#复制模式}

复制模式最好使用两个显示器都有的 `mode` 作为默认的 `mode`, 命令是 `xrandr --auto --output eDP1 --pos 0x0 --mode 1440x900 --output VGA1 --same-as eDP1`.


### 扩展模式 {#扩展模式}

命令是 `xrandr --auto --output eDP1 --pos 0x0 --mode 1920x1080 --primary --output VGA1 --mode 1440x900 --right-of eDP1`.

命令的结果就是 `VGA1` 会在 `eDP1` 的右边, `eDP1` 为主屏, 另外位置的参数还有 `left-of`, `--above`, `--below` 等.

如果需要自定义两个屏幕的位置, 可以通过计算每个屏幕的分辨率, 用 `--pos` 参数来指定每个屏幕显示的位置.


### 单屏模式 {#单屏模式}

这个模式是只显示某一个屏幕, 如只显示 `eDP1`, 可以使用命令 `xrandr --output eDP1 --pos 0x0 --mode 1920x1080 --primary --output VGA1 --off`,
这样就会把 `VGA1` 给关闭.


### 其它 {#其它}

另外屏幕的旋转, 镜像和缩放可以分别使用 `--rotate`, `reflect` 和 `--scale` 参数来实现.

至于更详细的信息请使用 `man xrandr` 来查看.
