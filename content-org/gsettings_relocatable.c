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
