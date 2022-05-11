/*
 * Copyright © 2019 Benjamin Otte
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Benjamin Otte <otte@gnome.org>
 */

#include "config.h"

#include "gtktreeexpander.h"

#include "gtkaccessible.h"
#include "gtkboxlayout.h"
#include "gtkbuiltiniconprivate.h"
#include "gtkdropcontrollermotion.h"
#include "gtkenums.h"
#include "gtkgestureclick.h"
#include "gtkintl.h"
#include "gtktreelistmodel.h"
#include "gtkprivate.h"

/**
 * GtkTreeExpander:
 *
 * `GtkTreeExpander` is a widget that provides an expander for a list.
 *
 * It is typically placed as a bottommost child into a `GtkListView`
 * to allow users to expand and collapse children in a list with a
 * [class@Gtk.TreeListModel]. `GtkTreeExpander` provides the common UI
 * elements, gestures and keybindings for this purpose.
 *
 * On top of this, the "listitem.expand", "listitem.collapse" and
 * "listitem.toggle-expand" actions are provided to allow adding custom
 * UI for managing expanded state.
 *
 * The `GtkTreeListModel` must be set to not be passthrough. Then it
 * will provide [class@Gtk.TreeListRow] items which can be set via
 * [method@Gtk.TreeExpander.set_list_row] on the expander.
 * The expander will then watch that row item automatically.
 * [method@Gtk.TreeExpander.set_child] sets the widget that displays
 * the actual row contents.
 *
 * # CSS nodes
 *
 * ```
 * treeexpander
 * ├── [indent]*
 * ├── [expander]
 * ╰── <child>
 * ```
 *
 * `GtkTreeExpander` has zero or one CSS nodes with the name "expander" that
 * should display the expander icon. The node will be `:checked` when it
 * is expanded. If the node is not expandable, an "indent" node will be
 * displayed instead.
 *
 * For every level of depth, another "indent" node is prepended.
 *
 * # Accessibility
 *
 * `GtkTreeExpander` uses the %GTK_ACCESSIBLE_ROLE_GROUP role. The expander icon
 * is represented as a %GTK_ACCESSIBLE_ROLE_BUTTON, labelled by the expander's
 * child, and toggling it will change the %GTK_ACCESSIBLE_STATE_EXPANDED state.
 */

struct _GtkTreeExpander
{
  GtkWidget parent_instance;

  GtkTreeListRow *list_row;
  GtkWidget *child;

  GtkWidget *expander_icon;
  guint notify_handler;

  gboolean indent_for_icon;

  guint expand_timer;
};

enum
{
  PROP_0,
  PROP_CHILD,
  PROP_ITEM,
  PROP_LIST_ROW,
  PROP_INDENT_FOR_ICON,

  N_PROPS
};

G_DEFINE_TYPE (GtkTreeExpander, gtk_tree_expander, GTK_TYPE_WIDGET)

static GParamSpec *properties[N_PROPS] = { NULL, };

