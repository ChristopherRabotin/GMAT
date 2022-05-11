/* GTK - The GIMP Toolkit
 * Copyright (C) 2011 Benjamin Otte <otte@gnome.org>
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

#include "config.h"

#include "gtkroundedboxprivate.h"

#include "gtkcsscornervalueprivate.h"
#include "gtkcsstypesprivate.h"
#include "gtkstylecontextprivate.h"

#include <string.h>

/**
 * _gtk_rounded_box_init_rect:
 * @box: box to initialize
 * @x: x coordinate of box
 * @y: y coordinate of box
 * @width: width of box
 * @height: height of box
 *
 * Initializes the given @box to represent the given rectangle.
 * The
 **/
void
_gtk_rounded_box_init_rect (GtkRoundedBox *box,
                            double         x,
                            double         y,
                            double         width,
                            double         height)
{
  memset (box, 0, sizeof (GtkRoundedBox));

  box->box.x = x;
  box->box.y = y;
  box->box.width = width;
  box->box.height = height;
}

/* clamp border radius, following CSS specs */
static void
gtk_rounded_box_clamp_border_radius (GtkRoundedBox *box)
{
  gdouble factor = 1.0;
  gdouble corners;

  corners = box->corner[GTK_CSS_TOP_LEFT].horizontal + box->corner[GTK_CSS_TOP_RIGHT].horizontal;
  if (corners != 0)
    factor = MIN (factor, box->box.width / corners);

  corners = box->corner[GTK_CSS_TOP_RIGHT].vertical + box->corner[GTK_CSS_BOTTOM_RIGHT].vertical;
  if (corners != 0)
    factor = MIN (factor, box->box.height / corners);

  corners = box->corner[GTK_CSS_BOTTOM_RIGHT].horizontal + box->corner[GTK_CSS_BOTTOM_LEFT].horizontal;
  if (corners != 0)
    factor = MIN (factor, box->box.width / corners);

  corners = box->corner[GTK_CSS_TOP_LEFT].vertical + box->corner[GTK_CSS_BOTTOM_LEFT].vertical;
  if (corners != 0)
    factor = MIN (factor, box->box.height / corners);

  box->corner[GTK_CSS_TOP_LEFT].horizontal *= factor;
  box->corner[GTK_CSS_TOP_LEFT].vertical *= factor;
  box->corner[GTK_CSS_TOP_RIGHT].horizontal *= factor;
  box->corner[GTK_CSS_TOP_RIGHT].vertical *= factor;
  box->corner[GTK_CSS_BOTTOM_RIGHT].horizontal *= factor;
  box->corner[GTK_CSS_BOTTOM_RIGHT].vertical *= factor;
  box->corner[GTK_CSS_BOTTOM_LEFT].horizontal *= factor;
  box->corner[GTK_CSS_BOTTOM_LEFT].vertical *= factor;
}

