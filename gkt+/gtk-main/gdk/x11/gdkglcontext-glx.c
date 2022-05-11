/* GDK - The GIMP Drawing Kit
 *
 * gdkglcontext-glx.c: GLX specific wrappers
 *
 * SPDX-FileCopyrightText: 2014  Emmanuele Bassi
 * SPDX-FileCopyrightText: 2021  GNOME Foundation
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "config.h"

#include "gdkglcontext-x11.h"
#include "gdkdisplay-x11.h"
#include "gdkprivate-x11.h"
#include "gdkscreen-x11.h"

#include "gdkx11display.h"
#include "gdkx11glcontext.h"
#include "gdkx11screen.h"
#include "gdkx11surface.h"
#include "gdkx11property.h"
#include <X11/Xatom.h>

#include "gdkprofilerprivate.h"
#include "gdkintl.h"

#include <cairo-xlib.h>

#include <epoxy/glx.h>

struct _GdkX11GLContextGLX
{
  GdkX11GLContext parent_instance;

  GLXContext glx_context;

#ifdef HAVE_XDAMAGE
  GLsync frame_fence;
  Damage xdamage;
#endif

  guint do_frame_sync : 1;
};

typedef struct _GdkX11GLContextClass    GdkX11GLContextGLXClass;

G_DEFINE_TYPE (GdkX11GLContextGLX, gdk_x11_gl_context_glx, GDK_TYPE_X11_GL_CONTEXT)

static GLXDrawable
gdk_x11_surface_get_glx_drawable (GdkSurface *surface)
{
  GdkX11Surface *self = GDK_X11_SURFACE (surface);
  GdkDisplay *display = gdk_surface_get_display (GDK_SURFACE (self));
  GdkX11Display *display_x11 = GDK_X11_DISPLAY (display);

  if (self->glx_drawable)
    return self->glx_drawable;

  self->glx_drawable = glXCreateWindow (gdk_x11_display_get_xdisplay (display),
                                        display_x11->glx_config,
                                        gdk_x11_surface_get_xid (surface),
                                        NULL);

  return self->glx_drawable;
}

void
gdk_x11_surface_destroy_glx_drawable (GdkX11Surface *self)
{
  if (self->glx_drawable == None)
    return;

  gdk_gl_context_clear_current_if_surface (GDK_SURFACE (self));

  glXDestroyWindow (gdk_x11_display_get_xdisplay (gdk_surface_get_display (GDK_SURFACE (self))),
                    self->glx_drawable);

  self->glx_drawable = None;
}

static void
maybe_wait_for_vblank (GdkDisplay  *display,
                       GLXDrawable  drawable)
{
  GdkX11Display *display_x11 = GDK_X11_DISPLAY (display);
  Display *dpy = gdk_x11_display_get_xdisplay (display);

  if (display_x11->has_glx_sync_control)
    {
      gint64 ust, msc, sbc;

      glXGetSyncValuesOML (dpy, drawable, &ust, &msc, &sbc);
      glXWaitForMscOML (dpy, drawable,
                        0, 2, (msc + 1) % 2,
                        &ust, &msc, &sbc);
    }
  else if (display_x11->has_glx_video_sync)
    {
      guint32 current_count;

      glXGetVideoSyncSGI (&current_count);
      glXWaitVideoSyncSGI (2, (current_count + 1) % 2, &current_count);
    }
}

static GLXDrawable
gdk_x11_gl_context_glx_get_drawable (GdkX11GLContextGLX *self)
{
  GdkDrawContext *draw_context = GDK_DRAW_CONTEXT (self);
  GdkSurface *surface;

  if (gdk_draw_context_is_in_frame (draw_context))
    surface = gdk_draw_context_get_surface (draw_context);
  else
    surface = GDK_X11_DISPLAY (gdk_draw_context_get_display (draw_context))->leader_gdk_surface;

  return gdk_x11_surface_get_glx_drawable (surface);
}

static void
gdk_x11_gl_context_glx_end_frame (GdkDrawContext *draw_context,
                                  cairo_region_t *painted)
{
  GdkX11GLContextGLX *self = GDK_X11_GL_CONTEXT_GLX (draw_context);
  GdkGLContext *context = GDK_GL_CONTEXT (draw_context);
  GdkSurface *surface = gdk_gl_context_get_surface (context);
  GdkX11Surface *x11_surface = GDK_X11_SURFACE (surface);
  GdkDisplay *display = gdk_gl_context_get_display (context);
  Display *dpy = gdk_x11_display_get_xdisplay (display);
  GdkX11Display *display_x11 = GDK_X11_DISPLAY (display);
  GLXDrawable drawable;

  GDK_DRAW_CONTEXT_CLASS (gdk_x11_gl_context_glx_parent_class)->end_frame (draw_context, painted);

  gdk_gl_context_make_current (context);

  drawable = gdk_x11_surface_get_glx_drawable (surface);

  GDK_DISPLAY_NOTE (display, OPENGL,
            g_message ("Flushing GLX buffers for drawable %lu (window: %lu), frame sync: %s",
                       (unsigned long) drawable,
                       (unsigned long) gdk_x11_surface_get_xid (surface),
                       self->do_frame_sync ? "yes" : "no"));

  gdk_profiler_add_mark (GDK_PROFILER_CURRENT_TIME, 0, "x11", "swap buffers");

  /* if we are going to wait for the vertical refresh manually
   * we need to flush pending redraws, and we also need to wait
   * for that to finish, otherwise we are going to tear.
   *
   * obviously, this condition should not be hit if we have
   * GLX_SGI_swap_control, and we ask the driver to do the right
   * thing.
   */
  if (self->do_frame_sync)
    {
      guint32 end_frame_counter = 0;
      gboolean has_counter = display_x11->has_glx_video_sync;
      gboolean can_wait = display_x11->has_glx_video_sync || display_x11->has_glx_sync_control;

      if (display_x11->has_glx_video_sync)
        glXGetVideoSyncSGI (&end_frame_counter);

      if (self->do_frame_sync && !display_x11->has_glx_swap_interval)
        {
          glFinish ();

          if (has_counter && can_wait)
            {
              if (x11_surface->glx_frame_counter == end_frame_counter)
                maybe_wait_for_vblank (display, drawable);
            }
          else if (can_wait)
            maybe_wait_for_vblank (display, drawable);
        }
    }

  gdk_x11_surface_pre_damage (surface);

