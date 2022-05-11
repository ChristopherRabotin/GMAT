/*
 * Copyright (c) 2008-2009  Christian Hammond
 * Copyright (c) 2008-2009  David Trowbridge
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "config.h"
#include <glib/gi18n-lib.h>

#include "window.h"
#include "object-tree.h"

#include "gtkstack.h"
#include "gtkmain.h"
#include "gtkinvisible.h"

typedef struct
{
  gint x;
  gint y;
  gboolean found;
  gboolean first;
  GtkWidget *res_widget;
} FindWidgetData;

static void
find_widget (GtkWidget      *widget,
             FindWidgetData *data)
{
  GtkAllocation new_allocation;
  gint x_offset = 0;
  gint y_offset = 0;

  gtk_widget_get_allocation (widget, &new_allocation);

  if (data->found || !gtk_widget_get_mapped (widget))
    return;

  /* Note that in the following code, we only count the
   * position as being inside a WINDOW widget if it is inside
   * widget->window; points that are outside of widget->window
   * but within the allocation are not counted. This is consistent
   * with the way we highlight drag targets.
   */
  if (gtk_widget_get_has_window (widget))
    {
      new_allocation.x = 0;
      new_allocation.y = 0;
    }

  if (gtk_widget_get_parent (widget) && !data->first)
    {
      GdkWindow *window;

      window = gtk_widget_get_window (widget);
      while (window != gtk_widget_get_window (gtk_widget_get_parent (widget)))
        {
          gint tx, ty, twidth, theight;

          if (window == NULL)
            return;

          twidth = gdk_window_get_width (window);
          theight = gdk_window_get_height (window);

          if (new_allocation.x < 0)
            {
              new_allocation.width += new_allocation.x;
              new_allocation.x = 0;
            }
          if (new_allocation.y < 0)
            {
              new_allocation.height += new_allocation.y;
              new_allocation.y = 0;
            }
          if (new_allocation.x + new_allocation.width > twidth)
            new_allocation.width = twidth - new_allocation.x;
          if (new_allocation.y + new_allocation.height > theight)
            new_allocation.height = theight - new_allocation.y;

          gdk_window_get_position (window, &tx, &ty);
          new_allocation.x += tx;
          x_offset += tx;
          new_allocation.y += ty;
          y_offset += ty;

          window = gdk_window_get_parent (window);
        }
    }

  if ((data->x >= new_allocation.x) && (data->y >= new_allocation.y) &&
      (data->x < new_allocation.x + new_allocation.width) &&
      (data->y < new_allocation.y + new_allocation.height))
    {
      /* First, check if the drag is in a valid drop site in
       * one of our children 
       */
      if (GTK_IS_CONTAINER (widget))
        {
          FindWidgetData new_data = *data;

          new_data.x -= x_offset;
          new_data.y -= y_offset;
          new_data.found = FALSE;
          new_data.first = FALSE;

          gtk_container_forall (GTK_CONTAINER (widget),
                                (GtkCallback)find_widget,
                                &new_data);

          data->found = new_data.found;
          if (data->found)
            data->res_widget = new_data.res_widget;
        }

      /* If not, and this widget is registered as a drop site, check to
       * emit "drag_motion" to check if we are actually in
       * a drop site.
       */
      if (!data->found)
        {
          data->found = TRUE;
          data->res_widget = widget;
        }
    }
}

static GtkWidget *
find_widget_at_pointer (GdkDevice *device)
{
  GtkWidget *widget = NULL;
  GdkWindow *pointer_window;
  gint x, y;
  FindWidgetData data;

  pointer_window = gdk_device_get_window_at_position (device, NULL, NULL);

  if (pointer_window)
    {
      gpointer widget_ptr;

      gdk_window_get_user_data (pointer_window, &widget_ptr);
      widget = widget_ptr;
    }

  if (widget)
    {
      gdk_window_get_device_position (gtk_widget_get_window (widget),
                                      device, &x, &y, NULL);

      data.x = x;
      data.y = y;
      data.found = FALSE;
      data.first = TRUE;

      find_widget (widget, &data);
      if (data.found)
        return data.res_widget;

      return widget;
    }

  return NULL;
}

static gboolean draw_flash (GtkWidget          *widget,
                            cairo_t            *cr,
                            GtkInspectorWindow *iw);

static void
clear_flash (GtkInspectorWindow *iw)
{
  if (iw->flash_widget)
    {
      gtk_widget_queue_draw (iw->flash_widget);
      g_signal_handlers_disconnect_by_func (iw->flash_widget, draw_flash, iw);
      g_signal_handlers_disconnect_by_func (iw->flash_widget, clear_flash, iw);
      iw->flash_widget = NULL;
    }
}

