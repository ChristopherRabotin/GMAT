/* simple.c
 * Copyright (C) 1997  Red Hat, Inc
 * Author: Elliot Lee
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
#include <gtk/gtk.h>


void
hello (void)
{
  g_print ("hello world\n");
}

int
main (int argc, char *argv[])
{
  GtkWidget *window;

  gtk_init (&argc, &argv);

  window = g_object_connect (g_object_new (gtk_window_get_type (),
                                           "type", GTK_WINDOW_TOPLEVEL,
                                           "title", "hello world",
                                           "resizable", FALSE,
                                           "border_width", 10,
                                           NULL),
                             "signal::destroy", gtk_main_quit, NULL,
                             NULL);
  g_object_connect (g_object_new (gtk_button_get_type (),
                                  "GtkButton::label", "hello world",
                                  "GtkWidget::parent", window,
                                  "GtkWidget::visible", TRUE,
                                  NULL),
                    "signal::clicked", hello, NULL,
                    NULL);
  gtk_widget_show (window);

  gtk_main ();

  return 0;
}
