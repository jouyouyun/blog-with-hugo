+++
title = "RESTfull 简介"
date = 2018-05-10T21:38:21+08:00
lastmod = 2019-02-12T14:36:05+08:00
tags = ["restfull"]
categories = ["BLOG"]
draft = false
+++

## 简介 {#简介}

`RESTfull` 即表现层状态转换，是一种设计风格，不是标准。其要点如下：

-   资源由 `URI` 来指定

    每个资源都有与之对应的 `URI=，一个 =URI` 只能对应一种资源，但一种资源可以对应多个 `URI` 不同的分类方法)。

-   对资源的操作对应到 `HTTP` 协议提供的方法

    如增删改查等操作，对应 `HTTP` 协议的 `POST, DELETE, PUT, GET` 等方法

-   资源的表现形式通常广泛流行的协议和标准

    如 `JSON, XML, HTML` 等格式，当然也可以是其它格式或者私有格式，但一定足够能描述自己

-   通过操作资源的表现形式来操作资源

    通过修改表现形式再辅以 `HTTP` 协议的方法，应用就可以管理资源

-   以超媒体作为应用状态的引擎

    资源相关操作的 `URI` 可附加在表现形式内，应用根据自身状态选择相应的 `URI` 来更改状态。

    操作可以是 `URI` 外，也可以是一段代码，让应用执行。

<!--more-->

总结起来就是：通过改变资源的表现形式来更改资源状态，以表现形式里的超媒体信息来引导应用的状态改变。

现在很多设计只用到了 **资源 `+` =HTTP=动词** 的方式，但 **超媒体**  不容忽视，是与应用交互的核心。


## `HTTP` 方法使用 {#http-方法使用}

下面介绍几种常用的 `HTTP` 协议方法:

-   POST

    用于资源的添加

-   DELETE

    用于资源的删除

-   PUT

    用于修改资源，需要提供完整的资源信息，可用于修改资源多项字段

-   GET

    用于资源的获取

-   PATCH

    用于修改资源的单个字段


## 示例 {#示例}

如构建一个用户信息服务，=API= 按照 `RESTfull` 风格可设计如下：

-   `[POST] /users/:user` 添加用户

-   `[DELETE] /users/:user` 删除用户

-   `[PUT] /users/:user` 修改用户整体信息

-   `[PATCH] /users/:user` 修改用户的单个字段

-   `[GET] /users/:user` 获取用户信息

-   `[GET] /users` 获取所有用户信息

另外在 `GET` 的信息中加入 `DELETE, PUT, PATCH` 的 `URI=，应用就可以根据这些 =URI` 显示可以进行的操作，动态的生成页面。
此种方法也减少了耦合，避免了 `URI` 变更时大量的修改。

---

尽管上面的描述很简单，但也能发现采用 `RESTfull` 来设计 `Web API` 是一种简洁明了的方案，通过与 `HTTP` 动词结合，避免了在 `URI` 中设置大量动词的问题，需要关注的只是对资源的抽象和划分。

本文到此为止，关于 `RESTfull` 更多更详尽的内容请自行查找。


## 参考文档 {#参考文档}

-   [深入浅出REST](http://www.infoq.com/cn/articles/rest-introduction)
-   [理解RESTful架构](http://www.ruanyifeng.com/blog/2011/09/restful.html)
-   [RESTful API 设计指南](http://www.ruanyifeng.com/blog/2014/05/restful%5Fapi.html)
-   [Restful API 的设计规范](http://novoland.github.io/%E8%AE%BE%E8%AE%A1/2015/08/17/Restful%20API%20%E7%9A%84%E8%AE%BE%E8%AE%A1%E8%A7%84%E8%8C%83.html)