#ifdef HAVE_XDAMAGE
  if (self->xdamage != 0 && _gdk_x11_surface_syncs_frames (surface))
    {
      g_assert (self->frame_fence == 0);

      self->frame_fence = glFenceSync (GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

      /* We consider the frame still getting painted until the GL operation is
       * finished, and the window gets damage reported from the X server.
       * It's only at this point the compositor can be sure it has full
       * access to the new updates.
       */
      _gdk_x11_surface_set_frame_still_painting (surface, TRUE);
    }
#endif

  glXSwapBuffers (dpy, drawable);

  if (self->do_frame_sync && display_x11->has_glx_video_sync)
    glXGetVideoSyncSGI (&x11_surface->glx_frame_counter);
}

static gboolean
gdk_x11_gl_context_glx_clear_current (GdkGLContext *context)
{
  GdkDisplay *display = gdk_gl_context_get_display (context);
  Display *dpy = gdk_x11_display_get_xdisplay (display);

  glXMakeContextCurrent (dpy, None, None, NULL);
  return TRUE;
}

static gboolean
gdk_x11_gl_context_glx_make_current (GdkGLContext *context,
                                     gboolean      surfaceless)
                                     
{
  GdkX11GLContextGLX *self = GDK_X11_GL_CONTEXT_GLX (context);
  GdkDisplay *display = gdk_gl_context_get_display (context);
  Display *dpy = gdk_x11_display_get_xdisplay (display);
  gboolean do_frame_sync = FALSE;
  GdkSurface *surface;
  GLXWindow drawable;

  if (!surfaceless)
    surface = gdk_gl_context_get_surface (context);
  else
    surface = GDK_X11_DISPLAY (display)->leader_gdk_surface;
  drawable = gdk_x11_surface_get_glx_drawable (surface);

  GDK_DISPLAY_NOTE (display, OPENGL,
                    g_message ("Making GLX context %p current to drawable %lu",
                               context, (unsigned long) drawable));

  if (!glXMakeContextCurrent (dpy, drawable, drawable, self->glx_context))
    return FALSE;

  if (!surfaceless && GDK_X11_DISPLAY (display)->has_glx_swap_interval)
    {
      /* If the WM is compositing there is no particular need to delay
       * the swap when drawing on the offscreen, rendering to the screen
       * happens later anyway, and its up to the compositor to sync that
       * to the vblank. */
      do_frame_sync = ! gdk_display_is_composited (display);

      if (do_frame_sync != self->do_frame_sync)
        {
          self->do_frame_sync = do_frame_sync;

          if (do_frame_sync)
            glXSwapIntervalSGI (1);
          else
            glXSwapIntervalSGI (0);
        }
    }

  return TRUE;
}

