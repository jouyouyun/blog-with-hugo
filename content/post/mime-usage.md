+++
title = "MIME Usage"
date = 2016-12-14T21:38:21+08:00
lastmod = 2019-02-10T18:24:30+08:00
tags = ["mime"]
categories = ["BLOG"]
draft = false
+++

## 基本使用 {#基本使用}

使用到的工具为: `xdg-mime, gvfs-mime`

-   查询文件的类型

    使用 `xdg-mime query filetype <filepath>` 可以得到文件的类型

-   查询默认程序

    使用 `xdg-mime query default text/plain` 或 `gvfs-mime --query <mimetype>`, `gvfs-mime` 还会列出支持此 `mimetype` 的所有程序

-   设置默认程序

    如将 `text/plain` 的默认打开程序设置为 `gvim.desktop`, 可以执行 `xdg-mime default gvim.desktop text/plain` 或 `gvfs-mime --set text/plain gvim.desktop`


## 根据文件扩展名指定类型 {#根据文件扩展名指定类型}

1.  创建 `xml` 文件, 指定类型, 如将 `*.org` 指定为 `text/org-mode`, 文件内容如下:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
  <mime-type type="text/org-mode">
         <comment xml:lang="en">Org-Mode File</comment>
         <glob pattern="*.org"/>
  </mime-type>
</mime-info>
```

1.  将上面的 `xml` 文件复制到 `$XDG_DATA_DIRS/mime/packages` 中, 即文件要在 `~/.local/share/mime/packages, /usr/local/share/mime/packages, /usr/share/mime/packages` 这几个目录中的一个
2.  执行 `update-mime-database <MIME DIR>` 更新 `mime datebase`, 上面的目录去掉 `packages` 就是 `MIME DIR`

这样 `*.org` 的类型就变为 `text/org-mode` 了, 参考 [Add MIME Type](https://freedesktop.org/wiki/Specifications/AddingMIMETutor/)