static void
_gtk_rounded_box_apply_border_radius (GtkRoundedBox *box,
                                      GtkCssValue **corner,
                                      GtkJunctionSides junction)
{
  if (corner[GTK_CSS_TOP_LEFT] && (junction & GTK_JUNCTION_CORNER_TOPLEFT) == 0)
    {
      box->corner[GTK_CSS_TOP_LEFT].horizontal = _gtk_css_corner_value_get_x (corner[GTK_CSS_TOP_LEFT],
                                                                              box->box.width);
      box->corner[GTK_CSS_TOP_LEFT].vertical = _gtk_css_corner_value_get_y (corner[GTK_CSS_TOP_LEFT],
                                                                            box->box.height);
    }
  if (corner[GTK_CSS_TOP_RIGHT] && (junction & GTK_JUNCTION_CORNER_TOPRIGHT) == 0)
    {
      box->corner[GTK_CSS_TOP_RIGHT].horizontal = _gtk_css_corner_value_get_x (corner[GTK_CSS_TOP_RIGHT],
                                                                               box->box.width);
      box->corner[GTK_CSS_TOP_RIGHT].vertical = _gtk_css_corner_value_get_y (corner[GTK_CSS_TOP_RIGHT],
                                                                             box->box.height);
    }
  if (corner[GTK_CSS_BOTTOM_RIGHT] && (junction & GTK_JUNCTION_CORNER_BOTTOMRIGHT) == 0)
    {
      box->corner[GTK_CSS_BOTTOM_RIGHT].horizontal = _gtk_css_corner_value_get_x (corner[GTK_CSS_BOTTOM_RIGHT],
                                                                                  box->box.width);
      box->corner[GTK_CSS_BOTTOM_RIGHT].vertical = _gtk_css_corner_value_get_y (corner[GTK_CSS_BOTTOM_RIGHT],
                                                                                box->box.height);
    }
  if (corner[GTK_CSS_BOTTOM_LEFT] && (junction & GTK_JUNCTION_CORNER_BOTTOMLEFT) == 0)
    {
      box->corner[GTK_CSS_BOTTOM_LEFT].horizontal = _gtk_css_corner_value_get_x (corner[GTK_CSS_BOTTOM_LEFT],
                                                                                 box->box.width);
      box->corner[GTK_CSS_BOTTOM_LEFT].vertical = _gtk_css_corner_value_get_y (corner[GTK_CSS_BOTTOM_LEFT],
                                                                               box->box.height);
    }

  gtk_rounded_box_clamp_border_radius (box);
}

void
_gtk_rounded_box_apply_border_radius_for_style (GtkRoundedBox    *box,
                                                GtkCssStyle      *style,
                                                GtkJunctionSides  junction)
{
  GtkCssValue *corner[4];

  corner[GTK_CSS_TOP_LEFT] = gtk_css_style_get_value (style, GTK_CSS_PROPERTY_BORDER_TOP_LEFT_RADIUS);
  corner[GTK_CSS_TOP_RIGHT] = gtk_css_style_get_value (style, GTK_CSS_PROPERTY_BORDER_TOP_RIGHT_RADIUS);
  corner[GTK_CSS_BOTTOM_LEFT] = gtk_css_style_get_value (style, GTK_CSS_PROPERTY_BORDER_BOTTOM_LEFT_RADIUS);
  corner[GTK_CSS_BOTTOM_RIGHT] = gtk_css_style_get_value (style, GTK_CSS_PROPERTY_BORDER_BOTTOM_RIGHT_RADIUS);

  _gtk_rounded_box_apply_border_radius (box, corner, junction);
}

void
_gtk_rounded_box_apply_outline_radius_for_style (GtkRoundedBox    *box,
                                                 GtkCssStyle      *style,
                                                 GtkJunctionSides  junction)
{
  GtkCssValue *corner[4];

  corner[GTK_CSS_TOP_LEFT] = gtk_css_style_get_value (style, GTK_CSS_PROPERTY_OUTLINE_TOP_LEFT_RADIUS);
  corner[GTK_CSS_TOP_RIGHT] = gtk_css_style_get_value (style, GTK_CSS_PROPERTY_OUTLINE_TOP_RIGHT_RADIUS);
  corner[GTK_CSS_BOTTOM_LEFT] = gtk_css_style_get_value (style, GTK_CSS_PROPERTY_OUTLINE_BOTTOM_LEFT_RADIUS);
  corner[GTK_CSS_BOTTOM_RIGHT] = gtk_css_style_get_value (style, GTK_CSS_PROPERTY_OUTLINE_BOTTOM_RIGHT_RADIUS);

  _gtk_rounded_box_apply_border_radius (box, corner, junction);
}

static void
gtk_css_border_radius_grow (GtkRoundedBoxCorner *corner,
                            double               horizontal,
                            double               vertical)
{
  if (corner->horizontal)
    corner->horizontal += horizontal;
  if (corner->vertical)
    corner->vertical += vertical;

  if (corner->horizontal <= 0 || corner->vertical <= 0)
    {
      corner->horizontal = 0;
      corner->vertical = 0;
    }
}