static cairo_region_t *
gdk_x11_gl_context_glx_get_damage (GdkGLContext *context)
{
  GdkDisplay *display = gdk_draw_context_get_display (GDK_DRAW_CONTEXT (context));
  GdkX11Display *display_x11 = GDK_X11_DISPLAY (display);
  Display *dpy = gdk_x11_display_get_xdisplay (display);
  unsigned int buffer_age = 0;

  if (display_x11->has_glx_buffer_age)
    {
      GdkX11GLContextGLX *self = GDK_X11_GL_CONTEXT_GLX (context);

      gdk_gl_context_make_current (context);
      glXQueryDrawable (dpy, gdk_x11_gl_context_glx_get_drawable (self),
                        GLX_BACK_BUFFER_AGE_EXT, &buffer_age);

      switch (buffer_age)
        {
          case 1:
            return cairo_region_create ();
            break;

          case 2:
            if (context->old_updated_area[0])
              return cairo_region_copy (context->old_updated_area[0]);
            break;

          case 3:
            if (context->old_updated_area[0] &&
                context->old_updated_area[1])
              {
                cairo_region_t *damage = cairo_region_copy (context->old_updated_area[0]);
                cairo_region_union (damage, context->old_updated_area[1]);
                return damage;
              }
            break;

          default:
            ;
        }
    }

  return GDK_GL_CONTEXT_CLASS (gdk_x11_gl_context_glx_parent_class)->get_damage (context);
}

static GLXContext
create_gl3_context (GdkDisplay   *display,
                    GLXFBConfig   config,
                    GdkGLContext *share,
                    int           profile,
                    int           flags,
                    int           major,
                    int           minor)
{
  int attrib_list[] = {
    GLX_CONTEXT_PROFILE_MASK_ARB, profile,
    GLX_CONTEXT_MAJOR_VERSION_ARB, major,
    GLX_CONTEXT_MINOR_VERSION_ARB, minor,
    GLX_CONTEXT_FLAGS_ARB, flags,
    None,
  };
  GLXContext res;

  GdkX11GLContextGLX *share_glx = NULL;

  if (share != NULL)
    share_glx = GDK_X11_GL_CONTEXT_GLX (share);

  gdk_x11_display_error_trap_push (display);

  res = glXCreateContextAttribsARB (gdk_x11_display_get_xdisplay (display),
                                    config,
                                    share_glx != NULL ? share_glx->glx_context : NULL,
                                    True,
                                    attrib_list);

  if (gdk_x11_display_error_trap_pop (display))
    return NULL;

  return res;
}

static GLXContext
create_legacy_context (GdkDisplay   *display,
                       GLXFBConfig   config,
                       GdkGLContext *share)
{
  GdkX11GLContextGLX *share_glx = NULL;
  GLXContext res;

  if (share != NULL)
    share_glx = GDK_X11_GL_CONTEXT_GLX (share);

  gdk_x11_display_error_trap_push (display);

  res = glXCreateNewContext (gdk_x11_display_get_xdisplay (display),
                             config,
                             GLX_RGBA_TYPE,
                             share_glx != NULL ? share_glx->glx_context : NULL,
                             TRUE);

  if (gdk_x11_display_error_trap_pop (display))
    return NULL;

  return res;
}

#ifdef HAVE_XDAMAGE
static void
bind_context_for_frame_fence (GdkX11GLContextGLX *self)
{
  GdkX11GLContextGLX *current_context_glx;
  GLXContext current_glx_context = NULL;
  GdkGLContext *current_context;
  gboolean needs_binding = TRUE;

  /* We don't care if the passed context is the current context,
   * necessarily, but we do care that *some* context that can
   * see the sync object is bound.
   *
   * If no context is bound at all, the GL dispatch layer will
   * make glClientWaitSync() silently return 0.
   */
  current_glx_context = glXGetCurrentContext ();

  if (current_glx_context == NULL)
    goto out;

  current_context = gdk_gl_context_get_current ();

  if (current_context == NULL)
    goto out;

  current_context_glx = GDK_X11_GL_CONTEXT_GLX (current_context);

  /* If the GLX context was changed out from under GDK, then
   * that context may not be one that is able to see the
   * created fence object.
   */
  if (current_context_glx->glx_context != current_glx_context)
    goto out;

  needs_binding = FALSE;

out:
  if (needs_binding)
    gdk_gl_context_make_current (GDK_GL_CONTEXT (self));
}

