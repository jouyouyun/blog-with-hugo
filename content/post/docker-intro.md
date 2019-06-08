+++
title = "Docker 使用简介"
author = "jouyouyun <jouyouwen717@gmail.com>"
date = 2019-04-21T11:58:00+08:00
lastmod = 2019-06-08T18:29:53+08:00
tags = ["docker", "container", "image", "compose"]
categories = ["BLOG"]
draft = false
+++

`Docker` 是使用 `GoLang` 开发的开源容器引擎，可以方便的打包开发好的应用，然后分发到任意 `linux` 主机上。

与传统的虚拟机相比拥有以下优势：

-   高效的系统资源利用率

    由于不需要进行硬件虚拟和运行完整的操作系统等额外开销，无论是应用执行速度、内存损耗或者文件存储速度， `Docker` 都更加高效

-   更快的启动速度

    `Docker` 容器应用直接运行与宿主内核，无需启动完整的操作系统，可以做到秒级启动

<!--more-->

-   一致的运行环境

    `Docker` 镜像提供了除内核外的完整运行环境，确保了应用运行环境的一致性

-   持续交付和部署

    可以通过 `Docker` 镜像来实现服务的持续交付、部署。使用 `Dockerfile` 来构建镜像，使用持续集成系统进行集成测试；使用镜像结合持续部署系统进行自动部署

-   迁移轻松

    只需要迁移镜像及镜像运行的数据就可在其他主机或平台运行