void
_gtk_rounded_box_grow (GtkRoundedBox *box,
                       double         top,
                       double         right,
                       double         bottom,
                       double         left)
{
  if (box->box.width + left + right < 0)
    {
      box->box.x -= left * box->box.width / (left + right);
      box->box.width = 0;
    }
  else
    {
      box->box.x -= left;
      box->box.width += left + right;
    }

  if (box->box.height + bottom + top < 0)
    {
      box->box.y -= top * box->box.height / (top + bottom);
      box->box.height = 0;
    }
  else
    {
      box->box.y -= top;
      box->box.height += top + bottom;
    }

  gtk_css_border_radius_grow (&box->corner[GTK_CSS_TOP_LEFT], left, top);
  gtk_css_border_radius_grow (&box->corner[GTK_CSS_TOP_RIGHT], right, top);
  gtk_css_border_radius_grow (&box->corner[GTK_CSS_BOTTOM_RIGHT], right, bottom);
  gtk_css_border_radius_grow (&box->corner[GTK_CSS_BOTTOM_LEFT], left, bottom);
}

void
_gtk_rounded_box_shrink (GtkRoundedBox *box,
                         double         top,
                         double         right,
                         double         bottom,
                         double         left)
{
  _gtk_rounded_box_grow (box, -top, -right, -bottom, -left);
}

void
_gtk_rounded_box_move (GtkRoundedBox *box,
                       double         dx,
                       double         dy)
{
  box->box.x += dx;
  box->box.y += dy;
}

typedef struct {
  double angle1;
  double angle2;
  gboolean negative;
} Arc;

static inline guint
mem_hash (gconstpointer v, gint len)
{
  const signed char *p;
  const signed char *end;
  guint32 h = 5381;

  p = v;
  end = p + len;
  for (;  p < end; p++)
    h = (h << 5) + h + *p;

  return h;
}

static guint
arc_path_hash (Arc *arc)
{
  return mem_hash ((gconstpointer)arc, sizeof (Arc));
}

static gboolean
arc_path_equal (Arc *arc1,
                Arc *arc2)
{
  return arc1->angle1 == arc2->angle1 &&
         arc1->angle2 == arc2->angle2 &&
         arc1->negative == arc2->negative;
}

/* We need the path to start with a line-to */
static cairo_path_t *
fixup_path (cairo_path_t *path)
{
  cairo_path_data_t *data;

  data = &path->data[0];
  if (data->header.type == CAIRO_PATH_MOVE_TO)
    data->header.type = CAIRO_PATH_LINE_TO;

  return path;
}

static void
append_arc (cairo_t *cr, double angle1, double angle2, gboolean negative)
{
  static GHashTable *arc_path_cache;
  Arc key;
  cairo_path_t *arc;

  memset (&key, 0, sizeof (Arc));
  key.angle1 = angle1;
  key.angle2 = angle2;
  key.negative = negative;

  if (arc_path_cache == NULL)
    arc_path_cache = g_hash_table_new_full ((GHashFunc)arc_path_hash,
                                            (GEqualFunc)arc_path_equal,
                                            g_free, (GDestroyNotify)cairo_path_destroy);

  arc = g_hash_table_lookup (arc_path_cache, &key);
  if (arc == NULL)
    {
      cairo_surface_t *surface;
      cairo_t *tmp;

      surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 1, 1);
      tmp = cairo_create (surface);

      if (negative)
        cairo_arc_negative (tmp, 0.0, 0.0, 1.0, angle1, angle2);
      else
        cairo_arc (tmp, 0.0, 0.0, 1.0, angle1, angle2);

      arc = fixup_path (cairo_copy_path (tmp));
      g_hash_table_insert (arc_path_cache, g_memdup (&key, sizeof (key)), arc);

      cairo_destroy (tmp);
      cairo_surface_destroy (surface);
    }

  cairo_append_path (cr, arc);
}