static void
finish_frame (GdkGLContext *context)
{
  GdkX11GLContextGLX *context_glx = GDK_X11_GL_CONTEXT_GLX (context);
  GdkSurface *surface = gdk_gl_context_get_surface (context);

  if (context_glx->xdamage == 0)
    return;

  if (context_glx->frame_fence == 0)
    return;

  glDeleteSync (context_glx->frame_fence);
  context_glx->frame_fence = 0;

  _gdk_x11_surface_set_frame_still_painting (surface, FALSE);
}

static gboolean
on_gl_surface_xevent (GdkGLContext   *context,
                      XEvent         *xevent,
                      GdkX11Display  *display_x11)
{
  GdkX11GLContextGLX *context_glx = GDK_X11_GL_CONTEXT_GLX (context);
  XDamageNotifyEvent *damage_xevent;

  if (xevent->type != (display_x11->damage_event_base + XDamageNotify))
    return FALSE;

  damage_xevent = (XDamageNotifyEvent *) xevent;

  if (damage_xevent->damage != context_glx->xdamage)
    return FALSE;

  if (context_glx->frame_fence)
    {
      GLenum wait_result;

      bind_context_for_frame_fence (context_glx);

      wait_result = glClientWaitSync (context_glx->frame_fence, 0, 0);

      switch (wait_result)
        {
          /* We assume that if the fence has been signaled, that this damage
           * event is the damage event that was triggered by the GL drawing
           * associated with the fence. That's, technically, not necessarly
           * always true. The X server could have generated damage for
           * an unrelated event (say the size of the window changing), at
           * just the right moment such that we're picking it up instead.
           *
           * We're choosing not to handle this edge case, but if it does ever
           * happen in the wild, it could lead to slight underdrawing by
           * the compositor for one frame. In the future, if we find out
           * this edge case is noticeable, we can compensate by copying the
           * painted region from gdk_x11_gl_context_end_frame and subtracting
           * damaged areas from the copy as they come in. Once the copied
           * region goes empty, we know that there won't be any underdraw,
           * and can mark painting has finished. It's not worth the added
           * complexity and resource usage to do this bookkeeping, however,
           * unless the problem is practically visible.
           */
          case GL_ALREADY_SIGNALED:
          case GL_CONDITION_SATISFIED:
          case GL_WAIT_FAILED:
            if (wait_result == GL_WAIT_FAILED)
              g_warning ("failed to wait on GL fence associated with last swap buffers call");
            finish_frame (context);
            break;

          /* We assume that if the fence hasn't been signaled, that this
           * damage event is not the damage event that was triggered by the
           * GL drawing associated with the fence. That's only true for
           * the Nvidia vendor driver. When using open source drivers, damage
           * is emitted immediately on swap buffers, before the fence ever
           * has a chance to signal.
           */
          case GL_TIMEOUT_EXPIRED:
            break;
          default:
            g_error ("glClientWaitSync returned unexpected result: %x", (guint) wait_result);
        }
    }

  return FALSE;
}

static void
on_surface_state_changed (GdkGLContext *context)
{
  GdkSurface *surface = gdk_gl_context_get_surface (context);

  if (GDK_SURFACE_IS_MAPPED (surface))
    return;

  /* If we're about to withdraw the surface, then we don't care if the frame is
   * still getting rendered by the GPU. The compositor is going to remove the surface
   * from the scene anyway, so wrap up the frame.
   */
  finish_frame (context);
}
#endif

