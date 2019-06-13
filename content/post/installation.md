+++
title = "系统安装二三事"
date = 2019-05-02T11:27:00+08:00
lastmod = 2019-06-13T20:32:57+08:00
tags = ["installation"]
categories = ["NOTE"]
draft = false
+++

在重装系统时, 需要考虑软件与数据的备份恢复, 遂记录下需要用到的软件, 自定义的配置和数据恢复.


## 软件 {#软件}


### 换源 {#换源}

-   `Archlinux`

    在 `/etc/pacman.d/mirrorlist` 只启用中国区的源, 另添加以下内容到 `/etc/pacman.conf` 中来启用 `yaourt aur` 源:

    ```shell
    [archlinuxcn]
    SigLevel = Optional TrustAll
    Server = https://mirrors.ustc.edu.cn/archlinuxcn/$arch~
    ```

-   `Deepin`

    这里使用 `163` 的源

    ```shell
    http://mirrors.163.com/deepin stable main contrib non-free
    ```

    具体的 `distribution`, `componments` 则根据安装后默认的源来填写.


### 输入法 {#输入法}

-   `Archlinux`

    ```shell
    yaourt -S fcitx fcitx-googlepinyin fcitx-frontend-gtk2 fcitx-frontend-gtk3 fcitx-frontend-qt4 fcitx-module-x11 fcitx-module-dbus fcitx-table fcitx-ui-classic
    ```

-   `Deepin`

    ```shell
    sudo apt-get install fcitx-googlepinyin
    # remove sogoupinyin
    sudo apt-get purge sogoupinyin fcitx-sogoupinyin-uk
    ```


### 常用工具 {#常用工具}

-   `Archlinux`

    ```shell
    yaourt -S zsh tmux gvim openssh most firefox albert parcellite the_silver_searcher wget xelatex wkhtmltopdf pandoc vlc zhcon terminator git emacs w3m curl \
    dia inkscape wireshark wps-office aria2 mpv htop gpicview deepin-screenshot foxitreader python-pip
    ```

-   `Deepin`

    ```shell
    sudo apt-get install zsh tmux vim-gtk3 openssh-server parcellite silversearcher-ag texlive-xetex wkhtmltopdf pandoc vlc zhcon terminator git emacs w3m \
    curl dia inkscape wireshark wps-office aria2 mpv htop gpicview deepin-screenshot foxitreader python-pip python3-pip
    ```

    `firefox, albert` 另外安装


### 开发 {#开发}

-   `Archlinux`

    ```shell
    yaourt -S valgrind ctags go hugo boost boost-libs nodejs d-feet devhelp cmake clang clang-format astyle
    ```

-   `Deepin`

    ```shell
    sudo apt-get install valgrind exuberant-ctags golang-go golang-src hugo libboost-all-dev nodejs-bin d-feet devhelp cmake clang clang-format libclang-dev astyle \
    libpython-dev
    sudo apt-get build-dep dde-daemon startdde dde-api
    ```


## 配置 {#配置}

-   `nodjs`

    设置 `npm` 加速镜像源与全局目录:

    ```shell
    # ~/.npmrc
    registry = http://registry.cnpmjs.org
    prefix=/Data/npm-global
    ```

-   `pip`

    设置 `pip` 加速镜像源:

    ```shell
    # ~/.pip/pip.conf
    [global]
    index-url = https://pypi.douban.com/simple
    ```

-   `docker`

    设置 `pip` 加速镜像源:

    ```shell
    # /etc/docker/daemon.json
    {
      "registry-mirrors": ["https://registry.docker-cn.com"]
    }
    ```

    存储目录的设置参见: [更改 Docker 数据的存储目录](http://jouyouyun.github.io/post/docker-data-storage/)

-   `hosts`

    之前用到的设置，重装之前备份好。

-   `sysctl.conf`

    文件最大打开数目，=tcp= 相关的配置

-   `coredump`

    安装 `systemd-coredump` 并在 `/etc/sysctl.conf` 中添加 `coredump` 的配置。

-   `/etc/systemd/`

    此目录下的配置文件的修改需要记录，如 `logind.conf` 中 `lid` 相关动作的处理方法。

-   项目的配置文件

    统一将各个项目的配置文件放在 `/Data/Projects/Configurations` 下面，重装前备份。

-   `IO` 调度算法

    系统一般所在的硬盘识别符为 `sda` ，双硬盘的可能是其它名称，使用 `df -h` 确定。
    `SSD` 硬盘可以使用 `noop` 调度算法。

    -   查看当前值： `cat /sys/block/sda/queue/scheduler`
        -   `noop` : `FIFO` 队列类型，使用于 `IO` 无压力的硬盘，如 `SSD` ，闪存设备， `RAM` 等
        -   `deadline` ：为每次请求都设置截至时间，读写分离在不同的队列中，使用于大吞吐量的场景，如数据库环境
        -   `cfq` : 默认值，完全公平队列，为每个进程都创建队列，然后使用时间片轮转处理进程的队列。是前两种的折中方案，使用于多媒体应用，桌面系统等。

    -   修改： `echo noop | sudo tee /sys/block/sda/queue/scheduler`

    -   持久化： 添加 `elevator=noop` 到 `/etc/default/grub` 中的 `GRUB_CMDLINE_LINUX` 中，然后使用 `sudo grub-mkconfig -o /boot/grub/grub.cfg` 更新启动项。

-   其它

    `vim, tmux, git, ssh, ss-tproxy` 的配置在 [conf.d](https://gitee.com/jouyouyun/conf.d) 中, `emacs` 的配置在 [prelude](https://github.com/jouyouyun/prelude) 中.

    `ss-tproxy` 执行 `git clone https://github.com/zfl9/ss-tproxy.git` 下载，需要安装 `ipset, chinadns, dnsmasq` 等依赖。


## 数据 {#数据}

由于将数据都单独保存在分区 `/Data` 中, 所以重装系统时只要不将其格式化就好, 装好后在修改 `/etc/fstab` 挂载上去, 如:

```shell
UUID=6f8f77fb-f8ca-43cb-bb40-4e8e7ca8e009   /Data           ext4        rw,relatime,data=ordered    0 1
```