static void
start_flash (GtkInspectorWindow *iw,
             GtkWidget          *widget)
{
  clear_flash (iw);

  iw->flash_count = 1;
  iw->flash_widget = widget;
  g_signal_connect_after (widget, "draw", G_CALLBACK (draw_flash), iw);
  g_signal_connect_swapped (widget, "unmap", G_CALLBACK (clear_flash), iw);
  gtk_widget_queue_draw (widget);
}

static void
select_widget (GtkInspectorWindow *iw,
               GtkWidget          *widget)
{
  GtkInspectorObjectTree *wt = GTK_INSPECTOR_OBJECT_TREE (iw->object_tree);

  iw->selected_widget = widget;

  if (!gtk_inspector_object_tree_select_object (wt, G_OBJECT (widget)))
    {
      gtk_inspector_object_tree_scan (wt, gtk_widget_get_toplevel (widget));
      gtk_inspector_object_tree_select_object (wt, G_OBJECT (widget));
    }
}

static void
on_inspect_widget (GtkWidget          *button,
                   GdkEvent           *event,
                   GtkInspectorWindow *iw)
{
  GtkWidget *widget;

  gdk_window_raise (gtk_widget_get_window (GTK_WIDGET (iw)));

  clear_flash (iw);

  widget = find_widget_at_pointer (gdk_event_get_device (event));

  if (widget)
    select_widget (iw, widget);
}

static void
on_highlight_widget (GtkWidget          *button,
                     GdkEvent           *event,
                     GtkInspectorWindow *iw)
{
  GtkWidget *widget;

  widget = find_widget_at_pointer (gdk_event_get_device (event));

  if (widget == NULL)
    {
      /* This window isn't in-process. Ignore it. */
      return;
    }

  if (gtk_widget_get_toplevel (widget) == GTK_WIDGET (iw))
    {
      /* Don't hilight things in the inspector window */
      return;
    }

  if (iw->flash_widget == widget)
    {
      /* Already selected */
      return;
    }

  clear_flash (iw);
  start_flash (iw, widget);
}

static void
deemphasize_window (GtkWidget *window)
{
  GdkScreen *screen;

  screen = gtk_widget_get_screen (window);
  if (gdk_screen_is_composited (screen) &&
      gtk_widget_get_visual (window) == gdk_screen_get_rgba_visual (screen))
    {
      cairo_rectangle_int_t rect;
      cairo_region_t *region;

      gtk_widget_set_opacity (window, 0.3);
      rect.x = rect.y = rect.width = rect.height = 0;
      region = cairo_region_create_rectangle (&rect);
      gtk_widget_input_shape_combine_region (window, region);
      cairo_region_destroy (region);
    }
  else
    gdk_window_lower (gtk_widget_get_window (window));
}

static void
reemphasize_window (GtkWidget *window)
{
  GdkScreen *screen;

  screen = gtk_widget_get_screen (window);
  if (gdk_screen_is_composited (screen) &&
      gtk_widget_get_visual (window) == gdk_screen_get_rgba_visual (screen))
    {
      gtk_widget_set_opacity (window, 1.0);
      gtk_widget_input_shape_combine_region (window, NULL);
    }
  else
    gdk_window_raise (gtk_widget_get_window (window));
}

static gboolean
property_query_event (GtkWidget *widget,
                      GdkEvent  *event,
                      gpointer   data)
{
  GtkInspectorWindow *iw = (GtkInspectorWindow *)data;

  if (event->type == GDK_BUTTON_RELEASE)
    {
      g_signal_handlers_disconnect_by_func (widget, property_query_event, data);
      gtk_grab_remove (widget);
      if (iw->grabbed)
        gdk_seat_ungrab (gdk_event_get_seat (event));
      reemphasize_window (GTK_WIDGET (iw));

      on_inspect_widget (widget, event, data);
    }
  else if (event->type == GDK_MOTION_NOTIFY)
    {
      on_highlight_widget (widget, event, data);
    }
  else if (event->type == GDK_KEY_PRESS)
    {
      GdkEventKey *ke = (GdkEventKey*)event;

      if (ke->keyval == GDK_KEY_Escape)
        {
          g_signal_handlers_disconnect_by_func (widget, property_query_event, data);
          gtk_grab_remove (widget);
          if (iw->grabbed)
            gdk_seat_ungrab (gdk_event_get_seat (event));
          reemphasize_window (GTK_WIDGET (iw));

          clear_flash (iw);
        }
    }

  return TRUE;
}

