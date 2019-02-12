+++
title = "怎么处理 'Origin'值从'Google, Inc.'修改到了'Google LLC' 的 APT 错误"
date = 2018-11-07T21:38:21+08:00
lastmod = 2019-02-12T14:34:23+08:00
tags = ["google", "apt", "origin"]
categories = ["BLOG"]
draft = false
+++

今天在执行 `sudo apt-get update` 出现了下面的错误提示：

```shell
E: 仓库'http://dl.google.com/linux/chrome/deb stable Release'将其'Origin'值从'Google, Inc.'修改到了'Google LLC'
N: 为了让这个仓库能够应用，这必须在更新之前显式接受。更多细节请参阅 apt-secure(8) 手册。

# 英语环境下的输出
E: Repository ‘http://dl.google.com/linux/chrome-remote-desktop/deb stable Release’ changed its ‘Origin’ value from ‘Google, Inc.’ to ‘Google LLC’
N: This must be accepted explicitly before updates for this repository can be applied. See apt-secure(8) manpage for details.
```

这个错误导致无法升级系统，错误说明了原因：是由于 `Google Chrome` 的 `Origin` 改变引起的，也给出了解决思路：手动接受这个改变。

<!--more-->

所以修复这个错误就很容易了，只需执行：

```shell
sudo apt update
```

执行后会看到下面的提示：

```shell
E: 仓库'http://dl.google.com/linux/chrome/deb stable Release'将其'Origin'值从'Google, Inc.'修改到了'Google LLC'
N: 为了让这个仓库能够应用，这必须在更新之前显式接受。更多细节请参阅 apt-secure(8) 手册。
您是否愿意接受这些更改，并且继续从此仓库更新？ [y/N] y

# 英语环境下的输出
E: Repository ‘http://dl.google.com/linux/chrome-remote-desktop/deb stable Release’ changed its ‘Origin’ value from ‘Google, Inc.’ to ‘Google LLC’
N: This must be accepted explicitly before updates for this repository can be applied. See apt-secure(8) manpage for details.
Do you want to accept these changes and continue updating from this repository? [y/N] y
```

此时输入 `y` 接受就可解决问题。
