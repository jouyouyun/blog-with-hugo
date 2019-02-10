+++
title = "使用 hugo 写博客"
author = ["Wen"]
date = 2019-02-02T17:31:21+08:00
lastmod = 2019-02-09T21:39:30+08:00
tags = ["hugo"]
categories = ["BLOG"]
draft = false
+++

[hugo](https://github.com/gohugoio/hugo) 是由 `Go` 语言实现的静态网站生成器, 简单、易用、高效、易扩展、快速部署。


## 安装 {#安装}

到 [Release](https://github.com/gohugoio/hugo/releases) 下载对应系统的二进制文件压缩包, 然后解压, 并把得到的二进制复制到 `Path` 中.


## 简单使用 {#简单使用}


### 创建一个站点 {#创建一个站点}

使用命令 `hugo new site <dir>` 来创建站点, 创建后的结构如下:

```shell
.
├── archetypes
│   └── default.md
├── config.toml
├── content
├── data
├── layouts
├── static
└── themes
```

各目录的简单说明如下, 具体请见 [Directory Structure Explained](https://gohugo.io/getting-started/directory-structure/) :

-   **archetypes:** 预定义的文章模板
-   **config.toml:** 站点的配置文件
-   **content:** 文章存放的目录
-   **data:** 生成 `website` 时用到的配置文件目录
-   **layouts:** 页面 `html` 的模板目录
-   **static:** 存放静态文件, 如 `css`, `js` 和图片等
-   **themes:** 主题存放目录


### 配置主题 {#配置主题}

主题可以直接复制到 `themes` 目录下, 也可以通过 `git submodule` 的方式放在 `themes`, 这里使用后者, 因为便于管理.

这里使用 `even` 主题, 安装如下:

```shell
git submodule add https://github.com/olOwOlo/hugo-theme-even.git themes/even
```

然后将 `themes/even/config.toml` 替换掉站点的.

子模块的一些常用操作:

```shell
# 添加子模块
git submodule add https://github.com/username/gitname.git gitname
# 递归克隆带有子模块的仓库
git clone https://github.com/username/yourblog.git yourblog --recursive
# 上面递归克隆的命令等同于下面几行代码
git clone https://github.com/username/yourblog.git yourblog
git submodule init    # 初始化子模块
git submodule update  # 更新子模块
# 此处要注意，如果你需要对子模块做出更改，需要创建或检出一个分支，Git默认不在任何分支上
# 删除子模块
# 逆初始化模块，其中{MOD_NAME}为模块目录，执行后可发现模块目录被清空
git submodule deinit {MOD_NAME}
# 删除.gitmodules中记录的模块信息（--cached选项清除.git/modules中的缓存）
git rm --cached {MOD_NAME}
# 提交更改到代码库，可观察到'.gitmodules'内容发生变更
git commit -am "Remove a submodule."
# 修改某子模块的远程库(url)
# 首先手动修改'.gitmodules'文件中对应模块的”url“属性
# 更新子模块的 url
git submodule sync
# 提交变更
git commit -am "Update submodule url."
```


### 配置 {#配置}

编辑 `config.toml` 文件来配置, 具体看文件里的注释.


### 新建文章 {#新建文章}

`hugo new post/<filename.md>` 将会在 `content/post/` 创建文件 `filename.md`, 然后编辑这个文件来写文章.

文章写完后去掉文件中的 `draft=true`, 不然生成到 `public` 中.


### 预览 {#预览}

执行 `hugo server` 就可以按照给定的地址去看刚写的文章了.


### 生成 {#生成}

使用 `hugo` 命令生成站点到 `public`, 然后将 `public` 发布.


## 自动发布到 `Github Page` {#自动发布到-github-page}

在 `github` 创建一个以 `<username>.github.io` 命令的仓库, 然后将 `public` 中的文件 `push` 到刚才的 `repo` 中.
