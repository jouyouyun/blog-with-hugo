+++
title = "DDE 后端 Tutorial"
date = 2020-03-15T11:38:21+08:00
lastmod = 2021-04-25T12:57:18+08:00
tags = ["go", "dde", "daemon", "audio", "network", "power", "bluetooth"]
categories = ["NOTE"]
draft = false
+++

`dde` 后端使用 `go` 作为主要的开发语言，使用 `dbus` 提供接口，主要使用 `gsettings` 来保存配置。所以在进行后端开发前需要对以上内容有基本的了解，这里假定本文档的阅读者熟悉 `dbus` 和 `gsettings` ，并有一定的开发经验。

这里就只对 `go` 进行简单的介绍。


## go {#go}

`go` 是一种类 `c` 的语言，但没有了 `c` 中令人头疼的指针和内存管理问题，极大的提高了开发效率。
`go` 中使用 `package` 来封装同一功能的模块，首字母大写的变量和函数是可以被其它 `package` 使用的。

学习 `go` 至少需要了解以下内容。


### 编码风格 {#编码风格}

`go` 提供了 `gofmt` 工具来对代码进行格式化，以使用统一的代码风格。因此在代码保存之前需要使用 `gofmt` 先进行格式化。

保存前格式化这种操作，一般可在编辑器中配置，如 `vim` , `emacs` 等，根据个人习惯选择编辑器。


### 变量 {#变量}

`go` 支持使用时声明和使用 `var` 声明两种方式，如下：


#### 使用 `var` 声明 {#使用-var-声明}

```go
// 主动声明
var (
        outData []byte
        err error
)

outData, err = os.Exec("ls").Output()
```


#### 使用时声明 {#使用时声明}

```go
// 使用时声明
outData, err := os.Exec("ls").Output()
```

变量名采用驼峰命名法，首字母小写。


### 输入/输出 {#输入-输出}

`go` 中的输入/输出函数跟 `c` 中的类似，在 `fmt` 中，具体的请阅读 `package fmt` 中的代码。


### 流程控制 {#流程控制}


#### if-else {#if-else}

同样是与 `c` 的类似，如：

```go
var num = 0
if num < 0 {
        fmt.Println("num < 0")
} else if num > 0 {
        fmt.Println("num > 0")
} else {
        fmt.Println("num == 0")
}
```


#### for {#for}

同样是与 `c` 的类似，如：

```go
for i := 0; i < 10; i++ {
        fmt.Println("Index:", i)
}
```


#### switch {#switch}

`go` 中的 `switch` 比 `c` 中的强大，不向 `c` 中只能是整型，并且每个 `case` 语句后不需要 `break` 语句，示例如下：

```go
var num = 0
switch num {
case 1:
        fmt.Println("Num is 1")
case 2:
        fmt.Println("Num is 2")
default:
        fmt.Println("Num is invalid")
}

var str = "hello"
switch str {
case "hello":
        fmt.Println("String is hello")
case "test":
        fmt.Println("String is test")
}
```


#### select {#select}

同样是与 `c` 的类似，如：

```go
var quit = make(chan struct{})
var ticker = time.NewTicker(time.Second * 3)
select {
case <-ticker.C:
         fmt.Println("Ticker recieved")
case <-quit:
         fmt.Println("Quit...")
}
```

---

另外 `go` 中不支持 `while` 语句，但支持 `break` 和 `goto` 语句。


### 函数 {#函数}

`go` 中的函数定义如下：

```shell
func 函数名(形式参数列表)(返回值列表){
    函数体
}
```

`go` 中的函数支持多返回值，这一点是与 `c` 中不同的。

其他的就与 `c` 相同。


### 异常处理 {#异常处理}

`go` 中可用 `panic` 函数让程序崩溃，也可使用 `recover` 函数捕获此次崩溃信息，让程序继续执行.


### 协程 {#协程}

`go` 中并发使用协程处理，使用 `go` 关键词即可开启一个协程，如：

```go
// 开启10个协程
for i:= 0; i < 10; i++ {
        go func(idx int) {
                fmt.Println("In index:", i)
        }(i)
}
```


### package {#package}

`go` 提供了丰富的标准库，至少需要熟悉以下 `package` ：