static void
_cairo_ellipsis (cairo_t *cr,
	         double xc, double yc,
	         double xradius, double yradius,
	         double angle1, double angle2)
{
  cairo_matrix_t save;

  if (xradius <= 0.0 || yradius <= 0.0)
    {
      cairo_line_to (cr, xc, yc);
      return;
    }

  cairo_get_matrix (cr, &save);
  cairo_translate (cr, xc, yc);
  cairo_scale (cr, xradius, yradius);
  append_arc (cr, angle1, angle2, FALSE);
  cairo_set_matrix (cr, &save);
}

static void
_cairo_ellipsis_negative (cairo_t *cr,
                          double xc, double yc,
                          double xradius, double yradius,
                          double angle1, double angle2)
{
  cairo_matrix_t save;

  if (xradius <= 0.0 || yradius <= 0.0)
    {
      cairo_line_to (cr, xc, yc);
      return;
    }

  cairo_get_matrix (cr, &save);
  cairo_translate (cr, xc, yc);
  cairo_scale (cr, xradius, yradius);
  append_arc (cr, angle1, angle2, TRUE);
  cairo_set_matrix (cr, &save);
}

void
_gtk_rounded_box_path (const GtkRoundedBox *box,
                       cairo_t             *cr)
{
  cairo_new_sub_path (cr);

  _cairo_ellipsis (cr,
                   box->box.x + box->corner[GTK_CSS_TOP_LEFT].horizontal,
                   box->box.y + box->corner[GTK_CSS_TOP_LEFT].vertical,
                   box->corner[GTK_CSS_TOP_LEFT].horizontal,
                   box->corner[GTK_CSS_TOP_LEFT].vertical,
                   G_PI, 3 * G_PI_2);
  _cairo_ellipsis (cr, 
                   box->box.x + box->box.width - box->corner[GTK_CSS_TOP_RIGHT].horizontal,
                   box->box.y + box->corner[GTK_CSS_TOP_RIGHT].vertical,
                   box->corner[GTK_CSS_TOP_RIGHT].horizontal,
                   box->corner[GTK_CSS_TOP_RIGHT].vertical,
                   - G_PI_2, 0);
  _cairo_ellipsis (cr,
                   box->box.x + box->box.width - box->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                   box->box.y + box->box.height - box->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                   box->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                   box->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                   0, G_PI_2);
  _cairo_ellipsis (cr,
                   box->box.x + box->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                   box->box.y + box->box.height - box->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                   box->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                   box->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                   G_PI_2, G_PI);

  cairo_close_path (cr);
}

double
_gtk_rounded_box_guess_length (const GtkRoundedBox *box,
                               GtkCssSide           side)
{
  double length;
  GtkCssSide before, after;

  before = side;
  after = (side + 1) % 4;

  if (side & 1)
    length = box->box.height
             - box->corner[before].vertical
             - box->corner[after].vertical;
  else
    length = box->box.width
             - box->corner[before].horizontal
             - box->corner[after].horizontal;

  length += G_PI * 0.125 * (box->corner[before].horizontal
                            + box->corner[before].vertical
                            + box->corner[after].horizontal
                            + box->corner[after].vertical);

  return length;
}

