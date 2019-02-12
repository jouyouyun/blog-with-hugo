+++
title = "XRecord 示例"
date = 2014-04-18T17:42:15+08:00
lastmod = 2019-02-12T14:37:01+08:00
tags = ["xtst", "xrecord"]
categories = ["NOTE"]
draft = false
+++

最近在做快捷键绑定的项目，但在绑定单按键时发现 `XGrabKey` 无法处理。遂请教组长，得知 `XRecord` 可以处理，于是就去查找 `XRecord` 的相关资料，但却很少。
所以在此记录一下整理的示例代码，以便日后查看，其功能在代码注释中写出。

<!--more-->

```c
/*
 * XRecord 是 XLib 的一个扩展，在 xtst 中。
 * 此程序用来监听按键、鼠标点击和鼠标移动事件
 */

#include <glib.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/extensions/record.h>

typedef struct _RecordEventInfo {
        Display *ctrl_disp; // 控制通道
        Display *data_disp; // 数据通道
        XRecordRange *range;
        XRecordContext context;
} RecordEventInfo;

typedef union {
        unsigned char type; // 事件类型
        xEvent xe; // 事件
} RecordDate;

static void record_init ();
static void record_finalize ();
static void record_event_cb (XPointer user_data, XRecordInterceptData *hook);
static gpointer enable_ctx_thread (gpointer user_data);

GMainLoop *loop= NULL;
static RecordEventInfo *grab_info = NULL;

static void
record_init ()
{
        grab_info = g_new0 (RecordEventInfo, 1);

        if ( !grab_info ) {
                g_warning ("Alloc RecordEventInfo memory failed...");
                record_finalize ();
        }

        grab_info->ctrl_disp = XOpenDisplay (NULL);
        grab_info->data_disp = XOpenDisplay (NULL);

        if ( !grab_info->ctrl_disp || !grab_info->data_disp ) {
                g_warning ("Unable to connect to X server...");
                record_finalize ();
        }

        gint dummy;

        if ( !XQueryExtension (grab_info->ctrl_disp, "XTEST",
                                   &dummy, &dummy, &dummy) ) {
                g_warning ("XTest extension missing...");
                record_finalize ();
        }

        if ( !XRecordQueryVersion (grab_info->ctrl_disp, &dummy, &dummy) ) {
                g_warning ("Failed to obtain xrecord version...");
                record_finalize ();
        }

        grab_info->range = XRecordAllocRange ();

        if ( !grab_info->range ) {
                g_warning ("Alloc XRecordRange memory failed...");
                record_finalize ();
        }

        // 定义需要监听的事件范围，具体见 XRecord 文档
        grab_info->range->device_events.first = KeyPress;
        grab_info->range->device_events.last = MotionNotify;

        XRecordClientSpec spec = XRecordAllClients;
        grab_info->context = XRecordCreateContext (
                                                grab_info->data_disp,
                                                0, &spec, 1,
                                                &grab_info->range, 1);

        if ( !grab_info->context ) {
                g_warning ("Unable to create context...");
                record_finalize();
        }

        XSynchronize (grab_info->ctrl_disp, TRUE);
        XFlush (grab_info->ctrl_disp);

        // 创建线程来处理接收到的数据
        GThread *thrd = g_thread_new ("enable context",
                                (GThreadFunc)enable_ctx_thread, NULL);

        if ( !thrd ) {
                g_warning ("Unable to create thread...");
                record_finalize ();
        }
}

/*
 * 释放资源
 */

static void
record_finalize ()
{
        if (!grab_info) {
                return;
        }

        if (grab_info->context) {
                XRecordDisableContext(grab_info->data_disp, grab_info->context);
                XRecordFreeContext(grab_info->data_disp, grab_info->context);
        }

        if (grab_info->range) {
                XFree(grab_info->range);
                grab_info->range = NULL;
        }

        if (grab_info->ctrl_disp) {
                XCloseDisplay (grab_info->ctrl_disp);
                grab_info->ctrl_disp = NULL;
        }

        if (grab_info->data_disp) {
                XCloseDisplay (grab_info->data_disp);
                grab_info->data_disp = NULL;
        }

        if (grab_info) {
                g_free (grab_info);
                grab_info = NULL;
        }
}

static gpointer
enable_ctx_thread (gpointer user_data)
{
        // 开始接收数据
        if ( !XRecordEnableContext (grab_info->data_disp,
                                        grab_info->context,
                                        record_event_cb, NULL) ) {
                g_warning ("Unable to enable context...");
                record_finalize ();
        }

        g_thread_exit (NULL);
        g_main_loop_quit(loop);

        return NULL;
}

static void
record_event_cb (XPointer user_data, XRecordInterceptData *hook)
{
        if ( hook->category != XRecordFromServer ) {
                XRecordFreeData(hook);
                g_warning ("Data not from X server...");
                return;
        }

        RecordDate *data = (RecordDate *)hook->data;
        int detail = data->xe.u.u.detail; // 按键或鼠标的 keycode
        int event_type = data->type; // 事件类型
        // 事件发生时的坐标
        int rootX = data->xe.u.keyButtonPointer.rootX;
        int rootY = data->xe.u.keyButtonPointer.rootY;
        /*int time = hook->server_time;*/

        switch (event_type) {
        case KeyPress:
                g_print("%d pressed\n", detail);
                /*KeySym sym = XKeycodeToKeysym(grab_info->data_disp, detail, 0);*/
                break;

        case KeyRelease:
                g_print("%d released\n", detail);
                break;

        case MotionNotify:
                g_print("Mouse Motion: %d -- %d\n", rootX, rootY);
                break;

        case ButtonPress:
                g_print("Mouse %d pressed\n", detail);
                break;

        case ButtonRelease:
                g_print("Mouse %d released\n", detail);
                break;

        default:
                break;
        }

        XRecordFreeData(hook);
}

int
main(int argc, char *argv[])
{
        record_init();

        loop = g_main_loop_new(NULL, FALSE);
        g_main_loop_run(loop);
        g_main_loop_unref(loop);
        record_finalize();

        return 0;
}
```
