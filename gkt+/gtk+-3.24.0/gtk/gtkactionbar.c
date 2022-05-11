/*
 * Copyright (c) 2013 - 2014 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"

#include "gtkactionbar.h"
#include "gtkintl.h"
#include "gtkaccessible.h"
#include "gtkbuildable.h"
#include "gtktypebuiltins.h"
#include "gtkbox.h"
#include "gtkrevealer.h"
#include "gtkcsscustomgadgetprivate.h"
#include "gtkwidgetprivate.h"
#include "gtkcontainerprivate.h"

#include <string.h>

/**
 * SECTION:gtkactionbar
 * @Short_description: A full width bar for presenting contextual actions
 * @Title: GtkActionBar
 * @See_also: #GtkBox
 *
 * GtkActionBar is designed to present contextual actions. It is
 * expected to be displayed below the content and expand horizontally
 * to fill the area.
 *
 * It allows placing children at the start or the end. In addition, it
 * contains an internal centered box which is centered with respect to
 * the full width of the box, even if the children at either side take
 * up different amounts of space.
 *
 * # CSS nodes
 *
 * GtkActionBar has a single CSS node with name actionbar.
 */

struct _GtkActionBarPrivate
{
  GtkWidget *box;
  GtkWidget *revealer;
  GtkCssGadget *gadget;
};

enum {
  CHILD_PROP_0,
  CHILD_PROP_PACK_TYPE,
  CHILD_PROP_POSITION
};

static void gtk_action_bar_finalize (GObject *object);
static void gtk_action_bar_buildable_interface_init (GtkBuildableIface *iface);

G_DEFINE_TYPE_WITH_CODE (GtkActionBar, gtk_action_bar, GTK_TYPE_BIN,
                         G_ADD_PRIVATE (GtkActionBar)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE,
                                                gtk_action_bar_buildable_interface_init))

static void
gtk_action_bar_show (GtkWidget *widget)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (widget));

  GTK_WIDGET_CLASS (gtk_action_bar_parent_class)->show (widget);

  gtk_revealer_set_reveal_child (GTK_REVEALER (priv->revealer), TRUE);
}

static void
child_revealed (GObject *object, GParamSpec *pspec, GtkWidget *widget)
{
  GTK_WIDGET_CLASS (gtk_action_bar_parent_class)->hide (widget);
  g_signal_handlers_disconnect_by_func (object, child_revealed, widget);
  g_object_notify (G_OBJECT (widget), "visible");
}

static void
gtk_action_bar_hide (GtkWidget *widget)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (widget));

  g_signal_connect_object (priv->revealer, "notify::child-revealed",
                            G_CALLBACK (child_revealed), widget, 0);
  gtk_revealer_set_reveal_child (GTK_REVEALER (priv->revealer), FALSE);
}

static void
gtk_action_bar_add (GtkContainer *container,
                    GtkWidget    *child)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (container));

  /* When constructing the widget, we want the revealer to be added
   * as the first child of the bar, as an implementation detail.
   * After that, the child added by the application should be added
   * to box.
   */

  if (priv->box == NULL)
    GTK_CONTAINER_CLASS (gtk_action_bar_parent_class)->add (container, child);
  else
    gtk_container_add (GTK_CONTAINER (priv->box), child);
}

static void
gtk_action_bar_remove (GtkContainer *container,
                       GtkWidget    *child)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (container));

  if (child == priv->revealer)
    GTK_CONTAINER_CLASS (gtk_action_bar_parent_class)->remove (container, child);
  else
    gtk_container_remove (GTK_CONTAINER (priv->box), child);
}

static void
gtk_action_bar_forall (GtkContainer *container,
                       gboolean      include_internals,
                       GtkCallback   callback,
                       gpointer      callback_data)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (container));

  if (include_internals)
    (* callback) (priv->revealer, callback_data);
  else if (priv->box)
    gtk_container_forall (GTK_CONTAINER (priv->box), callback, callback_data);
}

static void
gtk_action_bar_destroy (GtkWidget *widget)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (widget));

  if (priv->revealer)
    {
      gtk_widget_destroy (priv->revealer);
      priv->revealer = NULL;
    }

  GTK_WIDGET_CLASS (gtk_action_bar_parent_class)->destroy (widget);
}

static GType
gtk_action_bar_child_type (GtkContainer *container)
{
  return GTK_TYPE_WIDGET;
}

static void
gtk_action_bar_get_child_property (GtkContainer *container,
                                   GtkWidget    *child,
                                   guint         property_id,
                                   GValue       *value,
                                   GParamSpec   *pspec)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (container));

  if (child == priv->revealer)
    g_param_value_set_default (pspec, value);
  else
    gtk_container_child_get_property (GTK_CONTAINER (priv->box),
                                      child,
                                      pspec->name,
                                      value);
}

static void
gtk_action_bar_set_child_property (GtkContainer *container,
                                   GtkWidget    *child,
                                   guint         property_id,
                                   const GValue *value,
                                   GParamSpec   *pspec)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (container));

  if (child != priv->revealer)
    gtk_container_child_set_property (GTK_CONTAINER (priv->box),
                                      child,
                                      pspec->name,
                                      value);
}

