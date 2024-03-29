+++
title = "Linux 硬件信息获取"
date = 2019-05-08T17:50:00+08:00
lastmod = 2019-06-16T20:30:25+08:00
tags = ["hardware", "cpu", "disk", "network", "wireless", "wired", "memory", "dmi", "bios", "board", "lsblk", "lspci", "lsusb", "lshw", "dmidecode"]
categories = ["BLOG"]
draft = false
+++

在 `linux` 上可以通过 `dmidecode` 或是 `lshw` 来获取硬件信息，能够方便的查看系统配置。但它们的输出信息过多，解析起来有些麻烦，另外 `lshw` 对 `usb` 接口的网卡支持不好，显示的信息不够，所以在此整理下通过读文件或是一些简单命令来获取硬件信息的方法。


### **DMI** {#dmi}

一般情况下内核默认加载了 `dmi sysfs` ，路径是 `/sys/class/dmi` 。里面包含了 `bios` ， `board` ， `product` 等信息。

<!--more-->


#### **Bios** {#bios}

通过命令 `ls -l /sys/class/dmi/id/bios_*` 可以看到支持的 `bios` 字段，如下：

```shell
$ ls -l /sys/class/dmi/id/bios_*
-r--r--r-- 1 root root 4.0K 5月   8 17:18 /sys/class/dmi/id/bios_date
-r--r--r-- 1 root root 4.0K 5月   8 17:18 /sys/class/dmi/id/bios_vendor
-r--r--r-- 1 root root 4.0K 5月   8 17:18 /sys/class/dmi/id/bios_version
```

直接读文件即可获取对应值。


#### **Board** {#board}

通过命令 `ls -l /sys/class/dmi/id/board_*` 可以看到支持的 `board` 字段，如下：

```shell
$ ls -l /sys/class/dmi/id/board_*
-r--r--r-- 1 root root 4.0K 5月   8 08:45 /sys/class/dmi/id/board_asset_tag
-r--r--r-- 1 root root 4.0K 5月   8 08:45 /sys/class/dmi/id/board_name
-r-------- 1 root root 4.0K 5月   8 08:45 /sys/class/dmi/id/board_serial
-r--r--r-- 1 root root 4.0K 5月   8 08:43 /sys/class/dmi/id/board_vendor
-r--r--r-- 1 root root 4.0K 5月   8 08:45 /sys/class/dmi/id/board_version
```

直接读文件即可获取对应值，但有些文件需要 `root` 权限。


#### **Product** {#product}

通过命令 `ls -l /sys/class/dmi/id/product_*` 可以看到支持的 `product` 字段，如下：

```shell
$ ls -l /sys/class/dmi/id/product_*
-r--r--r-- 1 root root 4.0K 5月   8 08:45 /sys/class/dmi/id/product_family
-r--r--r-- 1 root root 4.0K 5月   8 08:43 /sys/class/dmi/id/product_name
-r-------- 1 root root 4.0K 5月   8 08:45 /sys/class/dmi/id/product_serial
-r--r--r-- 1 root root 4.0K 5月   8 17:12 /sys/class/dmi/id/product_sku
-r-------- 1 root root 4.0K 5月   8 08:45 /sys/class/dmi/id/product_uuid
-r--r--r-- 1 root root 4.0K 5月   8 08:45 /sys/class/dmi/id/product_version
```

直接读文件即可获取对应值，但有些文件需要 `root` 权限。

其中 `product_uuid` 可作为机器的唯一 `ID` 。


### **CPU** {#cpu}

通过读取文件 `/proc/cpuinfo` 可获取 `cpu` 的信息，一般 `model name` 字段为 `cpu` 名称，如：

```shell
$ cat /proc/cpuinfo|grep 'model name'
model name      : Intel(R) Core(TM) i7-3520M CPU @ 2.90GHz
model name      : Intel(R) Core(TM) i7-3520M CPU @ 2.90GHz
model name      : Intel(R) Core(TM) i7-3520M CPU @ 2.90GHz
model name      : Intel(R) Core(TM) i7-3520M CPU @ 2.90GHz
```

**但在龙芯，申威上可能不是这个字段，需要根据文件内容确定。**


### **Memory** {#memory}

通过读取文件 `/proc/meminfo` 可获取内存总大小，字段是 `MemTotal` ，如：

```shell
$ cat /proc/meminfo |grep MemTotal
MemTotal:        7860064 kB
```

**对于内存厂商等信息还未找到获取方法，待以后补全。**


### **Disk** {#disk}

硬盘信息这里使用 `lsblk` 来获取，通过指定它的参数来获取，如：

