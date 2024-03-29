+++
title = "Go 包管理之gom"
date = 2018-11-27T21:38:21+08:00
lastmod = 2019-02-12T14:33:42+08:00
tags = ["go", "module", "package", "gom"]
categories = ["NOTE"]
draft = false
+++

## Go Module 使用 {#go-module-使用}

`Go Module` 是官方用来管理 `package` 依赖的工具, `1.11` 新加的, 使用时需要设置变量: `GO111MODULE=on` .

`go mod help` 可以查看其帮助文档:

<!--more-->

```shell
$ go help mod
Go mod provides access to operations on modules.

Note that support for modules is built into all the go commands,
not just 'go mod'. For example, day-to-day adding, removing, upgrading,
and downgrading of dependencies should be done using 'go get'.
See 'go help modules' for an overview of module functionality.

Usage:

        go mod <command> [arguments]

The commands are:

        download    download modules to local cache
        edit        edit go.mod from tools or scripts
        graph       print module requirement graph
        init        initialize new module in current directory
        tidy        add missing and remove unused modules
        vendor      make vendored copy of dependencies
        verify      verify dependencies have expected content
        why         explain why packages or modules are needed

Use "go help mod <command>" for more information about a command.
```

下面来说明下如何使用.

---


### 示例 {#示例}

`go mod` 要求项目必须在 `GOPATH` 中, 所以在这个例子中设置为当前目录(`~/GoLang`) `export GOPATH=$PWD`, 然后在 `src` 下创建项目 `test` , 目录结构如下:

```shell
GoLang
└── src
    └── test
        └── main.go
```

示例的代码请见: [GoModule](https://github.com/jouyouyun/examples/tree/master/GoModule)


#### 初始化 {#初始化}

进入 `test` 所在目录, 执行 `go mod init` 即可完成初始化, 会多出一个文件: `go.mod` .


#### 记录依赖 {#记录依赖}

执行 `go get ./` 即会开始查找依赖并下载记录到 `go.mod` 文件中.

**`go mod` 会将依赖下载到 `$GOPATH/pkg` 下, 用来当作缓存, 可以在多个项目之间共享.**


#### 依赖变更及 vendor {#依赖变更及-vendor}

如在项目的开发过程中, 依赖有变更, 可使用 `go mod tidy` 来应用这些变更到 `go.mod` 文件.

在项目发布时会要将依赖复制到项目中, 此时使用 `go mod vendor` 即可完成此操作.

---

到此就介绍完了 `go mod` 的用法, 其他的子命令用法请查看帮助文档.

最后在说下需要注意的地方:

1.  **项目必须要在 `GOPATH` 中**
2.  **需要设置 `GO111MODULE=on` 变量**
