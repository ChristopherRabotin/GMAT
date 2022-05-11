/* GDK - The GIMP Drawing Kit
 *
 * gdkglcontext-wayland.c: Wayland specific OpenGL wrappers
 *
 * Copyright © 2014  Emmanuele Bassi
 * Copyright © 2014  Alexander Larsson
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

#include "config.h"

#include "gdkglcontext-wayland.h"

#include "gdkdisplay-wayland.h"
#include "gdksurface-wayland.h"

#include "gdkwaylanddisplay.h"
#include "gdkwaylandglcontext.h"
#include "gdkwaylandsurface.h"
#include "gdkprivate-wayland.h"

#include "gdk-private.h"
#include "gdksurfaceprivate.h"
#include "gdkprofilerprivate.h"

#include "gdkintl.h"

/**
 * GdkWaylandGLContext:
 *
 * The Wayland implementation of `GdkGLContext`.
 */

G_DEFINE_TYPE (GdkWaylandGLContext, gdk_wayland_gl_context, GDK_TYPE_GL_CONTEXT)

static void
gdk_wayland_gl_context_begin_frame (GdkDrawContext *draw_context,
                                    gboolean        prefers_high_depth,
                                    cairo_region_t *region)
{
  gdk_wayland_surface_ensure_wl_egl_window (gdk_draw_context_get_surface (draw_context));

  GDK_DRAW_CONTEXT_CLASS (gdk_wayland_gl_context_parent_class)->begin_frame (draw_context, prefers_high_depth, region);
}

static void
gdk_wayland_gl_context_end_frame (GdkDrawContext *draw_context,
                                  cairo_region_t *painted)
{
  GdkSurface *surface = gdk_draw_context_get_surface (draw_context);

  gdk_wayland_surface_sync (surface);
  gdk_wayland_surface_request_frame (surface);

  GDK_DRAW_CONTEXT_CLASS (gdk_wayland_gl_context_parent_class)->end_frame (draw_context, painted);

  gdk_wayland_surface_notify_committed (surface);
}

static void
gdk_wayland_gl_context_class_init (GdkWaylandGLContextClass *klass)
{
  GdkDrawContextClass *draw_context_class = GDK_DRAW_CONTEXT_CLASS (klass);
  GdkGLContextClass *context_class = GDK_GL_CONTEXT_CLASS (klass);

  draw_context_class->begin_frame = gdk_wayland_gl_context_begin_frame;
  draw_context_class->end_frame = gdk_wayland_gl_context_end_frame;

  context_class->backend_type = GDK_GL_EGL;
}

static void
gdk_wayland_gl_context_init (GdkWaylandGLContext *self)
{
}

/**
 * gdk_wayland_display_get_egl_display:
 * @display: (type GdkWaylandDisplay): a Wayland display
 *
 * Retrieves the EGL display connection object for the given GDK display.
 *
 * Returns: (nullable): the EGL display
 *
 * Since: 4.4
 */
gpointer
gdk_wayland_display_get_egl_display (GdkDisplay *display)
{
  g_return_val_if_fail (GDK_IS_WAYLAND_DISPLAY (display), NULL);

  return gdk_display_get_egl_display (display);
}

GdkGLContext *
gdk_wayland_display_init_gl (GdkDisplay  *display,
                             GError     **error)
{
  GdkWaylandDisplay *self = GDK_WAYLAND_DISPLAY (display);

  if (!gdk_display_init_egl (display, 
                             EGL_PLATFORM_WAYLAND_EXT,
                             self->wl_display,
                             TRUE,
                             error))
    return NULL;

  return g_object_new (GDK_TYPE_WAYLAND_GL_CONTEXT,
                       "display", display,
                       NULL);
}