```shell
$ lsblk -J -bno NAME,SERIAL,TYPE,SIZE,VENDOR,MODEL,MOUNTPOINT,UUID
{
   "blockdevices": [
      {"name": "sda", "serial": "TF0500WE0GAV0V", "type": "disk", "size": "500107862016", "vendor": "ATA     ", "model": "HGST HTS725050A7", "mountpoint": null,
         "children": [
            {"name": "sda1", "serial": null, "type": "part", "size": "4294967296", "vendor": null, "model": null, "mountpoint": "/boot"},
            {"name": "sda2", "serial": null, "type": "part", "size": "4294967296", "vendor": null, "model": null, "mountpoint": "[SWAP]"},
            {"name": "sda3", "serial": null, "type": "part", "size": "1024", "vendor": null, "model": null, "mountpoint": null},
            {"name": "sda5", "serial": null, "type": "part", "size": "107374182400", "vendor": null, "model": null, "mountpoint": "/Data"},
            {"name": "sda6", "serial": null, "type": "part", "size": "64424509440", "vendor": null, "model": null, "mountpoint": "/"}
         ]
      }
   ]
}
```

参数的含义通过 `lsblk -h` 命令查看。

**只有 `type` 为 `disk` 时才表示为一块硬盘，其它如 `loop` 则应该过滤掉。** 每块硬盘中的 `children` 表示它下面的分区，通过 `mountpoint` 可确定硬盘在此系统上的使用情况。


### **Network** {#network}

这里是先获取系统上的网络接口，这包括了物理网卡和虚拟网卡(如 `docker` 创建的)。

~~所以要先过滤，过滤顺序如下：~~

~~1. 按名字过滤~~

~~过滤掉 `lo`~~

~~2. 按驱动过滤~~

~~过滤掉驱动为 `dummy, veth, vboxnet, vmnet, vmxnet, vmxnet2, vmxnet3` 的网卡, 虚拟机中的~~

~~3. 按网卡类型过滤~~

~~过滤掉 `bridge` 类型的网卡~~

如果网卡接口同时存在于 `/sys/class/net/` 和 `/sys/devices/virtual/net/` 中，则需要过滤掉。

接下来分别给出网卡信息获取的方法：


#### **Interface Name** {#interface-name}

即是上面的目录下的子目录名


#### **Mac Address** {#mac-address}

读取文件 `/sys/class/net/<iface name>/address` 可得到


#### **IP** {#ip}

通过调用 `ioctl` 来获取指定 `iface name` 的 `ip` ，代码大致如下：

```shell
char* get_ip_for_iface(char *iface)
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        fprintf(stderr, "open socket failed: %s", strerror(errno));
        return;
    }

    // must init ifr
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);

    char *c_addr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
    char *ip = calloc(strlen(c_addr)+1, sizeof(char));
    memcpy(ip, c_addr, strlen(c_addr));

    return ip;
}
```

`ipv6` 的暂未测试。


#### **Model** {#model}

网卡一般在 `pci` 接口上，但也有些实在 `usb` 接口上，要分别获取。

不过都要先读取文件 `/sys/class/net/enp0s25/device/uevent` ，然后分别处理。

-   **pci**

    `uevent` 内容如：

    ```shell
    DRIVER=e1000e
    PCI_CLASS=20000
    PCI_ID=8086:1502
    PCI_SUBSYS_ID=17AA:21F3
    PCI_SLOT_NAME=0000:00:19.0
    MODALIAS=pci:v00008086d00001502sv000017AAsd000021F3bc02sc00i00
    ```

    取到其中的 `PCI_SLOT_NAME` ，然后执行 `lspci -k -s <PCI_SLOT_NAME>` 来获取 `model` 信息，如：

    ```shell
    $ lspci -k -s 0000:00:19.0
    00:19.0 Ethernet controller: Intel Corporation 82579LM Gigabit Network Connection (Lewisville) (rev 04)
            Subsystem: Lenovo 82579LM Gigabit Network Connection
            Kernel driver in use: e1000e
            Kernel modules: e1000e
    ```

    其中 `Subsystem` 之后的即是 `model` 信息。

-   **usb**

    `uevent` 内容如：

    ```shell
    DEVTYPE=usb_interface
    DRIVER=ath9k_htc
    PRODUCT=cf3/9271/108
    TYPE=255/255/255
    INTERFACE=255/0/0
    MODALIAS=usb:v0CF3p9271d0108dcFFdscFFdpFFicFFisc00ip00in00
    ```

    取到其中的 `PRODUCT` ，然后将 `/` 替换为 `:` ，然后执行 `lsusb -d <product>` 来获取 `model` 信息，如：

    ```shell
    # 可以不要最后的 '108'
    $ lsusb -d cf3:9271:108
    Bus 001 Device 007: ID 0cf3:9271 Atheros Communications, Inc. AR9271 802.11n
    ```

    其中 `Subsystem` 之后的即是 `model` 信息。


