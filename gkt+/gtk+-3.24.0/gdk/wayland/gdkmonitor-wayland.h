/*
 * Copyright © 2016 Red Hat, Inc
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GDK_WAYLAND_MONITOR_PRIVATE_H__
#define __GDK_WAYLAND_MONITOR_PRIVATE_H__

#include <glib.h>
#include "gdkwaylandmonitor.h"
#include "gdkmonitorprivate.h"
#include "gdkprivate-wayland.h"

struct _GdkWaylandMonitor {
  GdkMonitor parent;

  guint32 id;
  guint32 version;
  struct wl_output *output;
  gboolean added;
};

struct _GdkWaylandMonitorClass {
  GdkMonitorClass parent_class;
};

#endif
