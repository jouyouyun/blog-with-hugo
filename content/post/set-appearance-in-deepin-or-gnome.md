+++
title = "linux 下 deepin 和 gnome 桌面的外观设置"
date = 2018-10-23T21:38:21+08:00
lastmod = 2019-02-12T14:36:19+08:00
tags = ["deepin", "gnome", "theme", "gtk", "icon", "cursor", "font"]
categories = ["BLOG"]
draft = false
+++

桌面的外观设置一般包括窗口主题、图标主题、光标主题和字体设置，其中窗口主题就是常见的 `gtk` 主题，字体设置又包括标准字体、等款字体和字体大小。对于进些年的高分屏，又添加了字体缩放和窗口缩放这2个选项。

界面上设置方法就不介绍了，都可以在各自桌面的控制中心中找到，这里主要介绍命令行下的。

`deepin` 和 `gnome` 桌面环境的外观配置数据是使用 `gsettings` 来存储，所以可以通过 `gsettings` 来设置和读取。 `gsettings` 相比设置界面就是 可以设置到更多的选项，设置的值更加精确。

下面先简单介绍下 `gsettings` 的用法。


## `GSettings` {#gsettings}

<!--more-->

-   获取 `key`

    `gsettings get <schema_id> <key>`

-   设置 `key`

    `gsettings set <schema_id> <key> <value>`

其中 `schema` 是在 `/usr/share/glib-2.0/schemas` 这个目录下， `schema_id` 是文件内容中 `id` 的值, `key` 就是文件中描述的，每个 `key` 都有一个默认值。

如 `deepin` 的主题配置文件是 =com.deepin.dde.appearance.gchema.xml=，通过其文件开头的内容

```xml
<schema path="/com/deepin/dde/appearance/" id="com.deepin.dde.appearance" gettext-domain="DDE">
```

可知，其 `schema_id` 就是 `com.deepin.dde.appearance` 。


## 外观设置 {#外观设置}

`gnome` 的外观配置的 `id` 是 `org.gnome.desktop.interface=，=deepin` 的则是 `com.deepin.dde.appearance` 和 `com.deepin.xsettings` 。

通过命令 `gsettings list-keys <schema_id>` 可以查询到所有支持的 `key=，如查询 =gnome` 支持的 `key` ：

```shell
$ gsettings list-keys org.gnome.desktop.interface
gtk-theme
cursor-blink-timeout
gtk-color-palette
gtk-timeout-repeat
toolkit-accessibility
cursor-theme
toolbar-style
toolbar-icons-size
text-scaling-factor
icon-theme
gtk-timeout-initial
scaling-factor
...
```

每个 `key` 的含义请阅读对应的 `schema` 文件来查询。


### 窗口主题 {#窗口主题}

窗口主题对应的 `key` 是 =gtk-theme=，值是字符串类型。

所有系统附带的 `gtk` 主题都在 `/usr/share/themes` 目录下。


#### 读取 {#读取}

-   `gnome`

    `gsettings get org.gnome.desktop.interface gtk-theme`

-   `deepin`

    `gsettings get com.deepin.dde.appearance gtk-theme`


#### 设置 {#设置}

如设置 `Adapta` 作为窗口主题

-   `gnome`

    `gsettings set org.gnome.desktop.interface gtk-theme Adapta`

-   `deepin`

    `gsettings set com.deepin.dde.appearance gtk-them Adapta`


#### 恢复默认值 {#恢复默认值}

-   `gnome`

    `gsettings reset org.gnome.desktop.interface gtk-theme`

-   `deepin`

    `gsettings reset com.deepin.dde.appearance gtk-theme`


### 图标和光标主题 {#图标和光标主题}

图标主题的 `key` 是 `icon-theme` ，光标主题的 `key` 是 `cursor-theme` ，值都是字符串类型，系统附带的图标和光标主题都在 `/usr/share/icons/` 目录下。

主题的设置和获取方法与上面的窗口主题一样，这里就不再赘述了。


### 字体设置 {#字体设置}

`gnome` 标准字体设置的 `key` 是 `font-name` ，等宽字体的 `key` 是 `monospace-font-name` ，值也都是字符串类型，字体的大小附加在字体名称后面一起设置，如设置标准字体：

`gsettings set org.gnome.desktop.interface font-name 'Cantarell 11'`

`deepin` 下标准字体的 `key` 是 `font-standard` ，等宽字体的是 `font-monospace` ，值也是字符串类型。但它的字体大小是单独在 `font-size` 中设置的，值是浮点数类型。
使用 `gsettings` 操作的方法就不再赘述了，与窗口主题一致。


#### `HIDPI` 缩放 {#hidpi-缩放}

`gnome` 下字体缩放的 `key` 是 `text-scaling-factor=，它的值是浮点数类型；窗口缩放的 =key` 是 `scaling-factor` ，但它的值是无符号整型，即非负的整数。

`deepin` 下缩放的 **schema\_id** 变为了 **com.deepin.xsettings** ，字体缩放的 `key` 是 `scale-factor` ，它的值也是浮点数类型；窗口缩放的 `key` 是 `window-scale` ，它的值也是无符号整型。

**注意：** 只有在设置 `deepin` 缩放时 `schema_id` 才是 `com.deepin.xsettings` ，其他的设置时都是 `com.deepin.dde.appearance` 。

---

以上介绍的设置方法在使用 `deepin` 或 `gnome` 作为桌面的 `linux` 发行版都可以适用，如 `ubuntu=，=fedora` 和 `deepin` 等，这些设置的改变桌面环境是会实时响应。

希望这些能够帮助到你，如写一个自动备份和恢复的脚本等...

如果你是想知道这些设置是如何生效的，请持续关注哦，本人计划在下篇文章中介绍 `deepin` 下的实现原理，谢谢支持。