static void
gtk_tree_expander_click_gesture_pressed (GtkGestureClick *gesture,
                                         int              n_press,
                                         double           x,
                                         double           y,
                                         gpointer         unused)
{
  GtkWidget *widget = gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (gesture));

  gtk_widget_activate_action (widget, "listitem.toggle-expand", NULL);

  gtk_widget_set_state_flags (widget,
                              GTK_STATE_FLAG_ACTIVE,
                              FALSE);

  gtk_gesture_set_state (GTK_GESTURE (gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

static void
gtk_tree_expander_click_gesture_released (GtkGestureClick *gesture,
                                          int              n_press,
                                          double           x,
                                          double           y,
                                          gpointer         unused)
{
  gtk_widget_unset_state_flags (gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (gesture)),
                                GTK_STATE_FLAG_ACTIVE);

  gtk_gesture_set_state (GTK_GESTURE (gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

static void
gtk_tree_expander_click_gesture_canceled (GtkGestureClick  *gesture,
                                          GdkEventSequence *sequence,
                                          gpointer          unused)
{
  gtk_widget_unset_state_flags (gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (gesture)),
                                GTK_STATE_FLAG_ACTIVE);

  gtk_gesture_set_state (GTK_GESTURE (gesture), GTK_EVENT_SEQUENCE_CLAIMED);
}

static void
gtk_tree_expander_update_for_list_row (GtkTreeExpander *self)
{
  if (self->list_row == NULL)
    {
      GtkWidget *child;

      for (child = gtk_widget_get_first_child (GTK_WIDGET (self));
           child != self->child;
           child = gtk_widget_get_first_child (GTK_WIDGET (self)))
        {
          gtk_widget_unparent (child);
        }
      self->expander_icon = NULL;

      gtk_accessible_reset_state (GTK_ACCESSIBLE (self), GTK_ACCESSIBLE_STATE_EXPANDED);
    }
  else
    {
      GtkWidget *child;
      guint i, depth;

      depth = gtk_tree_list_row_get_depth (self->list_row);
      if (gtk_tree_list_row_is_expandable (self->list_row))
        {
          if (self->expander_icon == NULL)
            {
              GtkGesture *gesture;

              self->expander_icon =
                g_object_new (GTK_TYPE_BUILTIN_ICON,
                              "css-name", "expander",
                              "accessible-role", GTK_ACCESSIBLE_ROLE_BUTTON,
                              NULL);

              gesture = gtk_gesture_click_new ();
              gtk_event_controller_set_propagation_phase (GTK_EVENT_CONTROLLER (gesture),
                                                          GTK_PHASE_BUBBLE);
              gtk_gesture_single_set_touch_only (GTK_GESTURE_SINGLE (gesture),
                                                 FALSE);
              gtk_gesture_single_set_button (GTK_GESTURE_SINGLE (gesture),
                                             GDK_BUTTON_PRIMARY);
              g_signal_connect (gesture, "pressed",
                                G_CALLBACK (gtk_tree_expander_click_gesture_pressed), NULL);
              g_signal_connect (gesture, "released",
                                G_CALLBACK (gtk_tree_expander_click_gesture_released), NULL);
              g_signal_connect (gesture, "cancel",
                                G_CALLBACK (gtk_tree_expander_click_gesture_canceled), NULL);
              gtk_widget_add_controller (self->expander_icon, GTK_EVENT_CONTROLLER (gesture));

              gtk_widget_insert_before (self->expander_icon,
                                        GTK_WIDGET (self),
                                        self->child);

              gtk_accessible_update_property (GTK_ACCESSIBLE (self->expander_icon),
                                              GTK_ACCESSIBLE_PROPERTY_LABEL, _("Expand"),
                                              -1);
            }

          if (gtk_tree_list_row_get_expanded (self->list_row))
            {
              gtk_widget_set_state_flags (self->expander_icon, GTK_STATE_FLAG_CHECKED, FALSE);
              gtk_accessible_update_state (GTK_ACCESSIBLE (self),
                                           GTK_ACCESSIBLE_STATE_EXPANDED, TRUE,
                                           -1);
            }
          else
            {
              gtk_widget_unset_state_flags (self->expander_icon, GTK_STATE_FLAG_CHECKED);
              gtk_accessible_update_state (GTK_ACCESSIBLE (self),
                                           GTK_ACCESSIBLE_STATE_EXPANDED, FALSE,
                                           -1);
            }

          child = gtk_widget_get_prev_sibling (self->expander_icon);
        }
      else
        {
          g_clear_pointer (&self->expander_icon, gtk_widget_unparent);

          if (self->indent_for_icon)
            depth++;

          if (self->child)
            child = gtk_widget_get_prev_sibling (self->child);
          else
            child = gtk_widget_get_last_child (GTK_WIDGET (self));
        }

      for (i = 0; i < depth; i++)
        {
          if (child)
            child = gtk_widget_get_prev_sibling (child);
          else
            {
              GtkWidget *indent =
                g_object_new (GTK_TYPE_BUILTIN_ICON,
                              "css-name", "indent",
                              "accessible-role", GTK_ACCESSIBLE_ROLE_PRESENTATION,
                              NULL);

              gtk_widget_insert_after (indent, GTK_WIDGET (self), NULL);
            }
        }

      /* The level property is >= 1 */
      gtk_accessible_update_property (GTK_ACCESSIBLE (self),
                                      GTK_ACCESSIBLE_PROPERTY_LEVEL, depth + 1,
                                      -1);

      while (child)
        {
          GtkWidget *prev = gtk_widget_get_prev_sibling (child);
          gtk_widget_unparent (child);
          child = prev;
        }
    }
}

static void
gtk_tree_expander_list_row_notify_cb (GtkTreeListRow  *list_row,
                                      GParamSpec      *pspec,
                                      GtkTreeExpander *self)
{
  if (pspec->name == g_intern_static_string ("expanded"))
    {
      if (self->expander_icon)
        {
          if (gtk_tree_list_row_get_expanded (list_row))
            {
              gtk_widget_set_state_flags (self->expander_icon, GTK_STATE_FLAG_CHECKED, FALSE);
              gtk_accessible_update_state (GTK_ACCESSIBLE (self->expander_icon),
                                           GTK_ACCESSIBLE_STATE_EXPANDED, TRUE,
                                           -1);
            }
          else
            {
              gtk_widget_unset_state_flags (self->expander_icon, GTK_STATE_FLAG_CHECKED);
              gtk_accessible_update_state (GTK_ACCESSIBLE (self->expander_icon),
                                           GTK_ACCESSIBLE_STATE_EXPANDED, FALSE,
                                           -1);
            }
        }
    }
  else if (pspec->name == g_intern_static_string ("item"))
    {
      g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_ITEM]);
    }
  else
    {
      /* can this happen other than when destroying the row? */
      gtk_tree_expander_update_for_list_row (self);
    }
}

static gboolean
gtk_tree_expander_focus (GtkWidget        *widget,
                         GtkDirectionType  direction)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (widget);

  /* The idea of this function is the following:
   * 1. If any child can take focus, do not ever attempt
   *    to take focus.
   * 2. Otherwise, if this item is selectable or activatable,
   *    allow focusing this widget.
   *
   * This makes sure every item in a list is focusable for
   * activation and selection handling, but no useless widgets
   * get focused and moving focus is as fast as possible.
   */
  if (self->child)
    {
      if (gtk_widget_get_focus_child (widget))
        return FALSE;
      if (gtk_widget_child_focus (self->child, direction))
        return TRUE;
    }

  if (gtk_widget_is_focus (widget))
    return FALSE;

  if (!gtk_widget_get_focusable (widget))
    return FALSE;

  gtk_widget_grab_focus (widget);

  return TRUE;
}

