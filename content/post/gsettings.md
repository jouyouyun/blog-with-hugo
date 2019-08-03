+++
title = "GSettings"
date = 2019-08-03T19:30:21+08:00
lastmod = 2019-08-04T21:03:16+08:00
tags = ["gsettings"]
categories = ["BLOG"]
draft = false
+++

`GSettings` 实际上是一套可使用多个 `storage backends` 的 `API` ，默认使用 `dconf` 作为 `backend` ，也可以使用 `keyfile` 作为 `backend` 。但由于 `gio` 提供了 `GKeyFile` 结构来处理 `keyfile` 文件，所以解析 `keyfile` 文件时很少通过 `GSettings API` ，在这里只介绍使用 `dconf` 作为 `backend` 的使用方法。

<!--more-->

---


## dconf {#dconf}

`dconf` 是一个底层的配置管理系统，被设计用来作为 `GSettings` 的 `backend` ，并提供了图形化的配置管理工具 `dconf-editor` ，具有以下特点：

-   检索快速

    `dconf` 使用 `gvdb(GVariant Database file)` 格式的二进制文件存储数据，它是一个简单的数据库，以路径映射的方式存储 `key` ，检索高效。默认的文件路径为： `~/.config/dconf/user`

-   支持改变通知

    `dconf` 支持在 `key` 发生改变时发出通知

---


## 配置文件 {#配置文件}

`GSettings` 的配置文件是 `xml` 格式的，文件需以 `.gschema.xml` 结尾，文件名通常与 `id` 相同。配置文件安装在 `/usr/share/glib-2.0/schemas/` 目录下，手动添加进去的文件需要执行 `sudo glib-complie-schemas /usr/share/glib-2.0/schemas` 让其生效。

文件内容简要格式如下：

```xml
<?xml version="1.0" encoding="UTF-8"?>
<schemalist>
    <schema path="" id="">
        <key type=""  name="">
            <default></default>
            <summary></summary>
            <description></description>
        </key>
    </schema>
</schemalist>
```

可以看到一个文件里可以包含多个 `schema` ，每个 `schema` 可由多个 `key` 组成。详细说明如下：

-   id

    `schema` 中的 `id` 需要在整个配置系统中是唯一的，不然执行 `glib-compile-schemas` 时会忽略重复的 `id` ； `id` 的开头通常使用与应用相关的域名

-   path

    `schema` 中的 `path` 必须是以 `/` 开头并且以 `/` 结尾，不能包含连续的 `/` , `path` 用于指定在 `storage` 中存储路径，可以与 `id` 不一致

-   name

    `key` 的名称，需要在此 `schema` 中是唯一的， `name` 的值由小写字母、数字和 `-` 组成，并且开头必须是小写字母，不能以 `-` 结尾，也不能出现连续的 `-`

-   type

    `key` 的类型，需要是 `GVariant` 支持的类型，除了可以使用基本的类型外，也可按照 `GVariant` 的方式组合类型

-   default

    `key` 的默认值

-   summary

    `key` 的简单描述

-   description

    `key` 的详细描述

---

下面给出一个例子：

`schema` 文件：

```xml
<?xml version="1.0" encoding="UTF-8"?>
<schemalist>
    <enum id="com.deepin.test.LogLevel">
        <value nick="debug" value="1" />
        <value nick="info" value="2" />
        <value nick="fatal" value="3" />
    </enum>

    <schema path="/com/deepin/test/" id="com.deepin.test">
        <key type="b"  name="enabled">
            <default>true</default>
            <summary>Enable this module.</summary>
            <description>Whether enable this module.</description>
        </key>
        <key enum="com.deepin.test.LogLevel" name="loglevel">
            <default>'info'</default>
            <summary>Log level.</summary>
            <description>Log level.</description>
        </key>
        <key type="i" name="interval">
            <default>10</default>
            <range min="0" max="60" />
            <summary>Timer duration.</summary>
            <description>Timer duration.</description>
        </key>
        <key type="(ii)" name="size">
            <default>(800,600)</default>
            <summary>The window size.</summary>
            <description>The window size.</description>
        </key>
    </schema>
</schemalist>
```

程序代码如下：

