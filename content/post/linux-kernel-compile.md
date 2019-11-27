+++
title = "Compile Kernel"
date = 2019-08-06T21:38:21+08:00
lastmod = 2019-11-27T20:05:41+08:00
tags = ["kernel", "compile", "linux"]
categories = ["BLOG"]
draft = false
+++

最近由于要调试 `usbhid` 模块，所以需要编译 `kernel` ，在此整理下方法，以免每次都查找。


## Dependencies {#dependencies}

-   `deepin`

    `sudo apt-get install build-essential libncurses-dev bison flex libssl-dev libelf-dev`

-   `archlinux`

    `sudo pacman -S gcc ncurses bison flex openssl libelf elfutils`

若编译时有报错，请按照错误提示按照缺失的依赖。

<!--more-->

---


## Configuration {#configuration}

默认情况下使用当前内核的配置，操作如下：

-   `deepin`

    ``cp /boot/config-`uname -r` .config``

-   `archlinux`

    `zcat /proc/config.gz > .config`

如需自定义，执行 `make nconfig` 修改。

---


### Version {#version}

编译时需要指定版本，不然版本不同模块会加载失败，版本指定的方法如下：


#### vermagic {#vermagic}

内核编译时会设置 `vermagic` ，如果模块的 `vermagic` 不一致则会加载失败，使用 `modinfo <module>` 可查看当前值。

对于 `vermagic` 的设置，目前只找到了通过变量设置的方法，如下：

有 `vermagic` 为 `4.19.0-5-amd64` ，可通过变量 `VERSION, PATCHLEVEL, SUBLEVEL and EXTRAVERSION` 来指定它，详细如下：

-   VERSION : 对应的是 `4`
-   PATCHLEVEL : 对应的是 `19`
-   SUBLEVEL : 对应的是 `0`
-   EXTRAVERSION : 对应的是 `-5-amd64`

其中 `EXTRAVERSION` 还可通过 `.config` 中 `CONFIG_LOCALVERSION` 指定(需要确保 `CONFIG_LOCALVERSION_AUTO` 不被设置)

或者这样指定：

```shell
make KERNELRELEASE=4.19.34-2deepin-generic ARCH=arm64   KBUILD_BUILD_VERSION=20 KBUILD_SRC=
```


#### Avoid add '+' after kernel version {#avoid-add-plus-after-kernel-version}

> To prevent the script scripts/setlocalversion to append the + to the end of the kernel local version, create an empty .scmversion file in the root of the kernel sources.
>
> $ touch .scmversion
>
> this way, you'll be able to leave LOCALVERSION as is in the kernel configuration file, in case you want to append a local signature to the kernel name.

---


## Build {#build}

The kernel version: **4.19.37** , vermagic: **4.19.0-5-amd64**


### Prepare {#prepare}

`make VERSION=4 PATCHLEVEL=19 SUBLEVEL=0 EXTRAVERSION='-5-amd64' scripts prepare modules_prepare -j 24`


### Make {#make}

`make VERSION=4 PATCHLEVEL=19 SUBLEVEL=0 EXTRAVERSION='-5-amd64' -j 24`


### Build Special Module {#build-special-module}


#### Prepare {#prepare}

`make VERSION=4 PATCHLEVEL=19 SUBLEVEL=0 EXTRAVERSION='-5-amd64' scripts modules_prepare -j 24`


#### Make {#make}

`make VERSION=4 PATCHLEVEL=19 SUBLEVEL=0 EXTRAVERSION='-5-amd64' -C . M=<module path>`

例如编译 `usbhid.ko` 则是： `make VERSION=4 PATCHLEVEL=19 SUBLEVEL=0 EXTRAVERSION='-5-amd64' -C . M=drivers/hid/usbhid`

---


## Installation {#installation}


### Modules {#modules}

`sudo make modules_install`

可通过 `INSTALL_MOD_PATH` 指定 `modules` 安装目录

然后执行以下操作：

```shell
sudo depmod # 更新 modules 映射
sudo update-initramfs  -u # deepin 下更新当前内核，使用 '-k' 指定内核
sudo mkinitcpio -p <preset> # archlinux 下更新指定内核， 'preset' 在 '/etc/mkinitcpio.d' 中查看
```

---


### Image {#image}

`sudo make install`

目的是生成下面的文件：

```shell
initramfs-4.19.0-5-amd64.img
System.map-4.19.0-5-amd64
vmlinuz-4.19.0-5-amd64
```

如下：

```shell
$ sudo make install -j 4
sh ./arch/x86/boot/install.sh 4.19.0-5-amd64 arch/x86/boot/bzImage \
        System.map "/boot"
run-parts: executing /etc/kernel/postinst.d/apt-auto-removal 4.19.0-5-amd64 /boot/vmlinuz-4.19.0-5-amd64
run-parts: executing /etc/kernel/postinst.d/dkms 4.19.0-5-amd64 /boot/vmlinuz-4.19.0-5-amd64
run-parts: executing /etc/kernel/postinst.d/initramfs-tools 4.19.0-5-amd64 /boot/vmlinuz-4.19.0-5-amd64
update-initramfs: Generating /boot/initrd.img-4.19.0-5-amd64
cryptsetup: WARNING: The initramfs image may not contain cryptsetup binaries
    nor crypto modules. If that's on purpose, you may want to uninstall the
    'cryptsetup-initramfs' package in order to disable the cryptsetup initramfs
    integration and avoid this warning.
setupcon is missing. Please install the 'console-setup' package.
W: plymouth: The plugin label.so is missing, the selected theme might not work as expected.
W: plymouth: You might want to install the plymouth-themes package to fix this.
I: The initramfs will attempt to resume from /dev/sda1
I: (UUID=0d78d207-61eb-4e99-b7d8-a2b2cb2515d6)
I: Set the RESUME variable to override this.
run-parts: executing /etc/kernel/postinst.d/reconfigure-dde-daemon 4.19.0-5-amd64 /boot/vmlinuz-4.19.0-5-amd64
fix systemd/logind config
run-parts: executing /etc/kernel/postinst.d/vboxadd 4.19.0-5-amd64 /boot/vmlinuz-4.19.0-5-amd64
run-parts: executing /etc/kernel/postinst.d/zz-update-grub 4.19.0-5-amd64 /boot/vmlinuz-4.19.0-5-amd64
Generating grub configuration file ...
Found theme: /boot/grub/themes/deepin-fallback/theme.txt
Found linux image: /boot/vmlinuz-4.19.0-5-amd64
Found initrd image: /boot/initrd.img-4.19.0-5-amd64
Found linux image: /boot/vmlinuz-4.19.0-5-amd64.old
Found initrd image: /boot/initrd.img-4.19.0-5-amd64
done
```

---


## 参考文档 {#参考文档}

-   [Traditional\_compilation](https://wiki.archlinux.org/index.php/Kernel/Traditional%5Fcompilation)
-   [Compile kernel module](https://wiki.archlinux.org/index.php/Compile%5Fkernel%5Fmodule)
-   [How to compile and install Linux Kernel 5.1.14 from source code](https://www.cyberciti.biz/tips/compiling-linux-kernel-26.html)
-   [How to Compile Just One Kernel Module](https://yoursunny.com/t/2018/one-kernel-module/)
-   [Installation of Linux API Headers](http://www.linuxfromscratch.org/lfs/view/development/chapter06/linux-headers.html)
-   [Building modules for a precompiled kernel](https://linux.die.net/lkmpg/x380.html)
-   [Compile the Linux Kernel from sources](https://www.acmesystems.it/compile%5Flinux%5Fkernel)