static gboolean
gtk_action_bar_render (GtkCssGadget *gadget,
                       cairo_t      *cr,
                       int           x,
                       int           y,
                       int           width,
                       int           height,
                       gpointer      data)
{
  GTK_WIDGET_CLASS (gtk_action_bar_parent_class)->draw (gtk_css_gadget_get_owner (gadget), cr);

  return FALSE;
}

static gboolean
gtk_action_bar_draw (GtkWidget *widget,
                     cairo_t   *cr)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (widget));

  gtk_css_gadget_draw (priv->gadget, cr);

  return FALSE;
}

static void
gtk_action_bar_allocate (GtkCssGadget        *gadget,
                         const GtkAllocation *allocation,
                         int                  baseline,
                         GtkAllocation       *out_clip,
                         gpointer             data)
{
  GtkWidget *widget = gtk_css_gadget_get_owner (gadget);
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (widget));

  gtk_widget_size_allocate (priv->revealer, (GtkAllocation *)allocation);

  gtk_container_get_children_clip (GTK_CONTAINER (widget), out_clip);
}

static void
gtk_action_bar_size_allocate (GtkWidget     *widget,
                              GtkAllocation *allocation)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (widget));
  GtkAllocation clip;

  gtk_widget_set_allocation (widget, allocation);

  gtk_css_gadget_allocate (priv->gadget, allocation, gtk_widget_get_allocated_baseline (widget), &clip);

  gtk_widget_set_clip (widget, &clip);
}

static void
gtk_action_bar_measure (GtkCssGadget   *gadget,
                        GtkOrientation  orientation,
                        int             for_size,
                        int            *minimum,
                        int            *natural,
                        int            *minimum_baseline,
                        int            *natural_baseline,
                        gpointer        data)
{
  GtkWidget *widget = gtk_css_gadget_get_owner (gadget);
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (widget));

  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    gtk_widget_get_preferred_width_for_height (priv->revealer, for_size, minimum, natural);
  else
    gtk_widget_get_preferred_height_and_baseline_for_width (priv->revealer, for_size, minimum, natural, minimum_baseline, natural_baseline);
}

static void
gtk_action_bar_get_preferred_width_for_height (GtkWidget *widget,
                                               gint       height,
                                               gint      *minimum,
                                               gint      *natural)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (widget));

  gtk_css_gadget_get_preferred_size (priv->gadget,
                                     GTK_ORIENTATION_HORIZONTAL,
                                     height,
                                     minimum, natural,
                                     NULL, NULL);
}

static void
gtk_action_bar_get_preferred_height_and_baseline_for_width (GtkWidget *widget,
                                                            gint       width,
                                                            gint      *minimum,
                                                            gint      *natural,
                                                            gint      *minimum_baseline,
                                                            gint      *natural_baseline)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (GTK_ACTION_BAR (widget));

  gtk_css_gadget_get_preferred_size (priv->gadget,
                                     GTK_ORIENTATION_VERTICAL,
                                     width,
                                     minimum, natural,
                                     minimum_baseline, natural_baseline);
}

static void
gtk_action_bar_class_init (GtkActionBarClass *klass)
{
  GObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  object_class = G_OBJECT_CLASS (klass);
  widget_class = GTK_WIDGET_CLASS (klass);
  container_class = GTK_CONTAINER_CLASS (klass);

  object_class->finalize = gtk_action_bar_finalize;

  widget_class->show = gtk_action_bar_show;
  widget_class->hide = gtk_action_bar_hide;
  widget_class->destroy = gtk_action_bar_destroy;
  widget_class->draw = gtk_action_bar_draw;
  widget_class->size_allocate = gtk_action_bar_size_allocate;
  widget_class->get_preferred_width_for_height = gtk_action_bar_get_preferred_width_for_height;
  widget_class->get_preferred_height_and_baseline_for_width = gtk_action_bar_get_preferred_height_and_baseline_for_width;

  container_class->add = gtk_action_bar_add;
  container_class->remove = gtk_action_bar_remove;
  container_class->forall = gtk_action_bar_forall;
  container_class->child_type = gtk_action_bar_child_type;
  container_class->set_child_property = gtk_action_bar_set_child_property;
  container_class->get_child_property = gtk_action_bar_get_child_property;

  gtk_container_class_install_child_property (container_class,
                                              CHILD_PROP_PACK_TYPE,
                                              g_param_spec_enum ("pack-type",
                                                                 P_("Pack type"),
                                                                 P_("A GtkPackType indicating whether the child is packed with reference to the start or end of the parent"),
                                                                 GTK_TYPE_PACK_TYPE, GTK_PACK_START,
                                                                 G_PARAM_READWRITE));
  gtk_container_class_install_child_property (container_class,
                                              CHILD_PROP_POSITION,
                                              g_param_spec_int ("position",
                                                                P_("Position"),
                                                                P_("The index of the child in the parent"),
                                                                -1, G_MAXINT, 0,
                                                                G_PARAM_READWRITE));

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gtk/libgtk/ui/gtkactionbar.ui");
  gtk_widget_class_bind_template_child_private (widget_class, GtkActionBar, box);
  gtk_widget_class_bind_template_child_private (widget_class, GtkActionBar, revealer);

  gtk_widget_class_set_accessible_role (widget_class, ATK_ROLE_PANEL);
  gtk_widget_class_set_css_name (widget_class, "actionbar");
}