```c
/**
 * Copyright (C) 2019 jouyouyun <jouyouwen717@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * example.c -- An example for gsettings.
 *
 * compile:
 *    gcc -Wall -g example.c `pkg-config --libs --cflags gio-2.0`
 *
 */
#include <gio/gio.h>

int
main(int argc, char *argv[])
{
    GSettings *gs;
    gboolean enabled;
    gint loglevel, interval, width, height;

    // if failed, will segmetfault
    gs = g_settings_new("com.deepin.test");

    enabled = g_settings_get_boolean(gs, "enabled");
    g_print("Enabled: %d\n", enabled);

    loglevel = g_settings_get_enum(gs, "loglevel");
    g_print("Log level: %d\n", loglevel);

    interval = g_settings_get_int(gs, "interval");
    g_print("Timer duration: %d\n", interval);

    g_settings_get(gs, "size", "(ii)", &width, &height);
    g_print("Width: %d, height: %d\n", width, height);

    g_object_unref(gs);
    return 0;
}
```

**注意：** 在使用接口调用时，需要确保 `schema` 和 `key` 存在，不然会触发 **段错误** 。

---


### `Relocatable Schema` {#relocatable-schema}

通常一个 `id` 对应一个固定的 `path` ，但也可以没有，即不设置 `path` ，这样就是一个可重定位的 `schema` 。这个特性对于可变的配置很有用，如用户自定义的快捷键：快捷键的个数是不可知，这时可利用这个特性先声明一个快捷键 `schema` 作为模板，然后动态创建多个 `path` 来使用。示例如下：

`schema` 文件如下：

```xml
<?xml version="1.0" encoding="UTF-8"?>
<schemalist>
    <schema id="com.deepin.test.keybind">
        <key type="s"  name="name">
            <default>''</default>
            <summary>Shortcut name.</summary>
            <description>Shortcut name.</description>
        </key>
        <key type="as"  name="shortcuts">
            <default>['']</default>
            <summary>Shortcut keys.</summary>
            <description>Shortcut keys.</description>
        </key>
        <key type="s"  name="action">
            <default>''</default>
            <summary>Shortcut action.</summary>
            <description>Shortcut action.</description>
        </key>
        <key type="s"  name="description">
            <default>''</default>
            <summary>Shortcut description.</summary>
            <description>Shortcut description.</description>
        </key>
    </schema>
</schemalist>
```

程序代码如下：

```c
/**
 * Copyright (C) 2019 jouyouyun <jouyouwen717@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * relocatable.c -- An example for gsettings relocatable path.
 *
 * compile:
 *    gcc -Wall -g relocatable.c `pkg-config --libs --cflags gio-2.0`
 */

#include <stdio.h>
#include <gio/gio.h>

static void set_keybind(GSettings *gs);
static void get_keybind(GSettings *gs);

static void
set_keybind(GSettings *gs)
{
    gboolean ok;

    ok = g_settings_set_string(gs, "name", "test");
    if (!ok) {
        g_warning("Failed to set name to 'test'");
    }

    const gchar *shortcuts[] = {"Control-Alt-T", "Super-T", NULL};
    ok = g_settings_set_strv(gs, "shortcuts", shortcuts);
    if (!ok) {
        g_warning("Failed to set shortcuts");
    }
    ok = g_settings_set_string(gs, "action", "terminator");
    if (!ok) {
        g_warning("Failed to set action to 'terminator'");
    }
    ok = g_settings_set_string(gs, "description", "the shortcuts for terminator");
    if (!ok) {
        g_warning("Failed to set description to 'the shortcuts for terminator'");
    }
}

static void
get_keybind(GSettings *gs)
{
    gchar *name, *action, *desc;
    gchar **shortcuts = NULL;

    name = g_settings_get_string(gs, "name");
    if (name) {
        g_print("Shortcut name: %s\n", name);
        g_free(name);
    }

    shortcuts = g_settings_get_strv(gs, "shortcuts");
    if (shortcuts) {
        int i = 0;
        g_print("Shortcut key: ");
        for (; shortcuts[i]; i++) {
            if (i != 0) {
                g_print(", ");
            }
            g_print("%s", shortcuts[i]);
            g_free(shortcuts[i]);
        }
        g_print("\n");
        g_free(shortcuts);
    }

    action = g_settings_get_string(gs, "action");
    if (action) {
        g_print("Shortcut action: %s\n", action);
        g_free(action);
    }

    desc = g_settings_get_string(gs, "description");
    if (desc) {
        g_print("Shortcut desc: %s\n", desc);
        g_free(desc);
    }
}

int
main(int argc, char *argv[])
{
    if (argc != 3) {
        g_print("Usage: %s <action> <keybind id>\n"
                "\taction: get or set\n"
                "\tkeybind id: a non-negative integer\n", argv[0]);
        return 0;
    }

    GSettings *gs;
    gchar buf[1024] = {0};

    if (sprintf(buf, "/com/deepin/test/keybind/%s/", argv[2]) < 0) {
        g_error("Failed to combains path: %s\n", argv[2]);
        return -1;
    }
    gs = g_settings_new_with_path("com.deepin.test.keybind", buf);

    if (g_strcmp0(argv[1], "set") == 0) {
        set_keybind(gs);
    } else if (g_strcmp0(argv[1], "get") == 0) {
        get_keybind(gs);
    } else {
        g_warning("invalid action: %s", argv[1]);
    }

    g_object_unref(gs);
    g_settings_sync();
    return 0;
}
```

