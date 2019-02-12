+++
title = "Auto Hidden Cursor"
date = 2016-04-22T21:38:21+08:00
lastmod = 2019-02-12T14:29:40+08:00
tags = ["cursor"]
categories = ["BLOG"]
draft = false
+++

最近需要做一个隐藏光标的功能, 在这里将了解到的知识总结一下.


## 原理 {#原理}

隐藏光标的原理就是把光标设置为一个 `x11 null cursor`, 不需要隐藏时再将它设为 `x11 left_ptr cursor`.


### 设置光标 {#设置光标}

设置光标主要需要调用下面两个函数:

<!--more-->

```c
int XDefineCursor(Display *display, Window w, Cursor cursor);
void XFixesChangeCursorByName (Display *dpy, Cursor source, const char *name);
```


### 创建光标 {#创建光标}

至于创建 `null cursor` 可以见下面的代码:

```c
window = DefaultRootWindow(display);
Cursor invisibleCursor;
Pixmap bitmapNoData;
XColor black;
static char noData[] = { 0,0,0,0,0,0,0,0 };
black.red = black.green = black.blue = 0;
bitmapNoData = XCreateBitmapFromData(display, window, noData, 8, 8);
invisibleCursor = XCreatePixmapCursor(display, bitmapNoData, bitmapNoData, &black, &black, 0, 0);
```

创建完后, 将其设置为默认光标就可以了.

另外 `libxfixes` 也提供了隐藏/显示光标的接口:

```c
void XFixesHideCursor (Display *dpy, Window win);
void XFixesShowCursor (Display *dpy, Window win);
```


## 其它 {#其它}

隐藏光标推荐使用 `unclutter` 来设置, 它可以自定义空闲时间, 具体功能请见 `man` 手册.

如果使用 `lightdm` 作为登录管理器, 可以编辑 `/etc/lightdm/lightdm.conf` 文件,
在 `[Seat:*]` 后添加 `xserver-command=X -bs -core -nocursor`.