static void
gtk_action_bar_init (GtkActionBar *action_bar)
{
  GtkWidget *widget = GTK_WIDGET (action_bar);
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (action_bar);
  GtkCssNode *widget_node;

  gtk_widget_set_redraw_on_allocate (widget, TRUE);

  gtk_widget_init_template (GTK_WIDGET (action_bar));

  gtk_revealer_set_transition_type (GTK_REVEALER (priv->revealer), GTK_REVEALER_TRANSITION_TYPE_SLIDE_UP);

  widget_node = gtk_widget_get_css_node (GTK_WIDGET (action_bar));
  priv->gadget = gtk_css_custom_gadget_new_for_node (widget_node,
                                                     GTK_WIDGET (action_bar),
                                                     gtk_action_bar_measure,
                                                     gtk_action_bar_allocate,
                                                     gtk_action_bar_render,
                                                     NULL,
                                                     NULL);
}

static void
gtk_action_bar_finalize (GObject *object)
{
  GtkActionBar *action_bar = GTK_ACTION_BAR (object);
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (action_bar);

  g_clear_object (&priv->gadget);

  G_OBJECT_CLASS (gtk_action_bar_parent_class)->finalize (object);
}

static void
gtk_action_bar_buildable_add_child (GtkBuildable *buildable,
                                    GtkBuilder   *builder,
                                    GObject      *child,
                                    const gchar  *type)
{
  GtkActionBar *action_bar = GTK_ACTION_BAR (buildable);
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (action_bar);

  if (type && strcmp (type, "center") == 0)
    gtk_box_set_center_widget (GTK_BOX (priv->box), GTK_WIDGET (child));
  else if (!type)
    gtk_container_add (GTK_CONTAINER (buildable), GTK_WIDGET (child));
  else
    GTK_BUILDER_WARN_INVALID_CHILD_TYPE (action_bar, type);
}

static GtkBuildableIface *parent_buildable_iface;

static void
gtk_action_bar_buildable_interface_init (GtkBuildableIface *iface)
{
  parent_buildable_iface = g_type_interface_peek_parent (iface);
  iface->add_child = gtk_action_bar_buildable_add_child;
}

/**
 * gtk_action_bar_pack_start:
 * @action_bar: A #GtkActionBar
 * @child: the #GtkWidget to be added to @action_bar
 *
 * Adds @child to @action_bar, packed with reference to the
 * start of the @action_bar.
 *
 * Since: 3.12
 */
void
gtk_action_bar_pack_start (GtkActionBar *action_bar,
                           GtkWidget    *child)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (action_bar);

  gtk_box_pack_start (GTK_BOX (priv->box), child, FALSE, TRUE, 0);
}

/**
 * gtk_action_bar_pack_end:
 * @action_bar: A #GtkActionBar
 * @child: the #GtkWidget to be added to @action_bar
 *
 * Adds @child to @action_bar, packed with reference to the
 * end of the @action_bar.
 *
 * Since: 3.12
 */
void
gtk_action_bar_pack_end (GtkActionBar *action_bar,
                         GtkWidget    *child)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (action_bar);

  gtk_box_pack_end (GTK_BOX (priv->box), child, FALSE, TRUE, 0);
}

/**
 * gtk_action_bar_set_center_widget:
 * @action_bar: a #GtkActionBar
 * @center_widget: (allow-none): a widget to use for the center
 *
 * Sets the center widget for the #GtkActionBar.
 *
 * Since: 3.12
 */
void
gtk_action_bar_set_center_widget (GtkActionBar *action_bar,
                                  GtkWidget    *center_widget)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (action_bar);

  gtk_box_set_center_widget (GTK_BOX (priv->box), center_widget);
}

/**
 * gtk_action_bar_get_center_widget:
 * @action_bar: a #GtkActionBar
 *
 * Retrieves the center bar widget of the bar.
 *
 * Returns: (transfer none) (nullable): the center #GtkWidget or %NULL.
 *
 * Since: 3.12
 */
GtkWidget *
gtk_action_bar_get_center_widget (GtkActionBar *action_bar)
{
  GtkActionBarPrivate *priv = gtk_action_bar_get_instance_private (action_bar);

  g_return_val_if_fail (GTK_IS_ACTION_BAR (action_bar), NULL);

  return gtk_box_get_center_widget (GTK_BOX (priv->box));
}

/**
 * gtk_action_bar_new:
 *
 * Creates a new #GtkActionBar widget.
 *
 * Returns: a new #GtkActionBar
 *
 * Since: 3.12
 */
GtkWidget *
gtk_action_bar_new (void)
{
  return GTK_WIDGET (g_object_new (GTK_TYPE_ACTION_BAR, NULL));
}
