/* gtktextviewprivate.h: Private header for GtkTextView
 *
 * Copyright (c) 2016  Emmanuele Bassi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GTK_TEXT_VIEW_PRIVATE_H__
#define __GTK_TEXT_VIEW_PRIVATE_H__

#include "gtktextview.h"
#include "gtktextattributes.h"
#include "gtkcssnodeprivate.h"

G_BEGIN_DECLS

GtkCssNode *    gtk_text_view_get_text_node             (GtkTextView *text_view);
GtkCssNode *    gtk_text_view_get_selection_node        (GtkTextView *text_view);

GtkTextAttributes * gtk_text_view_get_default_attributes (GtkTextView *text_view);

GtkEventController *gtk_text_view_get_key_controller    (GtkTextView *text_view);


G_END_DECLS

#endif /* __GTK_TEXT_VIEW_PRIVATE_H__ */