void
gtk_inspector_on_inspect (GtkWidget          *button,
                          GtkInspectorWindow *iw)
{
  GdkDisplay *display;
  GdkCursor *cursor;
  GdkGrabStatus status;

  if (!iw->invisible)
    {
      iw->invisible = gtk_invisible_new_for_screen (gdk_screen_get_default ());
      gtk_widget_add_events (iw->invisible,
                             GDK_POINTER_MOTION_MASK |
                             GDK_BUTTON_PRESS_MASK |
                             GDK_BUTTON_RELEASE_MASK |
                             GDK_KEY_PRESS_MASK |
                             GDK_KEY_RELEASE_MASK);
      gtk_widget_realize (iw->invisible);
      gtk_widget_show (iw->invisible);
    }

  display = gdk_display_get_default ();
  cursor = gdk_cursor_new_from_name (display, "crosshair");
  status = gdk_seat_grab (gdk_display_get_default_seat (display),
                          gtk_widget_get_window (iw->invisible),
                          GDK_SEAT_CAPABILITY_ALL_POINTING, TRUE,
                          cursor, NULL, NULL, NULL);
  g_object_unref (cursor);
  iw->grabbed = status == GDK_GRAB_SUCCESS;

  g_signal_connect (iw->invisible, "event", G_CALLBACK (property_query_event), iw);

  gtk_grab_add (GTK_WIDGET (iw->invisible));
  deemphasize_window (GTK_WIDGET (iw));
}

static gboolean
draw_flash (GtkWidget          *widget,
            cairo_t            *cr,
            GtkInspectorWindow *iw)
{
  GtkAllocation alloc;

  if (iw && iw->flash_count % 2 == 0)
    return FALSE;

  if (GTK_IS_WINDOW (widget))
    {
      GtkWidget *child = gtk_bin_get_child (GTK_BIN (widget));
      /* We don't want to draw the drag highlight around the
       * CSD window decorations
       */
      if (child == NULL)
        return FALSE;

      gtk_widget_get_allocation (child, &alloc);
    }
  else
    {
      alloc.x = 0;
      alloc.y = 0;
      alloc.width = gtk_widget_get_allocated_width (widget);
      alloc.height = gtk_widget_get_allocated_height (widget);
    }

  cairo_set_source_rgba (cr, 0.0, 0.0, 1.0, 0.2);
  cairo_rectangle (cr,
                   alloc.x + 0.5, alloc.y + 0.5,
                   alloc.width - 1, alloc.height - 1);
  cairo_fill (cr);

  return FALSE;
}

static gboolean
on_flash_timeout (GtkInspectorWindow *iw)
{
  gtk_widget_queue_draw (iw->flash_widget);

  iw->flash_count++;

  if (iw->flash_count == 6)
    {
      g_signal_handlers_disconnect_by_func (iw->flash_widget, draw_flash, iw);
      g_signal_handlers_disconnect_by_func (iw->flash_widget, clear_flash, iw);
      iw->flash_widget = NULL;
      iw->flash_cnx = 0;

      return G_SOURCE_REMOVE;
    }

  return G_SOURCE_CONTINUE;
}

void
gtk_inspector_flash_widget (GtkInspectorWindow *iw,
                            GtkWidget          *widget)
{
  if (!gtk_widget_get_visible (widget) || !gtk_widget_get_mapped (widget))
    return;

  if (iw->flash_cnx != 0)
    {
      g_source_remove (iw->flash_cnx);
      iw->flash_cnx = 0;
    }

  start_flash (iw, widget);
  iw->flash_cnx = g_timeout_add (150, (GSourceFunc) on_flash_timeout, iw);
}

void
gtk_inspector_start_highlight (GtkWidget *widget)
{
  g_signal_connect_after (widget, "draw", G_CALLBACK (draw_flash), NULL);
  gtk_widget_queue_draw (widget);
}

void
gtk_inspector_stop_highlight (GtkWidget *widget)
{
  g_signal_handlers_disconnect_by_func (widget, draw_flash, NULL);
  g_signal_handlers_disconnect_by_func (widget, clear_flash, NULL);
  gtk_widget_queue_draw (widget);
}

void
gtk_inspector_window_select_widget_under_pointer (GtkInspectorWindow *iw)
{
  GdkDisplay *display;
  GdkDevice *device;
  GtkWidget *widget;

  display = gdk_display_get_default ();
  device = gdk_seat_get_pointer (gdk_display_get_default_seat (display));

  widget = find_widget_at_pointer (device);

  if (widget)
    select_widget (iw, widget);
}

/* vim: set et sw=2 ts=2: */
