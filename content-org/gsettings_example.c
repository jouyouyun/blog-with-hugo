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