void
_gtk_rounded_box_path_side (const GtkRoundedBox *box,
                            cairo_t             *cr,
                            GtkCssSide           side)
{
  switch (side)
    {
    case GTK_CSS_TOP:
      _cairo_ellipsis (cr,
                       box->box.x + box->corner[GTK_CSS_TOP_LEFT].horizontal,
                       box->box.y + box->corner[GTK_CSS_TOP_LEFT].vertical,
                       box->corner[GTK_CSS_TOP_LEFT].horizontal,
                       box->corner[GTK_CSS_TOP_LEFT].vertical,
                       5 * G_PI_4, 3 * G_PI_2);
      _cairo_ellipsis (cr, 
                       box->box.x + box->box.width - box->corner[GTK_CSS_TOP_RIGHT].horizontal,
                       box->box.y + box->corner[GTK_CSS_TOP_RIGHT].vertical,
                       box->corner[GTK_CSS_TOP_RIGHT].horizontal,
                       box->corner[GTK_CSS_TOP_RIGHT].vertical,
                       - G_PI_2, -G_PI_4);
      break;
    case GTK_CSS_RIGHT:
      _cairo_ellipsis (cr, 
                       box->box.x + box->box.width - box->corner[GTK_CSS_TOP_RIGHT].horizontal,
                       box->box.y + box->corner[GTK_CSS_TOP_RIGHT].vertical,
                       box->corner[GTK_CSS_TOP_RIGHT].horizontal,
                       box->corner[GTK_CSS_TOP_RIGHT].vertical,
                       - G_PI_4, 0);
      _cairo_ellipsis (cr,
                       box->box.x + box->box.width - box->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                       box->box.y + box->box.height - box->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                       box->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                       box->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                       0, G_PI_4);
      break;
    case GTK_CSS_BOTTOM:
      _cairo_ellipsis (cr,
                       box->box.x + box->box.width - box->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                       box->box.y + box->box.height - box->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                       box->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                       box->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                       G_PI_4, G_PI_2);
      _cairo_ellipsis (cr,
                       box->box.x + box->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                       box->box.y + box->box.height - box->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                       box->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                       box->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                       G_PI_2, 3 * G_PI_4);
      break;
    case GTK_CSS_LEFT:
      _cairo_ellipsis (cr,
                       box->box.x + box->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                       box->box.y + box->box.height - box->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                       box->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                       box->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                       3 * G_PI_4, G_PI);
      _cairo_ellipsis (cr,
                       box->box.x + box->corner[GTK_CSS_TOP_LEFT].horizontal,
                       box->box.y + box->corner[GTK_CSS_TOP_LEFT].vertical,
                       box->corner[GTK_CSS_TOP_LEFT].horizontal,
                       box->corner[GTK_CSS_TOP_LEFT].vertical,
                       G_PI, 5 * G_PI_4);
      break;
    default:
      g_assert_not_reached ();
      break;
    }
}

void
_gtk_rounded_box_path_top (const GtkRoundedBox *outer,
                           const GtkRoundedBox *inner,
                           cairo_t             *cr)
{
  double start_angle, middle_angle, end_angle;

  if (outer->box.y == inner->box.y)
    return;

  if (outer->box.x == inner->box.x)
    start_angle = G_PI;
  else
    start_angle = 5 * G_PI_4;
  middle_angle = 3 * G_PI_2;
  if (outer->box.x + outer->box.width == inner->box.x + inner->box.width)
    end_angle = 0;
  else
    end_angle = 7 * G_PI_4;

  cairo_new_sub_path (cr);

  _cairo_ellipsis (cr,
                   outer->box.x + outer->corner[GTK_CSS_TOP_LEFT].horizontal,
                   outer->box.y + outer->corner[GTK_CSS_TOP_LEFT].vertical,
                   outer->corner[GTK_CSS_TOP_LEFT].horizontal,
                   outer->corner[GTK_CSS_TOP_LEFT].vertical,
                   start_angle, middle_angle);
  _cairo_ellipsis (cr, 
                   outer->box.x + outer->box.width - outer->corner[GTK_CSS_TOP_RIGHT].horizontal,
                   outer->box.y + outer->corner[GTK_CSS_TOP_RIGHT].vertical,
                   outer->corner[GTK_CSS_TOP_RIGHT].horizontal,
                   outer->corner[GTK_CSS_TOP_RIGHT].vertical,
                   middle_angle, end_angle);

  _cairo_ellipsis_negative (cr, 
                            inner->box.x + inner->box.width - inner->corner[GTK_CSS_TOP_RIGHT].horizontal,
                            inner->box.y + inner->corner[GTK_CSS_TOP_RIGHT].vertical,
                            inner->corner[GTK_CSS_TOP_RIGHT].horizontal,
                            inner->corner[GTK_CSS_TOP_RIGHT].vertical,
                            end_angle, middle_angle);
  _cairo_ellipsis_negative (cr,
                            inner->box.x + inner->corner[GTK_CSS_TOP_LEFT].horizontal,
                            inner->box.y + inner->corner[GTK_CSS_TOP_LEFT].vertical,
                            inner->corner[GTK_CSS_TOP_LEFT].horizontal,
                            inner->corner[GTK_CSS_TOP_LEFT].vertical,
                            middle_angle, start_angle);

  cairo_close_path (cr);
}