-   `fmt`
-   `flag`
-   `errors`
-   `strings`
-   `strconv`
-   `io`
-   `io/ioutil`
-   `bytes`
-   `bufio`
-   `path`
-   `path/filepath`
-   `os`
-   `os/exec`
-   `sort`
-   `time`

---

---

关于 `go` 的内容就先介绍这些，后续希望能够自己去学习，官网提供的有文档。

接下来开始介绍后端的内容。


## DDE 后端 {#dde-后端}

首先介绍后端包含的项目：

-   [deepin-desktop-schemas](https://gitlab.deepin.io/dde-v20/deepin-desktop-schemas)

    存储后端配置，使用 `gsettings` 存储

-   [go-gir-generator](https://gitlab.deepin.io/github-linuxdeepin-mirror/go-gir-generator)

    对 `glib2.0` 的 `go` 绑定， `gsettings` 的接口就是其提供的

-   [go-lib](https://gitlab.deepin.io/dde-v20/go-lib)

    后端封装的常用功能 `package` ，要求能够熟练使用里面的每个 `package` ，例如 `dbus` 服务是由 `dbus1` 和 `dbusutils` 两个 `package` 提供的。

    使用时需将 `/usr/share/gocode` 加入到 `GOPATH` 变量中，然后导入：

    ```go
     import (
             "pkg.deepin.io/lib/dbus1"
              "pkg.deepin.io/lib/dbus1"
    )
    ```

-   [go-x11-client](https://gitlab.deepin.io/dde-v20/go-x11-client)

    对 `xcb` 接口的绑定，涉及 `x` 的操作需要了解

-   [go-dbus-factory](https://gitlab.deepin.io/dde-v20/go-dbus-factory)

    后端对常用的 `dbus` 服务的封装，可直接方便的调用对应 `dbus` 服务的接口。

-   [dde-api](https://gitlab.deepin.io/dde-v20/dde-api/)

    对桌面级模块的封装，并一共一些非常驻内存的程序

-   [dde-daemon](https://gitlab.deepin.io/dde-v20/dde-daemon)

    常驻内存的服务，主要分为系统级服务和用户级服务，即 `dde-system-daemon` 和 `dde-session-daemon`

-   [startdde](https://gitlab.deepin.io/dde-v20/startdde/)

    桌面的启动程序，启动整个桌面环境，按照指定的顺序启动桌面组件，并提供显示接口

上面是对后端所涉及的项目做了简单的介绍，下面会重点介绍一些关键的。


### startdde {#startdde}

做后桌面的启动程序，必须保证稳定和尽可能的简洁，所以不是启动阶段需要的功能都不要放入此项目中。此项目包含的功能如下：


#### 启动显示服务 {#启动显示服务}

启动显示服务，为桌面提供显示相关接口，主要用于分辨率、刷新率和多屏设置


#### xsettings {#xsettings}

启动 `xsettings manager` 用于主题、字体和 `dpi` 等的设置


#### 窗管启动 {#窗管启动}

通过检查硬件环境或用户的配置来决定启动特效模式或是普通模式的窗管


#### 桌面组建启动 {#桌面组建启动}

提供了配置文件 `/usr/share/startdde/auto_launch.json` 来定义桌面组建的启动顺序，对于需要在 `autostart` 之前启动的程序需要加入这个配置文件中。


#### autostart {#autostart}

扫描系统的 `autostart` 目录下的 `desktop` 文件，并将其启动


#### 应用启动接口 {#应用启动接口}

提供应用启动的接口， `dde-dock, dde-launcher, dde-desktop` 会使用


#### watchdog {#watchdog}

启动 `watchdog` 服务，进听重要的桌面组件是否存在，如果不存在就将其启动

---

以上就是 `startdde` 提供的主要功能，具体实现请自行阅读代码。


### dde-daemon {#dde-daemon}

`dde-daemon` 中主要分为 `dde-system-daemon` 和 `dde-session-daemon` 两个服务，将分别介绍。


#### dde-system-daemon {#dde-system-daemon}

本服务主要提供系统级服务，即需要 `root` 权限的和与用户无关的接口，包含的主要功能如下：

<!--list-separator-->

-  accounts

    提供用户管理的功能和接口，对应控制中心的账户模块。

<!--list-separator-->

-  apps

    提供应用信息启动次数，系统中应用变更监听的功能和接口， `dde-dock` 和 `dde-launcher` 会用到。

<!--list-separator-->

-  fprintd

    对 `fprintd` 接口的封装，提供开源的指纹接口，计划废弃

<!--list-separator-->

-  image\_effect

    提供图片模糊的功能和接口

<!--list-separator-->

-  gesture

    使用 `libinput` 的接口监听触摸板和触摸屏的事件，发送触摸板手势信号和触摸板长按信号。

<!--list-separator-->

-  network

    提供一些需要 `root` 权限操作的网络接口

<!--list-separator-->

-  power

    提供电池的信息的功能和接口，以及一些通用的电源接口

<!--list-separator-->

-  timedated

    时间日期管理的服务和接口

<!--list-separator-->

-  airplane\_mode

    飞行模式的接口


#### dde-session-daemon {#dde-session-daemon}

用户级的服务接口，主要供控制中心使用，包含的功能如下：

<!--list-separator-->

-  dock

    主要为 `dde-dock` 提供所需接口

<!--list-separator-->

-  trayicon

    托盘应用的管理接口

<!--list-separator-->

-  x\_event\_monitor

    提供统一的按键、鼠标移动和鼠标点击事件的监听程序

<!--list-separator-->

-  network

    提供当前网络功能的接口，包括设备数量、连接状态、无线接入点等

    网络服务的实际功能由 `NetworkManager` 提供

<!--list-separator-->

-  audio

    提供声音相关的接口，被 `dde-dock` 和  `dde-control-center` 使用

    声音功能实际由 `pulseaudio` 提供

<!--list-separator-->

-  screensaver

    基于 `org.freedesktop.Screensaver` 标准实现的接口，主要提供 `x` 中 `idle` 超时的设置，供其他模块使用

<!--list-separator-->

-  sessionwatcher

    监听当前 `session` 的改变，对变为非激活的 `session` 时禁用一些功能

<!--list-separator-->

-  power

    为 `dde-dock` 和 `dde-control-center` 提供接口，实现电源设置里待机、黑屏等功能

    电源信息由 `upower` 提供

<!--list-separator-->

-  launcher

    为 `dde-launcher` 提供接口，包括应用列表、应用排序、应用卸载等功能

<!--list-separator-->

-  keybinding

    桌面全局快捷键绑定的功能，以及快捷键更改、自定义快捷键等功能

<!--list-separator-->

-  appearance

    主题、字体和缩放等功能的接口

<!--list-separator-->

-  inputdevices

    输入输出设备的配置接口，如鼠标、键盘、触摸板等设备的配置

    设备的配置是通过配置 `xorg` 上提供的设备属性来完成的，也可使用 `xinput` 配置

<!--list-separator-->

-  gesture

    接受系统级 `gesture` 服务的信号，做出对应的处理

<!--list-separator-->

-  housekeeping

    检查 `home` 目录空间是否充足，不足时提示用户

<!--list-separator-->

-  timedate

    时间日期设置的接口

    实际设置接口由 `systemd-timedatectl` 提供

<!--list-separator-->

-  bluetooth

    蓝牙服务的接口，封装的是  `bluez` 的接口

<!--list-separator-->

-  systeminfo

    系统信息的接口，如 `cpu` 、内存、存储、版本等

<!--list-separator-->

-  calltrace

    生成调试所需 `calltrace` 的动态接口，用于调试内存泄漏和 `cpu` 占用

<!--list-separator-->

-  debug

    动态开启 `debug` 日志的接口

    ---

    到此基本完成对 `dde-daemon` 的介绍，想要详细了解还是需要去看代码。


## 调试方法 {#调试方法}

后端的所有项目都要求使用 `pkg.deepin.io/lib/log` 这个 `package` 提供日志功能，所以都可以通过 `DDE_DEBUG_LEVEL` 这个环境变量来修改日志级别，如改为 `debug` 级别：

```shell
export DDE_DEBUG_LEVEL="debug"
# 然后在启动程序
```

也可动态的开启 `debug` 日志，如：

-   `startdde`

    `qdbus --literal com.deepin.SessionManager /com/deepin/SessionManager com.deepin.SessionManager.ToggleDebug`

-   `dde-session-daemon`

    `gsettings set com.deepin.dde.daemon debug true`

    启用 `debug` 模块，如果不再需要，需要手动关闭，即设为  `false`

关于 `dde-daemon` 更多调试相关的，请参见 `docs/dde-session-daemon_debug.md` 文件
