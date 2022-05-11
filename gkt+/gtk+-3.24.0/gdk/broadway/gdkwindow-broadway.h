/* GDK - The GIMP Drawing Kit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#ifndef __GDK_WINDOW_BROADWAY_H__
#define __GDK_WINDOW_BROADWAY_H__

#include <gdk/gdkwindowimpl.h>

G_BEGIN_DECLS

typedef struct _GdkWindowImplBroadway GdkWindowImplBroadway;
typedef struct _GdkWindowImplBroadwayClass GdkWindowImplBroadwayClass;

/* Window implementation for Broadway
 */

#define GDK_TYPE_WINDOW_IMPL_BROADWAY              (gdk_window_impl_broadway_get_type ())
#define GDK_WINDOW_IMPL_BROADWAY(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), GDK_TYPE_WINDOW_IMPL_BROADWAY, GdkWindowImplBroadway))
#define GDK_WINDOW_IMPL_BROADWAY_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), GDK_TYPE_WINDOW_IMPL_BROADWAY, GdkWindowImplBroadwayClass))
#define GDK_IS_WINDOW_IMPL_BROADWAY(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), GDK_TYPE_WINDOW_IMPL_BROADWAY))
#define GDK_IS_WINDOW_IMPL_BROADWAY_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GDK_TYPE_WINDOW_IMPL_BROADWAY))
#define GDK_WINDOW_IMPL_BROADWAY_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), GDK_TYPE_WINDOW_IMPL_BROADWAY, GdkWindowImplBroadwayClass))

struct _GdkWindowImplBroadway
{
  GdkWindowImpl parent_instance;

  GdkWindow *wrapper;
  GdkScreen *screen;

  cairo_surface_t *surface;
  cairo_surface_t *last_surface;
  cairo_surface_t *ref_surface;

  GdkCursor *cursor;
  GHashTable *device_cursor;

  int id;

  gboolean visible;
  gboolean maximized;
  int transient_for;

  int pre_maximize_x;
  int pre_maximize_y;
  int pre_maximize_width;
  int pre_maximize_height;

  gint8 toplevel_window_type;
  gboolean dirty;
  gboolean last_synced;

  GdkGeometry geometry_hints;
  GdkWindowHints geometry_hints_mask;
};

struct _GdkWindowImplBroadwayClass
{
  GdkWindowImplClass parent_class;
};

GType gdk_window_impl_broadway_get_type (void);

G_END_DECLS

#endif /* __GDK_WINDOW_BROADWAY_H__ */