static GdkGLAPI
gdk_x11_gl_context_glx_realize (GdkGLContext  *context,
                                GError       **error)
{
  GdkX11Display *display_x11;
  GdkDisplay *display;
  GdkX11GLContextGLX *context_glx;
  Display *dpy;
  GdkSurface *surface;
  GdkGLContext *share;
  gboolean debug_bit, compat_bit, legacy_bit;
  int major, minor, flags;
  GdkGLAPI api = 0;

  display = gdk_gl_context_get_display (context);
  dpy = gdk_x11_display_get_xdisplay (display);
  context_glx = GDK_X11_GL_CONTEXT_GLX (context);
  display_x11 = GDK_X11_DISPLAY (display);
  share = gdk_display_get_gl_context (display);
  surface = gdk_gl_context_get_surface (context);

  gdk_gl_context_get_required_version (context, &major, &minor);
  debug_bit = gdk_gl_context_get_debug_enabled (context);
  compat_bit = gdk_gl_context_get_forward_compatible (context);

  /* If there is no glXCreateContextAttribsARB() then we default to legacy */
  legacy_bit = !display_x11->has_glx_create_context || GDK_DISPLAY_DEBUG_CHECK (display, GL_LEGACY);

  /* We cannot share legacy contexts with core profile ones, so the
   * shared context is the one that decides if we're going to create
   * a legacy context or not.
   */
  if (share != NULL && gdk_gl_context_is_legacy (share))
    legacy_bit = TRUE;

  flags = 0;
  if (debug_bit)
    flags |= GLX_CONTEXT_DEBUG_BIT_ARB;
  if (compat_bit)
    flags |= GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;

  GDK_DISPLAY_NOTE (display, OPENGL,
            g_message ("Creating GLX context (GL version:%d.%d, debug:%s, forward:%s, legacy:%s, GL:%s, GLES:%s)",
                       major, minor,
                       debug_bit ? "yes" : "no",
                       compat_bit ? "yes" : "no",
                       legacy_bit ? "yes" : "no",
                       gdk_gl_context_is_api_allowed (context, GDK_GL_API_GL, NULL) ? "yes" : "no",
                       gdk_gl_context_is_api_allowed (context, GDK_GL_API_GLES, NULL) ? "yes" : "no"));

  /* If we have access to GLX_ARB_create_context_profile then we can ask for
   * a compatibility profile; if we don't, then we have to fall back to the
   * old GLX 1.3 API.
   */
  if (legacy_bit && !GDK_X11_DISPLAY (display)->has_glx_create_context)
    {
      GDK_DISPLAY_NOTE (display, OPENGL, g_message ("Creating legacy GL context on request"));
      /* do it below */
    }
  else
    {
      if (gdk_gl_context_is_api_allowed (context, GDK_GL_API_GL, NULL))
        {
          int profile = legacy_bit ? GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
                                   : GLX_CONTEXT_CORE_PROFILE_BIT_ARB;

          /* We need to tweak the version, otherwise we may end up requesting
           * a compatibility context with a minimum version of 3.2, which is
           * an error
           */
          if (legacy_bit)
            {
              major = 3;
              minor = 0;
            }

          GDK_DISPLAY_NOTE (display, OPENGL, g_message ("Creating GL3 context"));
          context_glx->glx_context = create_gl3_context (display,
                                                         display_x11->glx_config,
                                                         share,
                                                         profile,
                                                         flags, major, minor);
          api = GDK_GL_API_GL;
        }

      if (context_glx->glx_context == NULL && !legacy_bit &&
          gdk_gl_context_is_api_allowed (context, GDK_GL_API_GLES, NULL))
        {
          GDK_DISPLAY_NOTE (display, OPENGL, g_message ("Creating GL3 GLES context"));
          context_glx->glx_context = create_gl3_context (display,
                                                         display_x11->glx_config,
                                                         share,
                                                         GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
                                                         flags, major, minor);
          api = GDK_GL_API_GLES;
        }
    }

  /* Fall back to legacy in case the GL3 context creation failed */
  if (context_glx->glx_context == NULL && 
      gdk_gl_context_is_api_allowed (context, GDK_GL_API_GL, NULL))
    {
      GDK_DISPLAY_NOTE (display, OPENGL, g_message ("Creating fallback legacy context"));
      context_glx->glx_context = create_legacy_context (display, display_x11->glx_config, share);
      legacy_bit = TRUE;
      api = GDK_GL_API_GL;
    }

  if (context_glx->glx_context == NULL)
    {
      g_set_error_literal (error, GDK_GL_ERROR,
                           GDK_GL_ERROR_NOT_AVAILABLE,
                           _("Unable to create a GL context"));
      return 0;
    }

  /* Ensure that any other context is created with a legacy bit set */
  gdk_gl_context_set_is_legacy (context, legacy_bit);

  GDK_DISPLAY_NOTE (display, OPENGL,
            g_message ("Realized GLX context[%p], %s, version: %d.%d",
                       context_glx->glx_context,
                       glXIsDirect (dpy, context_glx->glx_context) ? "direct" : "indirect",
                       display_x11->glx_version / 10,
                       display_x11->glx_version % 10));

#ifdef HAVE_XDAMAGE
  if (display_x11->have_damage &&
      display_x11->has_async_glx_swap_buffers)
    {
      gdk_x11_display_error_trap_push (display);
      context_glx->xdamage = XDamageCreate (dpy,
                                            gdk_x11_surface_get_xid (surface),
                                            XDamageReportRawRectangles);
      if (gdk_x11_display_error_trap_pop (display))
        {
          context_glx->xdamage = 0;
        }
      else
        {
          g_signal_connect_object (G_OBJECT (display),
                                   "xevent",
                                   G_CALLBACK (on_gl_surface_xevent),
                                   context,
                                   G_CONNECT_SWAPPED);
          g_signal_connect_object (G_OBJECT (surface),
                                   "notify::state",
                                   G_CALLBACK (on_surface_state_changed),
                                   context,
                                   G_CONNECT_SWAPPED);

        }
    }
#endif

  return api;
}