static gboolean
gtk_tree_expander_grab_focus (GtkWidget *widget)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (widget);

  if (self->child && gtk_widget_grab_focus (self->child))
    return TRUE;

  return GTK_WIDGET_CLASS (gtk_tree_expander_parent_class)->grab_focus (widget);
}

static void
gtk_tree_expander_clear_list_row (GtkTreeExpander *self)
{
  if (self->list_row == NULL)
    return;

  g_signal_handler_disconnect (self->list_row, self->notify_handler);
  self->notify_handler = 0;
  g_clear_object (&self->list_row);
}

static void
gtk_tree_expander_dispose (GObject *object)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (object);

  if (self->expand_timer)
    {
      g_source_remove (self->expand_timer);
      self->expand_timer = 0;
    }

  gtk_tree_expander_clear_list_row (self);
  gtk_tree_expander_update_for_list_row (self);

  g_clear_pointer (&self->child, gtk_widget_unparent);

  g_assert (self->expander_icon == NULL);

  G_OBJECT_CLASS (gtk_tree_expander_parent_class)->dispose (object);
}

static void
gtk_tree_expander_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (object);

  switch (property_id)
    {
    case PROP_CHILD:
      g_value_set_object (value, self->child);
      break;

    case PROP_ITEM:
      g_value_take_object (value, gtk_tree_expander_get_item (self));
      break;

    case PROP_LIST_ROW:
      g_value_set_object (value, self->list_row);
      break;

    case PROP_INDENT_FOR_ICON:
      g_value_set_boolean (value, gtk_tree_expander_get_indent_for_icon (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_tree_expander_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (object);

  switch (property_id)
    {
    case PROP_CHILD:
      gtk_tree_expander_set_child (self, g_value_get_object (value));
      break;

    case PROP_LIST_ROW:
      gtk_tree_expander_set_list_row (self, g_value_get_object (value));
      break;

    case PROP_INDENT_FOR_ICON:
      gtk_tree_expander_set_indent_for_icon (self, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gtk_tree_expander_expand (GtkWidget  *widget,
                          const char *action_name,
                          GVariant   *parameter)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (widget);

  if (self->list_row == NULL)
    return;

  gtk_tree_list_row_set_expanded (self->list_row, TRUE);
}

static void
gtk_tree_expander_collapse (GtkWidget  *widget,
                            const char *action_name,
                            GVariant   *parameter)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (widget);

  if (self->list_row == NULL)
    return;

  gtk_tree_list_row_set_expanded (self->list_row, FALSE);
}

static void
gtk_tree_expander_toggle_expand (GtkWidget  *widget,
                                 const char *action_name,
                                 GVariant   *parameter)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (widget);

  if (self->list_row == NULL)
    return;

  gtk_tree_list_row_set_expanded (self->list_row, !gtk_tree_list_row_get_expanded (self->list_row));
}

static gboolean
expand_collapse_right (GtkWidget *widget,
                       GVariant  *args,
                       gpointer   unused)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (widget);

  if (self->list_row == NULL)
    return FALSE;

  gtk_tree_list_row_set_expanded (self->list_row, gtk_widget_get_direction (widget) != GTK_TEXT_DIR_RTL);

  return TRUE;
}

static gboolean
expand_collapse_left (GtkWidget *widget,
                      GVariant  *args,
                      gpointer   unused)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (widget);

  if (self->list_row == NULL)
    return FALSE;

  gtk_tree_list_row_set_expanded (self->list_row, gtk_widget_get_direction (widget) == GTK_TEXT_DIR_RTL);

  return TRUE;
}