void
_gtk_rounded_box_path_right (const GtkRoundedBox *outer,
                             const GtkRoundedBox *inner,
                             cairo_t             *cr)
{
  double start_angle, middle_angle, end_angle;

  if (outer->box.x + outer->box.width == inner->box.x + inner->box.width)
    return;

  if (outer->box.y == inner->box.y)
    start_angle = 3 * G_PI_2;
  else
    start_angle = 7 * G_PI_4;
  middle_angle = 0;
  if (outer->box.y + outer->box.height == inner->box.y + inner->box.height)
    end_angle = G_PI_2;
  else
    end_angle = G_PI_4;

  cairo_new_sub_path (cr);

  _cairo_ellipsis (cr, 
                   outer->box.x + outer->box.width - outer->corner[GTK_CSS_TOP_RIGHT].horizontal,
                   outer->box.y + outer->corner[GTK_CSS_TOP_RIGHT].vertical,
                   outer->corner[GTK_CSS_TOP_RIGHT].horizontal,
                   outer->corner[GTK_CSS_TOP_RIGHT].vertical,
                   start_angle, middle_angle);
  _cairo_ellipsis (cr,
                   outer->box.x + outer->box.width - outer->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                   outer->box.y + outer->box.height - outer->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                   outer->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                   outer->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                   middle_angle, end_angle);

  _cairo_ellipsis_negative (cr,
                            inner->box.x + inner->box.width - inner->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                            inner->box.y + inner->box.height - inner->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                            inner->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                            inner->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                            end_angle, middle_angle);
  _cairo_ellipsis_negative (cr, 
                            inner->box.x + inner->box.width - inner->corner[GTK_CSS_TOP_RIGHT].horizontal,
                            inner->box.y + inner->corner[GTK_CSS_TOP_RIGHT].vertical,
                            inner->corner[GTK_CSS_TOP_RIGHT].horizontal,
                            inner->corner[GTK_CSS_TOP_RIGHT].vertical,
                            middle_angle, start_angle);

  cairo_close_path (cr);
}

void
_gtk_rounded_box_path_bottom (const GtkRoundedBox *outer,
                              const GtkRoundedBox *inner,
                              cairo_t             *cr)
{
  double start_angle, middle_angle, end_angle;

  if (outer->box.y + outer->box.height == inner->box.y + inner->box.height)
    return;

  if (outer->box.x + outer->box.width == inner->box.x + inner->box.width)
    start_angle = 0;
  else
    start_angle = G_PI_4;
  middle_angle = G_PI_2;
  if (outer->box.x == inner->box.x)
    end_angle = G_PI;
  else
    end_angle = 3 * G_PI_4;

  cairo_new_sub_path (cr);

  _cairo_ellipsis (cr,
                   outer->box.x + outer->box.width - outer->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                   outer->box.y + outer->box.height - outer->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                   outer->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                   outer->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                   start_angle, middle_angle);
  _cairo_ellipsis (cr,
                   outer->box.x + outer->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                   outer->box.y + outer->box.height - outer->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                   outer->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                   outer->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                   middle_angle, end_angle);

  _cairo_ellipsis_negative (cr,
                            inner->box.x + inner->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                            inner->box.y + inner->box.height - inner->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                            inner->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                            inner->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                            end_angle, middle_angle);
  _cairo_ellipsis_negative (cr,
                            inner->box.x + inner->box.width - inner->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                            inner->box.y + inner->box.height - inner->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                            inner->corner[GTK_CSS_BOTTOM_RIGHT].horizontal,
                            inner->corner[GTK_CSS_BOTTOM_RIGHT].vertical,
                            middle_angle, start_angle);

  cairo_close_path (cr);
}