static void
gdk_x11_gl_context_glx_dispose (GObject *gobject)
{
  GdkX11GLContextGLX *context_glx = GDK_X11_GL_CONTEXT_GLX (gobject);

#ifdef HAVE_XDAMAGE
  context_glx->xdamage = 0;
#endif

  if (context_glx->glx_context != NULL)
    {
      GdkGLContext *context = GDK_GL_CONTEXT (gobject);
      GdkDisplay *display = gdk_gl_context_get_display (context);
      Display *dpy = gdk_x11_display_get_xdisplay (display);

      if (glXGetCurrentContext () == context_glx->glx_context)
        glXMakeContextCurrent (dpy, None, None, NULL);

      GDK_DISPLAY_NOTE (display, OPENGL, g_message ("Destroying GLX context"));
      glXDestroyContext (dpy, context_glx->glx_context);
      context_glx->glx_context = NULL;
    }

  G_OBJECT_CLASS (gdk_x11_gl_context_glx_parent_class)->dispose (gobject);
}

static void
gdk_x11_gl_context_glx_class_init (GdkX11GLContextGLXClass *klass)
{
  GdkGLContextClass *context_class = GDK_GL_CONTEXT_CLASS (klass);
  GdkDrawContextClass *draw_context_class = GDK_DRAW_CONTEXT_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  context_class->backend_type = GDK_GL_GLX;

  context_class->realize = gdk_x11_gl_context_glx_realize;
  context_class->make_current = gdk_x11_gl_context_glx_make_current;
  context_class->clear_current = gdk_x11_gl_context_glx_clear_current;
  context_class->get_damage = gdk_x11_gl_context_glx_get_damage;

  draw_context_class->end_frame = gdk_x11_gl_context_glx_end_frame;

  gobject_class->dispose = gdk_x11_gl_context_glx_dispose;
}

static void
gdk_x11_gl_context_glx_init (GdkX11GLContextGLX *self)
{
  self->do_frame_sync = TRUE;
}

static gboolean
visual_is_rgba (XVisualInfo *visinfo)
{
  return
    visinfo->depth == 32 &&
    visinfo->visual->red_mask   == 0xff0000 &&
    visinfo->visual->green_mask == 0x00ff00 &&
    visinfo->visual->blue_mask  == 0x0000ff;
}

#define MAX_GLX_ATTRS   30