### **Bluetooth** {#bluetooth}

在 `/sys/class/bluetooth/` 下是蓝牙设备，与 **网卡** 一样，根据 `/sys/class/bluetooth/<hciX>/device/uevent` 的内容使用 `lspci` 或 `lsusb` 来获取 `model` 信息。

如：

```shell
$ cat /sys/class/bluetooth/hci0/device/uevent
DEVTYPE=usb_interface
DRIVER=btusb
PRODUCT=a5c/21e6/112
TYPE=255/1/1
INTERFACE=255/1/1
MODALIAS=usb:v0A5Cp21E6d0112dcFFdsc01dp01icFFisc01ip01in00
```

这就是一个 `usb` 接口的设备，所以使用 `lsusb` 来获取 `model` 信息，如：

```shell
$ lsusb -d a5c:21e6:112
Bus 001 Device 003: ID 0a5c:21e6 Broadcom Corp. BCM20702 Bluetooth 4.0 [ThinkPad]
```


### **Graphic** {#graphic}

显卡信息在 `/sys/class/drm/` 下，里面还包含了显卡支持输出接口，但只有 `card+integer` 组成的目录才是显卡的，如本机的信息：

```shell
$ ls /sys/class/drm/
card0@  card0-DP-1@  card0-DP-2@  card0-DP-3@  card0-HDMI-A-1@  card0-HDMI-A-2@  card0-HDMI-A-3@  card0-LVDS-1@  card0-VGA-1@  renderD128@  version
```

根据输出可知只有一块显卡 `card0` ，通过读取文件 `card0/device/uevent` 获取设备类型，然后同 **网卡** 一样查询 `model` 信息，如：

```shell
$ cat /sys/class/drm/card0/device/uevent
DRIVER=i915
PCI_CLASS=30000
PCI_ID=8086:0166
PCI_SUBSYS_ID=17AA:21FA
PCI_SLOT_NAME=0000:00:02.0
MODALIAS=pci:v00008086d00000166sv000017AAsd000021FAbc03sc00i00

$ lspci -k -s 0000:00:02.0
00:02.0 VGA compatible controller: Intel Corporation 3rd Gen Core processor Graphics Controller (rev 09)
        Subsystem: Lenovo 3rd Gen Core processor Graphics Controller
        Kernel driver in use: i915
        Kernel modules: i915
```

另外 `/sys/class/hwmon/` 和 `/sys/class/graphics/` 下有当前使用中的显卡设备，也是对应子目录下的 `device/uevent` 来获取信息。

若无 `device` 目录或是 `device/uevent` 中的内容既没有 `pci` 信息也没有 `usb` 信息，则过滤掉，有就如下所示获取：

**`hwmon`**

```shell
$ cat /sys/class/hwmon/hwmon2/device/uevent
DRIVER=nouveau
PCI_CLASS=30000
PCI_ID=10DE:0A75
PCI_SUBSYS_ID=17AA:3957
PCI_SLOT_NAME=0000:02:00.0
MODALIAS=pci:v000010DEd00000A75sv000017AAsd00003957bc03sc00i00

$ lspci -k -s 0000:02:00.0
02:00.0 VGA compatible controller: NVIDIA Corporation GT218M [GeForce 310M] (rev a2)
        Subsystem: Lenovo GT218M [GeForce 310M]
        Kernel driver in use: nouveau
        Kernel modules: nouveau
```

**`graphics`**

```shell
$ cat /sys/class/graphics/fb0/device/uevent
DRIVER=i915
PCI_CLASS=30000
PCI_ID=8086:0166
PCI_SUBSYS_ID=17AA:21FA
PCI_SLOT_NAME=0000:00:02.0
MODALIAS=pci:v00008086d00000166sv000017AAsd000021FAbc03sc00i00

$ lspci -k -s 0000:00:02.0
00:02.0 VGA compatible controller: Intel Corporation 3rd Gen Core processor Graphics Controller (rev 09)
        Subsystem: Lenovo 3rd Gen Core processor Graphics Controller
        Kernel driver in use: i915
        Kernel modules: i915
```


#### Display Monitor {#display-monitor}

显示器的信息目前是从 `edid` 中获取，先确定显示器连接的显卡端口，然后使用 `edid-decode` (需要安装)解析其的 `edid` 文件，就可得到详细信息。如本机是 `card0-LVDS-1` ：

