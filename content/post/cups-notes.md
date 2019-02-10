+++
title = "CUPS 资料"
date = 2018-04-08T21:38:21+08:00
lastmod = 2019-02-10T18:21:51+08:00
tags = ["cups"]
categories = ["NOTE"]
draft = false
+++

## Installation {#installation}

`sudo apt install cups ghostscript gsfonts`


## Driver {#driver}

-   gutenprint

    一组质量非常好的驱动集合，支持的目标机型包括 `Canon, Epson, Lexmark, Sony, Olympus` ；以及配合 `CUPS/GhostSscript/Foomatic/GIMP` 使用的 `PCL printers` 。

    安装包: `cups-driver-gutenprint gimp-gutenprint ijsgutenprint printer-driver-gutenprint`

-   foomatic

    `Foomatic` 是一个基于数据库的，集成自由软件打印机驱动和脱机打印程序的系统。

    安装包: `foomatic-db foomatic-db-engine foomatic-db-compressed-ppds foomatic-db-gutenprint foomatic-filters` 以及 `foomatic-db-nonfree`

-   foo2zjs

    `Drivers for ZjStream protocol printers such as the HP Laserjet 1018.`

    安装包: `printer-driver-foo2zjs`

-   hplip

    `HP GNU/Linux` 驱动. 支持 `DeskJet, OfficeJet, Photosmart, Business Inkjet` 和一些 `LaserJet printer` 型的, 以及一些兄弟打印机。

    安装包: `hplip hplip-gui hplip-plugin`

-   splix

    三星驱动，支持 `SPL` 打印机(`SPL：Samsung Printer Language`)  (`USB` 打印机要配合 `cups-usblp` 使用)

    安装包: `printer-driver-splix splix samsung-printers samsung-print firmware-samsung`

-   cndrvcups

    佳能 `UFR2` 驱动，支持 `LBP, iR` 和 `MF` 系列打印机.

    安装包: `cndrvcups-capt cndrvcups-ufr2-uk cndrvcups-ufr2lt-uk cndrvcups-utility`

-   cups-pdf

    `PDF` 虚拟打印机，这个东西可以把发送给他的打印任务输出为 `PDF` 文件。

这些驱动可以全部预装。

有些打印机需要 `PPD` 文件，上面的驱动会自带一部分，如果没有就需要去查找了：

-   到源里查找，包括其他发行版，如 [aur](https://wiki.archlinux.org/index.php/AUR) 中找
-   到 [OpenPrinting database](http://www.openprinting.org/printers) 选择制造商和型号
-   到制造商官网查找

---


## Configuration {#configuration}

配置文件在 `/etc/cups` 这个目录，为了方便排错，建议把 `/etc/cups/cupsd.conf` 中 `LogLevel` 设置为 `debug2`.


### USB 打印机 {#usb-打印机}

使用 `lsusb` 查看设备。

需要加载模块 `usbcore`, 有的打印机还需要加载 `usblp` 而有的需要把它屏蔽


### 并口打印机 {#并口打印机}

使用 `sudo dmesg | grep -i parport` 查看设备

需要加载模块 `lp parport parport_pc`

有些使用 `usb to parallel` 的打印机可能无法识别，需要使用不同的连接方式，并将 `DeviceID` 写入到文件 `/etc/cups/printers.conf=，如 =DeviceID = parallel:/dev/usb/lp0`

---


## Links {#links}

-   [Arch Wiki CUPS](https://wiki.archlinux.org/index.php/CUPS)
-   [Troubleshooting](https://wiki.archlinux.org/index.php/CUPS/Troubleshooting)
-   [Arch Printer Driver](https://wiki.archlinux.org/index.php/CUPS/Printer-specific%5Fproblems)
-   [Deepin Wiki CUPS](https://wiki.deepin.org/index.php?title=%E6%89%93%E5%8D%B0%E6%9C%BA)