static gboolean
gdk_x11_display_create_glx_config (GdkX11Display  *self,
                                   Visual        **out_visual,
                                   int            *out_depth,
                                   GError        **error)
{
  GdkDisplay *display = GDK_DISPLAY (self);
  Display *dpy = gdk_x11_display_get_xdisplay (display);
  int attrs[MAX_GLX_ATTRS];
  GLXFBConfig *configs;
  int count;
  enum {
    NO_VISUAL_FOUND,
    WITH_MULTISAMPLING,
    WITH_STENCIL_AND_DEPTH_BUFFER,
    NO_ALPHA,
    NO_ALPHA_VISUAL,
    PERFECT
  } best_features;
  int i = 0;

  attrs[i++] = GLX_DRAWABLE_TYPE;
  attrs[i++] = GLX_WINDOW_BIT;

  attrs[i++] = GLX_RENDER_TYPE;
  attrs[i++] = GLX_RGBA_BIT;

  attrs[i++] = GLX_DOUBLEBUFFER;
  attrs[i++] = GL_TRUE;

  attrs[i++] = GLX_RED_SIZE;
  attrs[i++] = 1;
  attrs[i++] = GLX_GREEN_SIZE;
  attrs[i++] = 1;
  attrs[i++] = GLX_BLUE_SIZE;
  attrs[i++] = 1;
  attrs[i++] = GLX_ALPHA_SIZE;
  attrs[i++] = 1;

  attrs[i++] = None;
  g_assert (i < MAX_GLX_ATTRS);

  configs = glXChooseFBConfig (dpy, DefaultScreen (dpy), attrs, &count);
  if (configs == NULL || count == 0)
    {
      g_set_error_literal (error, GDK_GL_ERROR,
                           GDK_GL_ERROR_NOT_AVAILABLE,
                           _("No GLX configurations available"));
      return FALSE;
    }

  best_features = NO_VISUAL_FOUND;

  for (i = 0; i < count; i++)
    {
      XVisualInfo *visinfo;
      int tmp;

      visinfo = glXGetVisualFromFBConfig (dpy, configs[i]);
      if (visinfo == NULL)
        continue;

      if (glXGetFBConfigAttrib (dpy, configs[i], GLX_SAMPLE_BUFFERS_ARB, &tmp) != Success || tmp != 0)
        {
          if (best_features < WITH_MULTISAMPLING)
            {
              GDK_NOTE (OPENGL, g_message ("Best GLX config is %u for visual 0x%lX with multisampling", i, visinfo->visualid));
              best_features = WITH_MULTISAMPLING;
              *out_visual = visinfo->visual;
              *out_depth = visinfo->depth;
              self->glx_config = configs[i];
            }
          XFree (visinfo);
          continue;
        }

      if (glXGetFBConfigAttrib (dpy, configs[i], GLX_DEPTH_SIZE, &tmp) != Success || tmp != 0 ||
          glXGetFBConfigAttrib (dpy, configs[i], GLX_STENCIL_SIZE, &tmp) != Success || tmp != 0)
        {
          if (best_features < WITH_STENCIL_AND_DEPTH_BUFFER)
            {
              GDK_NOTE (OPENGL, g_message ("Best GLX config is %u for visual 0x%lX with a stencil or depth buffer", i, visinfo->visualid));
              best_features = WITH_STENCIL_AND_DEPTH_BUFFER;
              *out_visual = visinfo->visual;
              *out_depth = visinfo->depth;
              self->glx_config = configs[i];
            }
          XFree (visinfo);
          continue;
        }
    
      if (!visual_is_rgba (visinfo))
        {
          if (best_features < NO_ALPHA_VISUAL)
            {
              GDK_NOTE (OPENGL, g_message ("Best GLX config is %u for visual 0x%lX with no RGBA Visual", i, visinfo->visualid));
              best_features = NO_ALPHA_VISUAL;
              *out_visual = visinfo->visual;
              *out_depth = visinfo->depth;
              self->glx_config = configs[i];
            }
          XFree (visinfo);
          continue;
        }

      GDK_NOTE (OPENGL, g_message ("GLX config %u for visual 0x%lX is the perfect choice", i, visinfo->visualid));
      best_features = PERFECT;
      *out_visual = visinfo->visual;
      *out_depth = visinfo->depth;
      self->glx_config = configs[i];
      XFree (visinfo);
      break;
    }

  XFree (configs);

  if (best_features == NO_VISUAL_FOUND)
    {
      g_set_error_literal (error, GDK_GL_ERROR,
                           GDK_GL_ERROR_NOT_AVAILABLE,
                           _("No GLX configuration with required features found"));
      return FALSE;
    }

  return TRUE;
}

#undef MAX_GLX_ATTRS

/**
 * gdk_x11_display_get_glx_version:
 * @display: (type GdkX11Display): a `GdkDisplay`
 * @major: (out): return location for the GLX major version
 * @minor: (out): return location for the GLX minor version
 *
 * Retrieves the version of the GLX implementation.
 *
 * Returns: %TRUE if GLX is available
 */
gboolean
gdk_x11_display_get_glx_version (GdkDisplay *display,
                                 int        *major,
                                 int        *minor)
{
  g_return_val_if_fail (GDK_IS_DISPLAY (display), FALSE);

  if (!GDK_IS_X11_DISPLAY (display))
    return FALSE;

  GdkX11Display *display_x11 = GDK_X11_DISPLAY (display);

  if (display_x11->glx_config == NULL)
    return FALSE;

  if (major != NULL)
    *major = display_x11->glx_version / 10;
  if (minor != NULL)
    *minor = display_x11->glx_version % 10;

  return TRUE;
}

/*< private >
 * gdk_x11_display_init_glx:
 * @display_x11: an X11 display that has not been inited yet. 
 * @out_visual: set to the Visual to be used with the returned config
 * @out_depth: set to the depth to be used with the returned config
 * @error: Return location for error
 *
 * Initializes the cached GLX state for the given @screen.
 *
 * This function must be called exactly once during initialization.
 *
 * Returns: %TRUE if GLX was initialized
 */
