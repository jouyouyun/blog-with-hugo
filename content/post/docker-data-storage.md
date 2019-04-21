+++
title = "更改 Docker 数据的存储目录"
date = 2019-04-21T12:58:00+08:00
lastmod = 2019-04-21T13:46:19+08:00
tags = ["docker", "storage"]
categories = ["BLOG"]
draft = false
+++

`Docker` 安装后默认将数据存储在 `/var/lib/docker` 下，随着 `image` 和 `container` 的增多，会导致 `/` 分区的空间被用完，如果 `/` 分区特别大就请忽略这篇文章。

因此需要将一些软件的数据存储到空间大的分区，与 `/` 分区分开。不仅 `Docker` 需要这样处理，如 `mysql/postgresql` 等软件也需要这样处理。

<!--more-->


## 处理方案 {#处理方案}


### 将 `/var` 单独分区 {#将-var-单独分区}

配置分区时为 `/var` 单独分区，然后在挂载到 `/` 上(在 `/etc/fstab` 中配置)，这样就可以简单的解决上面的问题了。


### 重新绑定 `Docker` 的数据目录 {#重新绑定-docker-的数据目录}

这个是在数据分区新建一个目录， 如 `/Data/var/docker` ，然后将 `/var/lib/docker` 绑定或软链到这个目录，实现将数据从 `/` 分区中转移出来。

至于数据分区则是一个空间较大的分区，主要用来存放用户数据的。如我喜欢单独分出一个很大的区，将其挂载到 `/Data` 上，
然后将 `$HOME` 目录中的 `Downloads, Documents, Public, Pictures` 等目录都软链到 `/Data/DE` 下，这样每次重装系统时只需保证 `/Data` 分区不被格式化就能保证数据不丢失了。

软链的方式就不做描述了，下面给出绑定 `/var/lib/docker` 到 `/Data/var/docker` 的步骤：

1.  创建目标目录

    `mkdir -p /Data/var/`

2.  转移数据

    ```shell
    sudo systemctl stop docker.service
    sudo rsync -avPHSX /var/lib/docker /Data/var/ # -H 处理Hardlink为Hardlink, 必须加上
    sudo rm -rf /var/lib/docker
    sudo mkdir /var/lib/docker
    ```

3.  目录绑定

    添加 `/Data/var/docker /var/lib/docker none bind 0 0` 到 `/etc/fstab` 中