static void
gtk_tree_expander_class_init (GtkTreeExpanderClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  widget_class->focus = gtk_tree_expander_focus;
  widget_class->grab_focus = gtk_tree_expander_grab_focus;

  gobject_class->dispose = gtk_tree_expander_dispose;
  gobject_class->get_property = gtk_tree_expander_get_property;
  gobject_class->set_property = gtk_tree_expander_set_property;

  /**
   * GtkTreeExpander:child: (attributes org.gtk.Property.get=gtk_tree_expander_get_child org.gtk.Property.set=gtk_tree_expander_set_child)
   *
   * The child widget with the actual contents.
   */
  properties[PROP_CHILD] =
    g_param_spec_object ("child",
                         P_("Child"),
                         P_("The child widget with the actual contents"),
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkTreeExpander:item: (attributes org.gtk.Property.get=gtk_tree_expander_get_item)
   *
   * The item held by this expander's row.
   */
  properties[PROP_ITEM] =
      g_param_spec_object ("item",
                           P_("Item"),
                           P_("The item held by this expander's row"),
                           G_TYPE_OBJECT,
                           G_PARAM_READABLE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkTreeExpander:list-row: (attributes org.gtk.Property.get=gtk_tree_expander_get_list_row org.gtk.Property.set=gtk_tree_expander_set_list_row)
   *
   * The list row to track for expander state.
   */
  properties[PROP_LIST_ROW] =
    g_param_spec_object ("list-row",
                         P_("List row"),
                         P_("The list row to track for expander state"),
                         GTK_TYPE_TREE_LIST_ROW,
                         G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS);

  /**
   * GtkTreeExpander:indent-for-icon: (attributes org.gtk.Property.get=gtk_tree_expander_get_indent_for_icon org.gtk.Property.set=gtk_tree_expander_set_indent_for_icon)
   *
   * TreeExpander indents the child by the width of an expander-icon if it is not expandable.
   *
   * Since: 4.6
   */
  properties[PROP_INDENT_FOR_ICON] =
      g_param_spec_boolean ("indent-for-icon",
                            P_ ("Indent without expander"),
                            P_ ("If the TreeExpander should indent the child if no expander-icon is shown"),
                            TRUE,
                            G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (gobject_class, N_PROPS, properties);

  /**
   * GtkTreeExpander|listitem.expand:
   *
   * Expands the expander if it can be expanded.
   */
  gtk_widget_class_install_action (widget_class,
                                   "listitem.expand",
                                   NULL,
                                   gtk_tree_expander_expand);

  /**
   * GtkTreeExpander|listitem.collapse:
   *
   * Collapses the expander.
   */
  gtk_widget_class_install_action (widget_class,
                                   "listitem.collapse",
                                   NULL,
                                   gtk_tree_expander_collapse);

  /**
   * GtkTreeExpander|listitem.toggle-expand:
   *
   * Tries to expand the expander if it was collapsed or collapses it if
   * it was expanded.
   */
  gtk_widget_class_install_action (widget_class,
                                   "listitem.toggle-expand",
                                   NULL,
                                   gtk_tree_expander_toggle_expand);

  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_plus, 0,
                                       "listitem.expand", NULL);
  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_KP_Add, 0,
                                       "listitem.expand", NULL);
  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_asterisk, 0,
                                       "listitem.expand", NULL);
  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_KP_Multiply, 0,
                                       "listitem.expand", NULL);
  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_minus, 0,
                                       "listitem.collapse", NULL);
  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_KP_Subtract, 0,
                                       "listitem.collapse", NULL);
  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_slash, 0,
                                       "listitem.collapse", NULL);
  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_KP_Divide, 0,
                                       "listitem.collapse", NULL);

  gtk_widget_class_add_binding (widget_class, GDK_KEY_Right, GDK_SHIFT_MASK,
                                expand_collapse_right, NULL);
  gtk_widget_class_add_binding (widget_class, GDK_KEY_KP_Right, GDK_SHIFT_MASK,
                                expand_collapse_right, NULL);
  gtk_widget_class_add_binding (widget_class, GDK_KEY_Right, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                expand_collapse_right, NULL);
  gtk_widget_class_add_binding (widget_class, GDK_KEY_KP_Right, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                expand_collapse_right, NULL);
  gtk_widget_class_add_binding (widget_class, GDK_KEY_Left, GDK_SHIFT_MASK,
                                expand_collapse_left, NULL);
  gtk_widget_class_add_binding (widget_class, GDK_KEY_KP_Left, GDK_SHIFT_MASK,
                                expand_collapse_left, NULL);
  gtk_widget_class_add_binding (widget_class, GDK_KEY_Left, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                expand_collapse_left, NULL);
  gtk_widget_class_add_binding (widget_class, GDK_KEY_KP_Left, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                expand_collapse_left, NULL);

  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_space, GDK_CONTROL_MASK,
                                       "listitem.toggle-expand", NULL);
  gtk_widget_class_add_binding_action (widget_class, GDK_KEY_KP_Space, GDK_CONTROL_MASK,
                                       "listitem.toggle-expand", NULL);

