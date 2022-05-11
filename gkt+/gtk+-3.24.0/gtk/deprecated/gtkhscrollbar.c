/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 * Copyright (C) 2001 Red Hat, Inc.
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

#include "config.h"

#include "gtkhscrollbar.h"

#include "gtkadjustment.h"
#include "gtkintl.h"
#include "gtkorientable.h"
#include "gtkscrollbar.h"

G_GNUC_BEGIN_IGNORE_DEPRECATIONS

/**
 * SECTION:gtkhscrollbar
 * @Short_description: A horizontal scrollbar
 * @Title: GtkHScrollbar
 * @See_also: #GtkScrollbar, #GtkScrolledWindow
 *
 * The #GtkHScrollbar widget is a widget arranged horizontally creating a
 * scrollbar. See #GtkScrollbar for details on
 * scrollbars. #GtkAdjustment pointers may be added to handle the
 * adjustment of the scrollbar or it may be left %NULL in which case one
 * will be created for you. See #GtkScrollbar for a description of what the
 * fields in an adjustment represent for a scrollbar.
 *
 * GtkHScrollbar has been deprecated, use #GtkScrollbar instead.
 */


G_DEFINE_TYPE (GtkHScrollbar, gtk_hscrollbar, GTK_TYPE_SCROLLBAR)

static void
gtk_hscrollbar_class_init (GtkHScrollbarClass *class)
{
  GTK_RANGE_CLASS (class)->stepper_detail = "hscrollbar";
}

static void
gtk_hscrollbar_init (GtkHScrollbar *hscrollbar)
{
  gtk_orientable_set_orientation (GTK_ORIENTABLE (hscrollbar),
                                  GTK_ORIENTATION_HORIZONTAL);
}

/**
 * gtk_hscrollbar_new:
 * @adjustment: (allow-none): the #GtkAdjustment to use, or %NULL to create a new adjustment
 *
 * Creates a new horizontal scrollbar.
 *
 * Returns: the new #GtkHScrollbar
 *
 * Deprecated: 3.2: Use gtk_scrollbar_new() with %GTK_ORIENTATION_HORIZONTAL instead
 */
GtkWidget *
gtk_hscrollbar_new (GtkAdjustment *adjustment)
{
  g_return_val_if_fail (adjustment == NULL || GTK_IS_ADJUSTMENT (adjustment),
                        NULL);

  return g_object_new (GTK_TYPE_HSCROLLBAR,
                       "adjustment", adjustment,
                       NULL);
}