---


### override {#override}

`gsettings` 支持 `override` 机制，可以更改默认值。

通常 `key` 的默认值在文件中已经指定，但有时不同的版本上需要的默认值不同，这时 `override` 机制就很有用，可以在不修改配置文件的情况下更改默认值。

`override` 文件以 **`数字_`** 开头，并以 **`.gschema.override`** 结尾，数字为两位整数，数值越大优先级越高。

`override` 文件同样安装在 `/usr/share/glib-2.0/schemas` 目录下，并需要使用 `glib-compile-schemas` 编译。

`override` 文件的格式如下：

```ini
[schema id]
key=value
```

如更改 `com.deepin.test` 中 `interval` 的默认值为 `30` ，则内容如下：

```ini
[com.deepin.test]
interval=30
```

然后执行：

```shell
sudo cp ./90_com.deepin.test.gschema.override /usr/share/glib-2.0/schemas/
sudo glib-compile-schemas /usr/share/glib-2.0/schemas
```

---


### key 改变信号 {#key-改变信号}

`GSettings` 提供了 `changed` 信号来发送 `key` 改变的事件，如有需要可监听此信号。

---


## 工具 {#工具}


### glib-sompile-schemas {#glib-sompile-schemas}

编译 `schema` 文件的命令


### `dconf-editor` {#dconf-editor}

`dconf` 的图形工具，根据 `path` 检索，可管理 `key`


### devhelp {#devhelp}

一款 `API` 文档查看工具，安装了 `libglib2.0-docs` 后，可在 `devhelp` 中查看 `GSettings` 的 `API` 文档


### gsettings {#gsettings}

这是一个 `gsettings` 的配置管理工具，下面介绍一些常用的子命令，详细用法请查看 `man` 手册：

-   list-schemas

    列出所有已安装并有固定 `path` 的 `schema`

-   list-relocatable-schemas

    列出所有已安装却没有固定 `path` 的 `schema`

-   list-keys

    列出指定 `schema` 中的所有 `key`

-   list-recursively

    列出指定 `schema` 中的所有 `key` 及其 `value` ，若 `schema` 未指定，则列出所有 `schema` 的

-   get

    获取指定 `schema` 中指定 `key` 的值

-   set

    设置指定 `schema` 中指定 `key` 的值

-   reset

    重置指定 `schema` 中指定 `key` 的值，即恢复默认值

-   monitor

    监听指定 `schema` 中指定 `key` 的改变，不指定时监听所有的

---


## 参考资料 {#参考资料}

-   [dconf](https://en.wikipedia.org/wiki/Dconf)
-   [GSettings](https://developer.gnome.org/GSettings/)
-   [GSettings API](https://developer.gnome.org/gio/stable/GSettings.html)
-   [GVariant Format Strings](https://developer.gnome.org/glib/stable/gvariant-format-strings.html)