void
_gtk_rounded_box_path_left (const GtkRoundedBox *outer,
                            const GtkRoundedBox *inner,
                            cairo_t             *cr)
{
  double start_angle, middle_angle, end_angle;

  if (outer->box.x == inner->box.x)
    return;

  if (outer->box.y + outer->box.height == inner->box.y + inner->box.height)
    start_angle = G_PI_2;
  else
    start_angle = 3 * G_PI_4;
  middle_angle = G_PI;
  if (outer->box.y == inner->box.y)
    end_angle = 3 * G_PI_2;
  else
    end_angle = 5 * G_PI_4;

  cairo_new_sub_path (cr);

  _cairo_ellipsis (cr,
                   outer->box.x + outer->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                   outer->box.y + outer->box.height - outer->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                   outer->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                   outer->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                   start_angle, middle_angle);
  _cairo_ellipsis (cr,
                   outer->box.x + outer->corner[GTK_CSS_TOP_LEFT].horizontal,
                   outer->box.y + outer->corner[GTK_CSS_TOP_LEFT].vertical,
                   outer->corner[GTK_CSS_TOP_LEFT].horizontal,
                   outer->corner[GTK_CSS_TOP_LEFT].vertical,
                   middle_angle, end_angle);

  _cairo_ellipsis_negative (cr,
                            inner->box.x + inner->corner[GTK_CSS_TOP_LEFT].horizontal,
                            inner->box.y + inner->corner[GTK_CSS_TOP_LEFT].vertical,
                            inner->corner[GTK_CSS_TOP_LEFT].horizontal,
                            inner->corner[GTK_CSS_TOP_LEFT].vertical,
                            end_angle, middle_angle);
  _cairo_ellipsis_negative (cr,
                            inner->box.x + inner->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                            inner->box.y + inner->box.height - inner->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                            inner->corner[GTK_CSS_BOTTOM_LEFT].horizontal,
                            inner->corner[GTK_CSS_BOTTOM_LEFT].vertical,
                            middle_angle, start_angle);

  cairo_close_path (cr);
}

void
_gtk_rounded_box_clip_path (const GtkRoundedBox *box,
                            cairo_t             *cr)
{
  cairo_rectangle (cr,
                   box->box.x, box->box.y,
                   box->box.width, box->box.height);
}

gboolean
_gtk_rounded_box_intersects_rectangle (const GtkRoundedBox *box,
                                       gdouble              x1,
                                       gdouble              y1,
                                       gdouble              x2,
                                       gdouble              y2)
{
  if (x2 < box->box.x ||
      y2 < box->box.y ||
      x1 >= box->box.x + box->box.width ||
      y1 >= box->box.y + box->box.height)
    return FALSE;

  return TRUE;
}

gboolean
_gtk_rounded_box_contains_rectangle (const GtkRoundedBox *box,
                                     gdouble              x1,
                                     gdouble              y1,
                                     gdouble              x2,
                                     gdouble              y2)
{
  if (x1 < box->box.x ||
      y1 < box->box.y ||
      x2 > box->box.x + box->box.width ||
      y2 > box->box.y + box->box.height)
    return FALSE;

  if (x1 < box->box.x + box->corner[GTK_CSS_TOP_LEFT].horizontal &&
      y1 < box->box.y + box->corner[GTK_CSS_TOP_LEFT].vertical)
    return FALSE;

  if (x2 > box->box.x + box->box.width - box->corner[GTK_CSS_TOP_RIGHT].horizontal &&
      y1 < box->box.y + box->corner[GTK_CSS_TOP_RIGHT].vertical)
    return FALSE;

  if (x2 > box->box.x + box->box.width - box->corner[GTK_CSS_BOTTOM_RIGHT].horizontal &&
      y2 > box->box.y + box->box.height - box->corner[GTK_CSS_BOTTOM_RIGHT].vertical)
    return FALSE;

  if (x1 < box->box.x + box->corner[GTK_CSS_BOTTOM_LEFT].horizontal &&
      y2 > box->box.y + box->box.height - box->corner[GTK_CSS_BOTTOM_LEFT].vertical)
    return FALSE;

  return TRUE;
}