gboolean
gdk_x11_display_init_glx (GdkX11Display  *display_x11,
                          Visual        **out_visual,
                          int            *out_depth,
                          GError        **error)
{
  GdkDisplay *display = GDK_DISPLAY (display_x11);
  Display *dpy;
  int screen_num;

  if (!gdk_gl_backend_can_be_used (GDK_GL_GLX, error))
    return FALSE;

  dpy = gdk_x11_display_get_xdisplay (display);

  if (!epoxy_has_glx (dpy))
    {
      g_set_error_literal (error, GDK_GL_ERROR,
                           GDK_GL_ERROR_NOT_AVAILABLE,
                           _("GLX is not supported"));
      return FALSE;
    }

  screen_num = display_x11->screen->screen_num;

  display_x11->glx_version = epoxy_glx_version (dpy, screen_num);

  display_x11->has_glx_create_context =
    epoxy_has_glx_extension (dpy, screen_num, "GLX_ARB_create_context_profile");
  display_x11->has_glx_create_es2_context =
    epoxy_has_glx_extension (dpy, screen_num, "GLX_EXT_create_context_es2_profile");
  display_x11->has_glx_swap_interval =
    epoxy_has_glx_extension (dpy, screen_num, "GLX_SGI_swap_control");
  display_x11->has_glx_texture_from_pixmap =
    epoxy_has_glx_extension (dpy, screen_num, "GLX_EXT_texture_from_pixmap");
  display_x11->has_glx_video_sync =
    epoxy_has_glx_extension (dpy, screen_num, "GLX_SGI_video_sync");
  display_x11->has_glx_buffer_age =
    epoxy_has_glx_extension (dpy, screen_num, "GLX_EXT_buffer_age");
  display_x11->has_glx_sync_control =
    epoxy_has_glx_extension (dpy, screen_num, "GLX_OML_sync_control");
  display_x11->has_glx_multisample =
    epoxy_has_glx_extension (dpy, screen_num, "GLX_ARB_multisample");
  display_x11->has_glx_visual_rating =
    epoxy_has_glx_extension (dpy, screen_num, "GLX_EXT_visual_rating");

  if (g_strcmp0 (glXGetClientString (dpy, GLX_VENDOR), "NVIDIA Corporation") == 0)
    {
      Atom type;
      int format;
      gulong nitems;
      gulong bytes_after;
      guchar *data = NULL;

      /* With the mesa based drivers, we can safely assume the compositor can
       * access the updated surface texture immediately after glXSwapBuffers is
       * run, because the kernel ensures there is an implicit synchronization
       * operation upon texture access. This is not true with the Nvidia vendor
       * driver. There is a window of time after glXSwapBuffers before other
       * processes can see the updated drawing. We need to take special care,
       * in that case, to defer telling the compositor our latest frame is
       * ready until after the GPU has completed all issued commands related
       * to the frame, and that the X server says the frame has been drawn.
       *
       * As this can cause deadlocks, we want to make sure to only enable it for Xorg,
       * but not for XWayland, Xnest or whatever other X servers exist.
       */

      gdk_x11_display_error_trap_push (display);
      if (XGetWindowProperty (dpy, DefaultRootWindow (dpy),
                              gdk_x11_get_xatom_by_name_for_display (display, "XFree86_VT"),
                              0, 1, False, AnyPropertyType,
                              &type, &format, &nitems, &bytes_after, &data) == Success)
        {
          if (type != None)
            display_x11->has_async_glx_swap_buffers = TRUE;
        }
      gdk_x11_display_error_trap_pop_ignored (display);

      if (data)
        XFree (data);
    }

  if (!gdk_x11_display_create_glx_config (display_x11, out_visual, out_depth, error))
    return FALSE;

  GDK_DISPLAY_NOTE (display, OPENGL,
            g_message ("GLX version %d.%d found\n"
                       " - Vendor: %s\n"
                       " - Checked extensions:\n"
                       "\t* GLX_ARB_create_context_profile: %s\n"
                       "\t* GLX_EXT_create_context_es2_profile: %s\n"
                       "\t* GLX_SGI_swap_control: %s\n"
                       "\t* GLX_EXT_texture_from_pixmap: %s\n"
                       "\t* GLX_SGI_video_sync: %s\n"
                       "\t* GLX_EXT_buffer_age: %s\n"
                       "\t* GLX_OML_sync_control: %s"
                       "\t* GLX_ARB_multisample: %s"
                       "\t* GLX_EXT_visual_rating: %s",
                     display_x11->glx_version / 10,
                     display_x11->glx_version % 10,
                     glXGetClientString (dpy, GLX_VENDOR),
                     display_x11->has_glx_create_context ? "yes" : "no",
                     display_x11->has_glx_create_es2_context ? "yes" : "no",
                     display_x11->has_glx_swap_interval ? "yes" : "no",
                     display_x11->has_glx_texture_from_pixmap ? "yes" : "no",
                     display_x11->has_glx_video_sync ? "yes" : "no",
                     display_x11->has_glx_buffer_age ? "yes" : "no",
                     display_x11->has_glx_sync_control ? "yes" : "no",
                     display_x11->has_glx_multisample ? "yes" : "no",
                     display_x11->has_glx_visual_rating ? "yes" : "no"));

  return TRUE;
}