```shell
$ cat /sys/class/drm/card0-LVDS-1/edid|edid-decode
Extracted contents:
header:          00 ff ff ff ff ff ff 00
serial number:   06 af 6c 10 00 00 00 00 00 14
version:         01 04
basic params:    90 1c 10 78 02
chroma info:     20 e5 92 55 54 92 28 25 50 54
established:     00 00 00
standard:        01 01 01 01 01 01 01 01 01 01 01 01 01 01 01 01
descriptor 1:    12 1b 56 58 50 00 19 30 30 20 36 00 15 9c 10 00 00 18
descriptor 2:    00 00 00 0f 00 00 00 00 00 00 00 00 00 00 00 00 00 20
descriptor 3:    00 00 00 fe 00 41 55 4f 0a 20 20 20 20 20 20 20 20 20
descriptor 4:    00 00 00 fe 00 42 31 32 35 58 57 30 31 20 56 30 20 0a
extensions:      00
checksum:        ec

Manufacturer: AUO Model 106c Serial Number 0
Made week 0 of 2010
EDID version: 1.4
Digital display
6 bits per primary color channel
Digital interface is not defined
Maximum image size: 28 cm x 16 cm
Gamma: 2.20
Supported color formats: RGB 4:4:4
First detailed timing is preferred timing
Established timings supported:
Standard timings supported:
Detailed mode: Clock 69.300 MHz, 277 mm x 156 mm
               1366 1414 1446 1454 hborder 0
                768  771  777  793 vborder 0
               -hsync -vsync
Manufacturer-specified data, tag 15
ASCII string: AUO
ASCII string: B125XW01
Checksum: 0xec (valid)
EDID block does NOT conform to EDID 1.3!
        Missing name descriptor
        Missing monitor ranges
        Detailed block string not properly terminated
```


### Sound {#sound}

声卡设备在 `/sys/class/sound` 目录下，目录名一般是 `card+integer` 组成，如本机的信息：

```shell
$ ls /sys/class/sound/
card0@  controlC0@  hwC0D0@  hwC0D3@  pcmC0D0c@  pcmC0D0p@  pcmC0D3p@  pcmC0D7p@  pcmC0D8p@  timer@
```

就只有一块声卡 `card0` ，通过读取文件 `card0/device/uevent` 获取设备类型，然后同 **网卡** 一样查询 `model` 信息，如：

```shell
$ cat /sys/class/sound/card0/device/uevent
DRIVER=snd_hda_intel
PCI_CLASS=40300
PCI_ID=8086:1E20
PCI_SUBSYS_ID=17AA:21FA
PCI_SLOT_NAME=0000:00:1b.0
MODALIAS=pci:v00008086d00001E20sv000017AAsd000021FAbc04sc03i00

$ lspci -k -s 0000:00:1b.0
00:1b.0 Audio device: Intel Corporation 7 Series/C216 Chipset Family High Definition Audio Controller (rev 04)
        Subsystem: Lenovo 7 Series/C216 Chipset Family High Definition Audio Controller
        Kernel driver in use: snd_hda_intel
        Kernel modules: snd_hda_intel
```


### **Input/Output Device** {#input-output-device}

输入设备的信息可以从 `/proc/bus/input/devices` 文件中获取，如：

```shell
I: Bus=0019 Vendor=0000 Product=0005 Version=0000
N: Name="Lid Switch"
P: Phys=PNP0C0D/button/input0
S: Sysfs=/devices/LNXSYSTM:00/LNXSYBUS:00/PNP0C0D:00/input/input0
U: Uniq=
H: Handlers=event0
B: PROP=0
B: EV=21
B: SW=1

I: Bus=0011 Vendor=0001 Product=0001 Version=ab54
N: Name="AT Translated Set 2 keyboard"
P: Phys=isa0060/serio0/input0
S: Sysfs=/devices/platform/i8042/serio0/input/input3
U: Uniq=
H: Handlers=sysrq kbd event3 leds
B: PROP=0
B: EV=120013
B: KEY=10000 0 0 0 1000402000000 3803078f800d001 feffffdfffefffff fffffffffffffffe
B: MSC=10
B: LED=7

I: Bus=0011 Vendor=0002 Product=0007 Version=01b1
N: Name="SynPS/2 Synaptics TouchPad"
P: Phys=isa0060/serio1/input0
S: Sysfs=/devices/platform/i8042/serio1/input/input5
U: Uniq=
H: Handlers=mouse0 event5
B: PROP=5
B: EV=b
B: KEY=e520 10000 0 0 0 0
B: ABS=660800011000003

...
```