#if 0
  /* These can't be implements yet. */
  gtk_widget_class_add_binding (widget_class, GDK_KEY_BackSpace, 0, go_to_parent_row, NULL, NULL);
  gtk_widget_class_add_binding (widget_class, GDK_KEY_BackSpace, GDK_CONTROL_MASK, go_to_parent_row, NULL, NULL);
#endif

  gtk_widget_class_set_layout_manager_type (widget_class, GTK_TYPE_BOX_LAYOUT);
  gtk_widget_class_set_css_name (widget_class, I_("treeexpander"));
  gtk_widget_class_set_accessible_role (widget_class, GTK_ACCESSIBLE_ROLE_GROUP);
}

static gboolean
gtk_tree_expander_expand_timeout (gpointer data)
{
  GtkTreeExpander *self = GTK_TREE_EXPANDER (data);

  if (self->list_row != NULL)
    gtk_tree_list_row_set_expanded (self->list_row, TRUE);

  self->expand_timer = 0;

  return G_SOURCE_REMOVE;
}

#define TIMEOUT_EXPAND 500

static void
gtk_tree_expander_drag_enter (GtkDropControllerMotion *motion,
                              double                   x,
                              double                   y,
                              GtkTreeExpander         *self)
{
  if (self->list_row == NULL)
    return;

  if (!gtk_tree_list_row_get_expanded (self->list_row) &&
      !self->expand_timer)
    {
      self->expand_timer = g_timeout_add (TIMEOUT_EXPAND, (GSourceFunc) gtk_tree_expander_expand_timeout, self);
      gdk_source_set_static_name_by_id (self->expand_timer, "[gtk] gtk_tree_expander_expand_timeout");
    }
}