-   易于维护和扩展

    由于使用镜像进行部署，使维护更为容易。由于支持在镜像的基础上进行定制，使得扩展变得更简单。而官方也维护了一大批高质量的[镜像](https://hub.docker.com)，大大降低了镜像的制作成本


## 基本概念 {#基本概念}


### 仓库 {#仓库}

`Docker` 提供了仓库(`Repository`)用于存放制作好的镜像，方便使用者获取，在本地可通知配置多个 `Repository` 。

-   拉取

    可以使用命令来拉取镜像:

    `docker pull [repo url>/]image name>`

    默认的 `repo url` 是 `hub.docker.com` ，拉取默认仓库中的镜像时是不需要 `url` 的。
    如拉取 `debian` ： `docker pull debian` 。

-   推送

    我们也可将自己制作好的镜像推送到仓库，以便分发，使用命令：

    `docker push [<repo url>/]<image name>[:<image tag>>`

-   搜索

    使用 `docker search` 命令则可搜索默认 `repo url` 内的镜像。

-   镜像加速

    由于默认 `repo url` 在国外，为了加快拉取速度，需要指定其为国内的，向 `/etc/docker/daemon.json` 中添加:

    ```json
    {
      "registry-mirrors": ["https://registry.docker-cn.com"]
    }
    ```

    便可使用 `Docker` 在中国的镜像加速站。

    对于企业用户还可搭建自己的私有镜像仓库，方便管理内部镜像。

-   登录

    `docker` 在使用公共镜像时可直接 `pull` ，无需登录。但在使用私有镜像时则需要向登录，使用命令 `docker login <repository url>` 。

    `repository url` 不指定则使用默认值，默认是 `hub.docker.com` 。


### 镜像 {#镜像}

镜像是经过 `Dockerfile` 规则打包生成的， `Dockerfile` 文件了指明了镜像的生成及运行方式。


#### Dockerfile {#dockerfile}

这里简单介绍下 `Dockerfile` 中常用的命令:

-   FROM

    指明基于那个镜像，通常是一个系统环境，如 `debian` 。在多阶段(`stage`)构建中可使用 `FROM ... AS ...` 来命令阶段，供后面的阶段使用

-   ENV

    设置镜像的环境变量

-   EXPOSE

    导出端口，语法： `EXPOSE port/protocol`

-   RUN

    执行一条命令

-   COPY

    复制镜像内部的文件，也可从指定 `stage` 中复制文件，使用 `--from=<stage name>` 来实现

-   ADD

    复制本地文件到镜像中，并做了一些额外工作，如自动解压文件、保持文件属性等

-   WORKDIR

    切换工作目录

-   CMD

    执行命令，极容易在 `docker run` 时被覆盖。主要用于指定镜像的默认启动命令，有以下三种语法：

    -   `CMD ["executable", "param1", "param2"]` `Exec` 模式，首先
    -   `CMD ["param1", "param2"]` 设置 `ENTRYPOINT` 的默认参数
    -   `CMD command param1 param2` `Shell` 模式

-   ENTRYPOINT

    指定镜像默认的运行命令，可以使用 `--entrypoint` 参数在 `docker run` 时覆盖 `Dockerfile` 内的 `ENTRYPOINT`

    有 `Exec` 和 `Shell` 两种模式

    通常使用这个命令来指定镜像的启动命令，因为 `CMD` 稍不注意就被覆盖了

更多关于 `Dockerfile` 内容请参见： [Dockerfile reference](https://docs.docker.com/engine/reference/builder/)

这里给出一个简单的 `Dockerfile=， 是 =debian sid` 的：

```dockerfile
FROM scratch
ADD rootfs.tar.xz /
CMD ["bash"]
```


#### 创建 {#创建}

使用 `docker build -f <Dockerfile path> [-t <image name>[:<tag>]] .` 创建镜像和标记 `Tag`

`Tag` 可用来标记不同的镜像版本，就像版本号。


#### 列出 {#列出}

使用 `docker image ls` 可以列出下载下来的镜像


#### 运行 {#运行}

使用 `docker run` 命令运行镜像，可使用参数配置，下面给出一个例子：

`docker run -it -d -p <local port>:<container port> -v <local path>:<container path> -e "TESST_ENV=1" --name "<conatiner name>" <image>`

-   `-i` 是交互式操作
-   `-t` 是终端
-   `-p` 指定端口映射
-   `-v` 指定 `volume` 映射
-   `-d` 后台模式
-   `-e` 指定环境变量
-   `--name` 指定运行后的容器名称

\*\*注意：\*\*这些参数必须在 `image` 之前指定，否则会被当作 `image` 启动命令的参数。


#### 删除 {#删除}

使用 `docker image rm <image id>` 删除镜像


#### 导入和导出 {#导入和导出}

-   导出： `docker save <image id> > <image save>.tar`
-   导入： `docker load < <image save>.tar`


### 容器 {#容器}

容器是镜像运行后的实体，运行与属于自己的独立的命名空间。


#### 创建 {#创建}

使用 `docker run` 命令启动镜像就可以创建一个容器


#### 列出 {#列出}

使用 `docker ps` 可列出正在运行的容器，加上 `-a` 参数可列出所以存在的容器


#### 控制 {#控制}

-   启动

    使用 `docker container start <containe id>` 启动容器

-   终止

    使用 `docker container stop <containe id>` 终止容器

-   暂停

    `dcoker container pause <containe id>`

-   重启

    `dcoker container restart <containe id>`

-   日志

    `dcoker container logs <containe id>`


#### 删除 {#删除}

`docker container rm <container id>` 命令可删除容器


#### 进入容器 {#进入容器}

使用 `docker exec -it <container id> /bin/sh` 命令可进入容器的执行环境


#### 导入和导出 {#导入和导出}

-   导入： `docker import <container export>.tar <container name>`
-   导出: `docker export <container id> > <container export>.tar`


## 部署 {#部署}

直接运行镜像不便于部署服务，因而官方推出了 `Docker Compose` 来负责应用部署。

使用 `docker-compose.yml` 文件将应用需要的镜像都定义在一起，然后使用 `docker-compose` 命令来部署。

`docker-compose.yml` 的语法详见： [Compose file version 3 reference](https://docs.docker.com/compose/compose-file/)，这里给出一份部署 `wordpress` 的文件：

```yaml
version: "3.7"

services:
  wordpress:
    image: wordpress
    ports:
      - "8080:80"
    networks:
      - backend

  mysql:
    image: mysql
    volumes:
       - db-data:/var/lib/mysql/data
    networks:
       - backend

volumes:
  db-data:

networks:
  backend:
```


### 常用命令 {#常用命令}

-   up

    启动服务，加上 `-d` 表示在后端运行

-   down

    终止服务

-   build

    构建服务中的容器

-   config

    验证文件格式是否有误

-   exec

    进入指定的容器，如进入上面 `wordpress` 中的 `mysql` 则是： `docker-compose exec mysql`

-   logs

    显示日志，可指定容器

-   restart

    重启服务

更多命令请见： [Overview of docker-compose CLI](https://docs.docker.com/compose/reference/overview/)