由于内容太多，这里就只显示部分内容。

另外也可通过 `xinput` 命令获取，如：

```shell
$ xinput
⎡ Virtual core pointer                          id=2    [master pointer  (3)]
⎜   ↳ Virtual core XTEST pointer                id=4    [slave  pointer  (2)]
⎜   ↳ SynPS/2 Synaptics TouchPad                id=11   [slave  pointer  (2)]
⎜   ↳ TPPS/2 IBM TrackPoint                     id=12   [slave  pointer  (2)]
⎣ Virtual core keyboard                         id=3    [master keyboard (2)]
    ↳ Virtual core XTEST keyboard               id=5    [slave  keyboard (3)]
    ↳ Power Button                              id=6    [slave  keyboard (3)]
    ↳ Video Bus                                 id=7    [slave  keyboard (3)]
    ↳ Sleep Button                              id=8    [slave  keyboard (3)]
    ↳ Integrated Camera: Integrated C           id=9    [slave  keyboard (3)]
    ↳ AT Translated Set 2 keyboard              id=10   [slave  keyboard (3)]
    ↳ ThinkPad Extra Buttons                    id=13   [slave  keyboard (3)]
```

使用 `xinput list-prop <device id>` 可以查看设备的属性。


### **Battery** {#battery}

电池信息可以从 `/sys/class/power_supply/<name>/uevent` 文件中获取，电池的名称一般以 `BAT` 开头。如本机的信息：

```shell
$ cat /sys/class/power_supply/BAT0/uevent
POWER_SUPPLY_NAME=BAT0
POWER_SUPPLY_STATUS=Full
POWER_SUPPLY_PRESENT=1
POWER_SUPPLY_TECHNOLOGY=Li-ion
POWER_SUPPLY_CYCLE_COUNT=0
POWER_SUPPLY_VOLTAGE_MIN_DESIGN=11100000
POWER_SUPPLY_VOLTAGE_NOW=12226000
POWER_SUPPLY_POWER_NOW=0
POWER_SUPPLY_ENERGY_FULL_DESIGN=57720000
POWER_SUPPLY_ENERGY_FULL=48000000
POWER_SUPPLY_ENERGY_NOW=48000000
POWER_SUPPLY_CAPACITY=100
POWER_SUPPLY_CAPACITY_LEVEL=Full
POWER_SUPPLY_MODEL_NAME=45N1023
POWER_SUPPLY_MANUFACTURER=SANYO
POWER_SUPPLY_SERIAL_NUMBER=15921
```


### Backlight {#backlight}

`/sys/class/backlight/` 目录下的是背光设备，如显示屏，背光键盘等，可以更改文件内容来调节这些设备的亮度。如：

```shell
$ ls /sys/class/backlight/intel_backlight/
actual_brightness  bl_power  brightness  device@  max_brightness  power/  subsystem@  type  uevent
```

-   **brightness**

    更改这个文件可以修改此设备的当前亮度

-   **max\_brightness**

    这个文件显示的是此设备支持的最大亮度

另外背光设备 `device` 可能只想真实的显卡设备，一般是子目录中包含 `video` 的。


### Camera {#camera}

`/sys/class/video4linux/` 下是摄像头设备，不同子目录中的设备可能是同一个，也是读取 `device/uevent` 文件来选择 `lspci` 或 `lsusb` 获取设备信息，如：

```shell
$ cat /sys/class/video4linux/video0/device/uevent
DEVTYPE=usb_interface
DRIVER=uvcvideo
PRODUCT=5986/2d2/11
TYPE=239/2/1
INTERFACE=14/1/0
MODALIAS=usb:v5986p02D2d0011dcEFdsc02dp01ic0Eisc01ip00in00

$ lsusb -d 5986:2d2:11
Bus 001 Device 004: ID 5986:02d2 Acer, Inc
```


### Printer {#printer}

打印机应该是在 `/sys/class/printer` 下，信息获取方法应该与上文一致，本人手中没有打印机就不给出示例了。


### Fingerprint {#fingerprint}

指纹的功能目前是由 `libfprint` 项目提供，调用其提供的接口来获取。

如使用 `qdbus` 来获取：

```shell
$ qdbus --system --literal net.reactivated.Fprint /net/reactivated/Fprint/Manager net.reactivated.Fprint.Manager.GetDevices
[Argument: ao {}]
```

输出可知本机没有指纹设备。


## 实现 {#实现}

这里用 `Go` 实现了 `hardware` ，见此： [hardware](https://github.com/jouyouyun/hardware)