static void
gtk_tree_expander_drag_leave (GtkDropControllerMotion *motion,
                              GtkTreeExpander         *self)
{
  if (self->expand_timer)
    {
      g_source_remove (self->expand_timer);
      self->expand_timer = 0;
    }
}

static void
gtk_tree_expander_init (GtkTreeExpander *self)
{
  GtkEventController *controller;

  gtk_widget_set_focusable (GTK_WIDGET (self), TRUE);
  self->indent_for_icon = TRUE;

  controller = gtk_drop_controller_motion_new ();
  g_signal_connect (controller, "enter", G_CALLBACK (gtk_tree_expander_drag_enter), self);
  g_signal_connect (controller, "leave", G_CALLBACK (gtk_tree_expander_drag_leave), self);
  gtk_widget_add_controller (GTK_WIDGET (self), controller);
}

/**
 * gtk_tree_expander_new:
 *
 * Creates a new `GtkTreeExpander`
 *
 * Returns: a new `GtkTreeExpander`
 **/
GtkWidget *
gtk_tree_expander_new (void)
{
  return g_object_new (GTK_TYPE_TREE_EXPANDER,
                       NULL);
}

/**
 * gtk_tree_expander_get_child: (attributes org.gtk.Method.get_property=child)
 * @self: a `GtkTreeExpander`
 *
 * Gets the child widget displayed by @self.
 *
 * Returns: (nullable) (transfer none): The child displayed by @self
 **/
GtkWidget *
gtk_tree_expander_get_child (GtkTreeExpander *self)
{
  g_return_val_if_fail (GTK_IS_TREE_EXPANDER (self), NULL);

  return self->child;
}

/**
 * gtk_tree_expander_set_child: (attributes org.gtk.Method.set_property=child)
 * @self: a `GtkTreeExpander`
 * @child: (nullable): a `GtkWidget`
 *
 * Sets the content widget to display.
 */
void
gtk_tree_expander_set_child (GtkTreeExpander *self,
                             GtkWidget       *child)
{
  g_return_if_fail (GTK_IS_TREE_EXPANDER (self));
  g_return_if_fail (child == NULL || GTK_IS_WIDGET (child));

  if (self->child == child)
    return;

  g_clear_pointer (&self->child, gtk_widget_unparent);

  if (child)
    {
      self->child = child;
      gtk_widget_set_parent (child, GTK_WIDGET (self));

      gtk_accessible_update_relation (GTK_ACCESSIBLE (self),
                                      GTK_ACCESSIBLE_RELATION_LABELLED_BY, self->child, NULL,
                                      -1);
    }
  else
    {
      gtk_accessible_reset_relation (GTK_ACCESSIBLE (self), GTK_ACCESSIBLE_RELATION_LABELLED_BY);
    }

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_CHILD]);
}

