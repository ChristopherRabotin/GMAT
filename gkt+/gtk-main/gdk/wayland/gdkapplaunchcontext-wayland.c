/*
 * Copyright © 2010 Intel Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <string.h>
#include <unistd.h>

#include <glib.h>
#include <gio/gdesktopappinfo.h>

#include "gdkwayland.h"
#include "gdkprivate-wayland.h"
#include "gdkapplaunchcontextprivate.h"
#include "gdkintl.h"

typedef struct {
  gchar *token;
} AppLaunchData;

static void
token_done (gpointer                        data,
            struct xdg_activation_token_v1 *provider,
            const char                     *token)
{
  AppLaunchData *app_launch_data = data;

  app_launch_data->token = g_strdup (token);
}

static const struct xdg_activation_token_v1_listener token_listener = {
  token_done,
};

static char *
gdk_wayland_app_launch_context_get_startup_notify_id (GAppLaunchContext *context,
                                                      GAppInfo          *info,
                                                      GList             *files)
{
  GdkWaylandDisplay *display;
  gchar *id = NULL;

  g_object_get (context, "display", &display, NULL);

  if (display->xdg_activation)
    {
      struct xdg_activation_token_v1 *token;
      GdkWaylandSeat *seat;
      GdkSurface *focus_surface;
      AppLaunchData app_launch_data = { 0 };

      seat = GDK_WAYLAND_SEAT (gdk_display_get_default_seat (GDK_DISPLAY (display)));
      focus_surface = gdk_wayland_device_get_focus (gdk_seat_get_keyboard (GDK_SEAT (seat)));
      token = xdg_activation_v1_get_activation_token (display->xdg_activation);

      xdg_activation_token_v1_add_listener (token,
                                            &token_listener,
                                            &app_launch_data);
      xdg_activation_token_v1_set_serial (token,
                                          _gdk_wayland_seat_get_last_implicit_grab_serial (seat, NULL),
                                          gdk_wayland_seat_get_wl_seat (GDK_SEAT (seat)));
      if (focus_surface)
        xdg_activation_token_v1_set_surface (token,
                                             gdk_wayland_surface_get_wl_surface (focus_surface));
      xdg_activation_token_v1_commit (token);

      while (app_launch_data.token == NULL)
        wl_display_roundtrip (display->wl_display);

      xdg_activation_token_v1_destroy (token);
      id = app_launch_data.token;
    }
  else if (display->gtk_shell_version >= 3)
    {
      id = g_uuid_string_random ();
      gtk_shell1_notify_launch (display->gtk_shell, id);
    }

  g_object_unref (display);

  return id;
}

static void
gdk_wayland_app_launch_context_launch_failed (GAppLaunchContext *context,
                                              const char        *startup_notify_id)
{
}

typedef struct _GdkWaylandAppLaunchContext GdkWaylandAppLaunchContext;
typedef struct _GdkWaylandAppLaunchContextClass GdkWaylandAppLaunchContextClass;

struct _GdkWaylandAppLaunchContext
{
  GdkAppLaunchContext base;
  char *name;
  guint serial;
};

struct _GdkWaylandAppLaunchContextClass
{
  GdkAppLaunchContextClass base_class;
};

GType gdk_wayland_app_launch_context_get_type (void);

G_DEFINE_TYPE (GdkWaylandAppLaunchContext, gdk_wayland_app_launch_context, GDK_TYPE_APP_LAUNCH_CONTEXT)

static void
gdk_wayland_app_launch_context_class_init (GdkWaylandAppLaunchContextClass *klass)
{
  GAppLaunchContextClass *ctx_class = G_APP_LAUNCH_CONTEXT_CLASS (klass);

  ctx_class->get_startup_notify_id = gdk_wayland_app_launch_context_get_startup_notify_id;
  ctx_class->launch_failed = gdk_wayland_app_launch_context_launch_failed;
}

static void
gdk_wayland_app_launch_context_init (GdkWaylandAppLaunchContext *ctx)
{
}

GdkAppLaunchContext *
_gdk_wayland_display_get_app_launch_context (GdkDisplay *display)
{
  GdkAppLaunchContext *ctx;

  ctx = g_object_new (gdk_wayland_app_launch_context_get_type (),
                      "display", display,
                      NULL);

  return ctx;
}