/**
 * gtk_tree_expander_get_item: (attributes org.gtk.Method.get_property=item)
 * @self: a `GtkTreeExpander`
 *
 * Forwards the item set on the `GtkTreeListRow` that @self is managing.
 *
 * This call is essentially equivalent to calling:
 *
 * ```c
 * gtk_tree_list_row_get_item (gtk_tree_expander_get_list_row (@self));
 * ```
 *
 * Returns: (nullable) (transfer full) (type GObject): The item of the row
 */
gpointer
gtk_tree_expander_get_item (GtkTreeExpander *self)
{
  g_return_val_if_fail (GTK_IS_TREE_EXPANDER (self), NULL);

  if (self->list_row == NULL)
    return NULL;

  return gtk_tree_list_row_get_item (self->list_row);
}

/**
 * gtk_tree_expander_get_list_row: (attributes org.gtk.Method.get_property=list-row)
 * @self: a `GtkTreeExpander`
 *
 * Gets the list row managed by @self.
 *
 * Returns: (nullable) (transfer none): The list row displayed by @self
 */
GtkTreeListRow *
gtk_tree_expander_get_list_row (GtkTreeExpander *self)
{
  g_return_val_if_fail (GTK_IS_TREE_EXPANDER (self), NULL);

  return self->list_row;
}

/**
 * gtk_tree_expander_set_list_row: (attributes org.gtk.Method.set_property=list-row)
 * @self: a `GtkTreeExpander` widget
 * @list_row: (nullable): a `GtkTreeListRow`
 *
 * Sets the tree list row that this expander should manage.
 */
void
gtk_tree_expander_set_list_row (GtkTreeExpander *self,
                                GtkTreeListRow  *list_row)
{
  g_return_if_fail (GTK_IS_TREE_EXPANDER (self));
  g_return_if_fail (list_row == NULL || GTK_IS_TREE_LIST_ROW (list_row));

  if (self->list_row == list_row)
    return;

  g_object_freeze_notify (G_OBJECT (self));

  gtk_tree_expander_clear_list_row (self);

  if (list_row)
    {
      self->list_row = g_object_ref (list_row);
      self->notify_handler = g_signal_connect (list_row,
                                               "notify",
                                               G_CALLBACK (gtk_tree_expander_list_row_notify_cb),
                                               self);
    }

  gtk_tree_expander_update_for_list_row (self);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_LIST_ROW]);
  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_ITEM]);

  g_object_thaw_notify (G_OBJECT (self));
}

/**
 * gtk_tree_expander_get_indent_for_icon: (attributes org.gtk.Method.get_property=indent-for-icon)
 * @self: a `GtkTreeExpander`
 *
 * TreeExpander indents the child by the width of an expander-icon if it is not expandable.
 *
 * Returns: TRUE if the child should be indented when not expandable. Otherwise FALSE.
 *
 * Since: 4.6
 */
gboolean
gtk_tree_expander_get_indent_for_icon (GtkTreeExpander *self)
{
  g_return_val_if_fail (GTK_IS_TREE_EXPANDER (self), FALSE);

  return self->indent_for_icon;
}

/**
 * gtk_tree_expander_set_indent_for_icon: (attributes org.gtk.Method.set_property=indent-for-icon)
 * @self: a `GtkTreeExpander` widget
 * @indent_for_icon: TRUE if the child should be indented without expander. Otherwise FALSE.
 *
 * Sets if the TreeExpander should indent the child by the width of an expander-icon when it is not expandable.
 *
 * Since: 4.6
 */
void
gtk_tree_expander_set_indent_for_icon (GtkTreeExpander *self,
                                       gboolean indent_for_icon)
{
  g_return_if_fail (GTK_IS_TREE_EXPANDER (self));

  if (indent_for_icon == self->indent_for_icon)
    return;

  self->indent_for_icon = indent_for_icon;

  gtk_tree_expander_update_for_list_row (self);

  g_object_notify_by_pspec (G_OBJECT (self), properties[PROP_INDENT_FOR_ICON]);
}
