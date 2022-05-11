/* GTK - The GIMP Toolkit
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

#define GDK_DISABLE_DEPRECATION_WARNINGS

#include "gtkimagemenuitem.h"

#include "gtkmenuitemprivate.h"
#include "gtkaccellabel.h"
#include "gtkstock.h"
#include "gtkiconfactory.h"
#include "gtkimage.h"
#include "gtkmenubar.h"
#include "gtkcontainer.h"
#include "gtkwindow.h"
#include "gtkactivatable.h"

#include "gtkintl.h"
#include "gtkprivate.h"


/**
 * SECTION:gtkimagemenuitem
 * @Short_description: A deprecated widget for a menu item with an icon
 * @Title: GtkImageMenuItem
 *
 * A GtkImageMenuItem is a menu item which has an icon next to the text label.
 *
 * This is functionally equivalent to:
 *
 * |[<!-- language="C" -->
 *   GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
 *   GtkWidget *icon = gtk_image_new_from_icon_name ("folder-music-symbolic", GTK_ICON_SIZE_MENU);
 *   GtkWidget *label = gtk_label_new ("Music");
 *   GtkWidget *menu_item = gtk_menu_item_new ();
 *
 *   gtk_container_add (GTK_CONTAINER (box), icon);
 *   gtk_container_add (GTK_CONTAINER (box), label);
 *
 *   gtk_container_add (GTK_CONTAINER (menu_item), box);
 *
 *   gtk_widget_show_all (menu_item);
 * ]|
 *
 * Note that the user may disable display of menu icons using
 * the #GtkSettings:gtk-menu-images setting, so make sure to still
 * fill in the text label. If you want to ensure that your menu items
 * show an icon you are strongly encouraged to use a #GtkMenuItem
 * with a #GtkImage instead.
 *
 * #GtkImageMenuItem has been deprecated since GTK+ 3.10. If you want to
 * display an icon in a menu item, you should use #GtkMenuItem and pack a
 * #GtkBox with a #GtkImage and a #GtkLabel instead. You should also consider
 * using #GtkBuilder and the XML #GMenu description for creating menus, by
 * following the [GMenu guide][https://developer.gnome.org/GMenu/]. You should
 * consider using icons in menu items only sparingly, and for "objects" (or
 * "nouns") elements only, like bookmarks, files, and links; "actions" (or
 * "verbs") should not have icons.
 *
 * Furthermore, if you would like to display keyboard accelerator, you must
 * pack the accel label into the box using gtk_box_pack_end() and align the
 * label, otherwise the accelerator will not display correctly. The following
 * code snippet adds a keyboard accelerator to the menu item, with a key
 * binding of Ctrl+M:
 *
 * |[<!-- language="C" -->
 *   GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
 *   GtkWidget *icon = gtk_image_new_from_icon_name ("folder-music-symbolic", GTK_ICON_SIZE_MENU);
 *   GtkWidget *label = gtk_accel_label_new ("Music");
 *   GtkWidget *menu_item = gtk_menu_item_new ();
 *   GtkAccelGroup *accel_group = gtk_accel_group_new ();
 *
 *   gtk_container_add (GTK_CONTAINER (box), icon);
 *
 *   gtk_label_set_use_underline (GTK_LABEL (label), TRUE);
 *   gtk_label_set_xalign (GTK_LABEL (label), 0.0);
 *
 *   gtk_widget_add_accelerator (menu_item, "activate", accel_group,
 *                               GDK_KEY_m, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
 *   gtk_accel_label_set_accel_widget (GTK_ACCEL_LABEL (label), menu_item);
 *
 *   gtk_box_pack_end (GTK_BOX (box), label, TRUE, TRUE, 0);
 *
 *   gtk_container_add (GTK_CONTAINER (menu_item), box);
 *
 *   gtk_widget_show_all (menu_item);
 * ]|
 */


struct _GtkImageMenuItemPrivate
{
  GtkWidget     *image;

  gchar         *label;
  guint          use_stock         : 1;
  guint          always_show_image : 1;
};

enum {
  PROP_0,
  PROP_IMAGE,
  PROP_USE_STOCK,
  PROP_ACCEL_GROUP,
  PROP_ALWAYS_SHOW_IMAGE
};

static GtkActivatableIface *parent_activatable_iface;

static void gtk_image_menu_item_destroy              (GtkWidget        *widget);
static void gtk_image_menu_item_get_preferred_width  (GtkWidget        *widget,
                                                      gint             *minimum,
                                                      gint             *natural);
static void gtk_image_menu_item_get_preferred_height (GtkWidget        *widget,
                                                      gint             *minimum,
                                                      gint             *natural);
static void gtk_image_menu_item_get_preferred_height_for_width (GtkWidget *widget,
                                                                gint       width,
                                                                gint      *minimum,
                                                                gint      *natural);
static void gtk_image_menu_item_size_allocate        (GtkWidget        *widget,
                                                      GtkAllocation    *allocation);
static void gtk_image_menu_item_map                  (GtkWidget        *widget);
static void gtk_image_menu_item_remove               (GtkContainer     *container,
                                                      GtkWidget        *child);
static void gtk_image_menu_item_toggle_size_request  (GtkMenuItem      *menu_item,
                                                      gint             *requisition);
static void gtk_image_menu_item_set_label            (GtkMenuItem      *menu_item,
                                                      const gchar      *label);
static const gchar * gtk_image_menu_item_get_label   (GtkMenuItem *menu_item);

static void gtk_image_menu_item_forall               (GtkContainer    *container,
                                                      gboolean         include_internals,
                                                      GtkCallback      callback,
                                                      gpointer         callback_data);

static void gtk_image_menu_item_finalize             (GObject         *object);
static void gtk_image_menu_item_set_property         (GObject         *object,
                                                      guint            prop_id,
                                                      const GValue    *value,
                                                      GParamSpec      *pspec);
static void gtk_image_menu_item_get_property         (GObject         *object,
                                                      guint            prop_id,
                                                      GValue          *value,
                                                      GParamSpec      *pspec);
static void gtk_image_menu_item_screen_changed       (GtkWidget       *widget,
                                                      GdkScreen       *previous_screen);

static void gtk_image_menu_item_recalculate          (GtkImageMenuItem *image_menu_item);

static void gtk_image_menu_item_activatable_interface_init (GtkActivatableIface  *iface);
static void gtk_image_menu_item_update                     (GtkActivatable       *activatable,
                                                            GtkAction            *action,
                                                            const gchar          *property_name);
static void gtk_image_menu_item_sync_action_properties     (GtkActivatable       *activatable,
                                                            GtkAction            *action);


G_DEFINE_TYPE_WITH_CODE (GtkImageMenuItem, gtk_image_menu_item, GTK_TYPE_MENU_ITEM,
                         G_ADD_PRIVATE (GtkImageMenuItem)
                         G_IMPLEMENT_INTERFACE (GTK_TYPE_ACTIVATABLE,
                                                gtk_image_menu_item_activatable_interface_init))


static void
gtk_image_menu_item_class_init (GtkImageMenuItemClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  GtkWidgetClass *widget_class = (GtkWidgetClass*) klass;
  GtkMenuItemClass *menu_item_class = (GtkMenuItemClass*) klass;
  GtkContainerClass *container_class = (GtkContainerClass*) klass;

  widget_class->destroy = gtk_image_menu_item_destroy;
  widget_class->screen_changed = gtk_image_menu_item_screen_changed;
  widget_class->get_preferred_width = gtk_image_menu_item_get_preferred_width;
  widget_class->get_preferred_height = gtk_image_menu_item_get_preferred_height;
  widget_class->get_preferred_height_for_width = gtk_image_menu_item_get_preferred_height_for_width;
  widget_class->size_allocate = gtk_image_menu_item_size_allocate;
  widget_class->map = gtk_image_menu_item_map;

  container_class->forall = gtk_image_menu_item_forall;
  container_class->remove = gtk_image_menu_item_remove;

  menu_item_class->toggle_size_request = gtk_image_menu_item_toggle_size_request;
  menu_item_class->set_label           = gtk_image_menu_item_set_label;
  menu_item_class->get_label           = gtk_image_menu_item_get_label;

  gobject_class->finalize     = gtk_image_menu_item_finalize;
  gobject_class->set_property = gtk_image_menu_item_set_property;
  gobject_class->get_property = gtk_image_menu_item_get_property;

  /**
   * GtkImageMenuItem:image:
   *
   * Child widget to appear next to the menu text.
   *
   * Deprecated: 3.10: Use a #GtkMenuItem containing a #GtkBox with
   *   a #GtkAccelLabel and a #GtkImage instead
   */
  g_object_class_install_property (gobject_class,
                                   PROP_IMAGE,
                                   g_param_spec_object ("image",
                                                        P_("Image widget"),
                                                        P_("Child widget to appear next to the menu text"),
                                                        GTK_TYPE_WIDGET,
                                                        GTK_PARAM_READWRITE | G_PARAM_DEPRECATED));
  /**
   * GtkImageMenuItem:use-stock:
   *
   * If %TRUE, the label set in the menuitem is used as a
   * stock id to select the stock item for the item.
   *
   * Since: 2.16
   *
   * Deprecated: 3.10: Use a named icon from the #GtkIconTheme instead
   */
  g_object_class_install_property (gobject_class,
                                   PROP_USE_STOCK,
                                   g_param_spec_boolean ("use-stock",
                                                         P_("Use stock"),
                                                         P_("Whether to use the label text to create a stock menu item"),
                                                         FALSE,
                                                         GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_DEPRECATED));

  /**
   * GtkImageMenuItem:always-show-image:
   *
   * If %TRUE, the menu item will always show the image, if available.
   *
   * Use this property only if the menuitem would be useless or hard to use
   * without the image.
   *
   * Since: 2.16
   *
   * Deprecated: 3.10: Use a #GtkMenuItem containing a #GtkBox with
   *   a #GtkAccelLabel and a #GtkImage instead
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ALWAYS_SHOW_IMAGE,
                                   g_param_spec_boolean ("always-show-image",
                                                         P_("Always show image"),
                                                         P_("Whether the image will always be shown"),
                                                         FALSE,
                                                         GTK_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_DEPRECATED));

  /**
   * GtkImageMenuItem:accel-group:
   *
   * The Accel Group to use for stock accelerator keys
   *
   * Since: 2.16
   *
   * Deprecated: 3.10: Use gtk_widget_add_accelerator() instead 
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ACCEL_GROUP,
                                   g_param_spec_object ("accel-group",
                                                        P_("Accel Group"),
                                                        P_("The Accel Group to use for stock accelerator keys"),
                                                        GTK_TYPE_ACCEL_GROUP,
                                                        GTK_PARAM_WRITABLE | G_PARAM_DEPRECATED));

}

static void
gtk_image_menu_item_init (GtkImageMenuItem *image_menu_item)
{
  GtkImageMenuItemPrivate *priv;

  image_menu_item->priv = gtk_image_menu_item_get_instance_private (image_menu_item);
  priv = image_menu_item->priv;

  priv->image = NULL;
  priv->use_stock = FALSE;
  priv->label  = NULL;
}

static void
gtk_image_menu_item_finalize (GObject *object)
{
  GtkImageMenuItemPrivate *priv = GTK_IMAGE_MENU_ITEM (object)->priv;

  g_free (priv->label);
  priv->label  = NULL;

  G_OBJECT_CLASS (gtk_image_menu_item_parent_class)->finalize (object);
}

static void
gtk_image_menu_item_set_property (GObject         *object,
                                  guint            prop_id,
                                  const GValue    *value,
                                  GParamSpec      *pspec)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (object);

  switch (prop_id)
    {
    case PROP_IMAGE:
      gtk_image_menu_item_set_image (image_menu_item, (GtkWidget *) g_value_get_object (value));
      break;
    case PROP_USE_STOCK:
      G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
      gtk_image_menu_item_set_use_stock (image_menu_item, g_value_get_boolean (value));
      G_GNUC_END_IGNORE_DEPRECATIONS;
      break;
    case PROP_ALWAYS_SHOW_IMAGE:
      gtk_image_menu_item_set_always_show_image (image_menu_item, g_value_get_boolean (value));
      break;
    case PROP_ACCEL_GROUP:
      G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
      gtk_image_menu_item_set_accel_group (image_menu_item, g_value_get_object (value));
      G_GNUC_END_IGNORE_DEPRECATIONS;
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_image_menu_item_get_property (GObject         *object,
                                  guint            prop_id,
                                  GValue          *value,
                                  GParamSpec      *pspec)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (object);

  switch (prop_id)
    {
    case PROP_IMAGE:
      g_value_set_object (value, gtk_image_menu_item_get_image (image_menu_item));
      break;
    case PROP_USE_STOCK:
      G_GNUC_BEGIN_IGNORE_DEPRECATIONS;
      g_value_set_boolean (value, gtk_image_menu_item_get_use_stock (image_menu_item));
      G_GNUC_END_IGNORE_DEPRECATIONS;
      break;
    case PROP_ALWAYS_SHOW_IMAGE:
      g_value_set_boolean (value, gtk_image_menu_item_get_always_show_image (image_menu_item));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static gboolean
show_image (GtkImageMenuItem *image_menu_item)
{
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;
  GtkSettings *settings = gtk_widget_get_settings (GTK_WIDGET (image_menu_item));
  gboolean show;

  if (priv->always_show_image)
    show = TRUE;
  else
    g_object_get (settings, "gtk-menu-images", &show, NULL);

  return show;
}

static void
gtk_image_menu_item_map (GtkWidget *widget)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (widget);
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;

  GTK_WIDGET_CLASS (gtk_image_menu_item_parent_class)->map (widget);

  if (priv->image)
    g_object_set (priv->image,
                  "visible", show_image (image_menu_item),
                  NULL);
}

static void
gtk_image_menu_item_destroy (GtkWidget *widget)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (widget);
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;

  if (priv->image)
    gtk_container_remove (GTK_CONTAINER (image_menu_item),
                          priv->image);

  GTK_WIDGET_CLASS (gtk_image_menu_item_parent_class)->destroy (widget);
}

static void
gtk_image_menu_item_toggle_size_request (GtkMenuItem *menu_item,
                                         gint        *requisition)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (menu_item);
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;
  GtkPackDirection pack_dir;
  GtkWidget *parent;
  GtkWidget *widget = GTK_WIDGET (menu_item);

  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    pack_dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    pack_dir = GTK_PACK_DIRECTION_LTR;

  *requisition = 0;

  if (priv->image && gtk_widget_get_visible (priv->image))
    {
      GtkRequisition image_requisition;
      guint toggle_spacing;

      gtk_widget_get_preferred_size (priv->image, &image_requisition, NULL);

      gtk_widget_style_get (GTK_WIDGET (menu_item),
                            "toggle-spacing", &toggle_spacing,
                            NULL);

      if (pack_dir == GTK_PACK_DIRECTION_LTR || pack_dir == GTK_PACK_DIRECTION_RTL)
        {
          if (image_requisition.width > 0)
            *requisition = image_requisition.width + toggle_spacing;
        }
      else
        {
          if (image_requisition.height > 0)
            *requisition = image_requisition.height + toggle_spacing;
        }
    }
}

static void
gtk_image_menu_item_recalculate (GtkImageMenuItem *image_menu_item)
{
  GtkImageMenuItemPrivate    *priv = image_menu_item->priv;
  GtkStockItem             stock_item;
  GtkWidget               *image;
  const gchar             *resolved_label = priv->label;

  if (priv->use_stock && priv->label)
    {

      G_GNUC_BEGIN_IGNORE_DEPRECATIONS;

      if (!priv->image)
        {
          image = gtk_image_new_from_stock (priv->label, GTK_ICON_SIZE_MENU);
          gtk_image_menu_item_set_image (image_menu_item, image);
        }

      if (gtk_stock_lookup (priv->label, &stock_item))
          resolved_label = stock_item.label;

      gtk_menu_item_set_use_underline (GTK_MENU_ITEM (image_menu_item), TRUE);

      G_GNUC_END_IGNORE_DEPRECATIONS;
    }

  GTK_MENU_ITEM_CLASS
    (gtk_image_menu_item_parent_class)->set_label (GTK_MENU_ITEM (image_menu_item), resolved_label);

}

static void
gtk_image_menu_item_set_label (GtkMenuItem      *menu_item,
                               const gchar      *label)
{
  GtkImageMenuItemPrivate *priv = GTK_IMAGE_MENU_ITEM (menu_item)->priv;

  if (priv->label != label)
    {
      g_free (priv->label);
      priv->label = g_strdup (label);

      gtk_image_menu_item_recalculate (GTK_IMAGE_MENU_ITEM (menu_item));

      g_object_notify (G_OBJECT (menu_item), "label");

    }
}

static const gchar *
gtk_image_menu_item_get_label (GtkMenuItem *menu_item)
{
  GtkImageMenuItemPrivate *priv = GTK_IMAGE_MENU_ITEM (menu_item)->priv;

  return priv->label;
}

static void
gtk_image_menu_item_get_preferred_width (GtkWidget        *widget,
                                         gint             *minimum,
                                         gint             *natural)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (widget);
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;
  GtkPackDirection pack_dir;
  GtkWidget *parent;

  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    pack_dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    pack_dir = GTK_PACK_DIRECTION_LTR;

  GTK_WIDGET_CLASS (gtk_image_menu_item_parent_class)->get_preferred_width (widget, minimum, natural);

  if ((pack_dir == GTK_PACK_DIRECTION_TTB || pack_dir == GTK_PACK_DIRECTION_BTT) &&
      priv->image &&
      gtk_widget_get_visible (priv->image))
    {
      gint child_minimum, child_natural;

      gtk_widget_get_preferred_width (priv->image, &child_minimum, &child_natural);

      *minimum = MAX (*minimum, child_minimum);
      *natural = MAX (*natural, child_natural);
    }
}

static void
gtk_image_menu_item_get_preferred_height (GtkWidget        *widget,
                                          gint             *minimum,
                                          gint             *natural)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (widget);
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;
  gint child_height = 0;
  GtkPackDirection pack_dir;
  GtkWidget *parent;

  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    pack_dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    pack_dir = GTK_PACK_DIRECTION_LTR;

  if (priv->image && gtk_widget_get_visible (priv->image))
    {
      GtkRequisition child_requisition;

      gtk_widget_get_preferred_size (priv->image, &child_requisition, NULL);

      child_height = child_requisition.height;
    }

  GTK_WIDGET_CLASS (gtk_image_menu_item_parent_class)->get_preferred_height (widget, minimum, natural);

  if (pack_dir == GTK_PACK_DIRECTION_RTL || pack_dir == GTK_PACK_DIRECTION_LTR)
    {
      *minimum = MAX (*minimum, child_height);
      *natural = MAX (*natural, child_height);
    }
}

static void
gtk_image_menu_item_get_preferred_height_for_width (GtkWidget        *widget,
                                                    gint              width,
                                                    gint             *minimum,
                                                    gint             *natural)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (widget);
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;
  gint child_height = 0;
  GtkPackDirection pack_dir;
  GtkWidget *parent;

  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    pack_dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    pack_dir = GTK_PACK_DIRECTION_LTR;

  if (priv->image && gtk_widget_get_visible (priv->image))
    {
      GtkRequisition child_requisition;

      gtk_widget_get_preferred_size (priv->image, &child_requisition, NULL);

      child_height = child_requisition.height;
    }

  GTK_WIDGET_CLASS
    (gtk_image_menu_item_parent_class)->get_preferred_height_for_width (widget, width, minimum, natural);

  if (pack_dir == GTK_PACK_DIRECTION_RTL || pack_dir == GTK_PACK_DIRECTION_LTR)
    {
      *minimum = MAX (*minimum, child_height);
      *natural = MAX (*natural, child_height);
    }
}


static void
gtk_image_menu_item_size_allocate (GtkWidget     *widget,
                                   GtkAllocation *allocation)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (widget);
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;
  GtkAllocation widget_allocation;
  GtkPackDirection pack_dir;
  GtkWidget *parent;

  parent = gtk_widget_get_parent (widget);

  if (GTK_IS_MENU_BAR (parent))
    pack_dir = gtk_menu_bar_get_child_pack_direction (GTK_MENU_BAR (parent));
  else
    pack_dir = GTK_PACK_DIRECTION_LTR;

  GTK_WIDGET_CLASS (gtk_image_menu_item_parent_class)->size_allocate (widget, allocation);

  if (priv->image && gtk_widget_get_visible (priv->image))
    {
      gint x, y, offset;
      GtkStyleContext *context;
      GtkStateFlags state;
      GtkBorder padding;
      GtkRequisition child_requisition;
      GtkAllocation child_allocation;
      guint toggle_spacing;
      gint toggle_size;

      toggle_size = GTK_MENU_ITEM (image_menu_item)->priv->toggle_size;
      gtk_widget_style_get (widget,
                            "toggle-spacing", &toggle_spacing,
                            NULL);

      /* Man this is lame hardcoding action, but I can't
       * come up with a solution that's really better.
       */

      gtk_widget_get_preferred_size (priv->image, &child_requisition, NULL);

      gtk_widget_get_allocation (widget, &widget_allocation);

      context = gtk_widget_get_style_context (widget);
      state = gtk_widget_get_state_flags (widget);
      gtk_style_context_get_padding (context, state, &padding);
      offset = gtk_container_get_border_width (GTK_CONTAINER (image_menu_item));

      if (pack_dir == GTK_PACK_DIRECTION_LTR ||
          pack_dir == GTK_PACK_DIRECTION_RTL)
        {
          if ((gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR) ==
              (pack_dir == GTK_PACK_DIRECTION_LTR))
            x = offset + padding.left +
               (toggle_size - toggle_spacing - child_requisition.width) / 2;
          else
            x = widget_allocation.width - offset - padding.right -
              toggle_size + toggle_spacing +
              (toggle_size - toggle_spacing - child_requisition.width) / 2;

          y = (widget_allocation.height - child_requisition.height) / 2;
        }
      else
        {
          if ((gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR) ==
              (pack_dir == GTK_PACK_DIRECTION_TTB))
            y = offset + padding.top +
              (toggle_size - toggle_spacing - child_requisition.height) / 2;
          else
            y = widget_allocation.height - offset - padding.bottom -
              toggle_size + toggle_spacing +
              (toggle_size - toggle_spacing - child_requisition.height) / 2;

          x = (widget_allocation.width - child_requisition.width) / 2;
        }

      child_allocation.width = child_requisition.width;
      child_allocation.height = child_requisition.height;
      child_allocation.x = widget_allocation.x + MAX (x, 0);
      child_allocation.y = widget_allocation.y + MAX (y, 0);

      gtk_widget_size_allocate (priv->image, &child_allocation);
    }
}

static void
gtk_image_menu_item_forall (GtkContainer   *container,
                            gboolean        include_internals,
                            GtkCallback     callback,
                            gpointer        callback_data)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (container);
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;

  GTK_CONTAINER_CLASS (gtk_image_menu_item_parent_class)->forall (container,
                                                                  include_internals,
                                                                  callback,
                                                                  callback_data);

  if (include_internals && priv->image)
    (* callback) (priv->image, callback_data);
}


static void
gtk_image_menu_item_activatable_interface_init (GtkActivatableIface  *iface)
{
  parent_activatable_iface = g_type_interface_peek_parent (iface);
  iface->update = gtk_image_menu_item_update;
  iface->sync_action_properties = gtk_image_menu_item_sync_action_properties;
}

static GtkWidget *
gtk_image_menu_item_ensure_image (GtkImageMenuItem *item)
{
  GtkWidget *image;

  image = gtk_image_menu_item_get_image (item);
  if (!GTK_IS_IMAGE (image))
    {
      image = gtk_image_new ();
      gtk_widget_show (image);
      gtk_image_menu_item_set_image (item, image);
    }

  return image;
}

static gboolean
activatable_update_stock_id (GtkImageMenuItem *image_menu_item, GtkAction *action)
{
  const gchar *stock_id  = gtk_action_get_stock_id (action);

  G_GNUC_BEGIN_IGNORE_DEPRECATIONS;

  if (stock_id && gtk_icon_factory_lookup_default (stock_id))
    {
      GtkWidget *image;

      image = gtk_image_menu_item_ensure_image (image_menu_item);
      gtk_image_set_from_stock (GTK_IMAGE (image), stock_id, GTK_ICON_SIZE_MENU);
      return TRUE;
    }

  G_GNUC_END_IGNORE_DEPRECATIONS;

  return FALSE;
}

static gboolean
activatable_update_gicon (GtkImageMenuItem *image_menu_item, GtkAction *action)
{
  GIcon       *icon = gtk_action_get_gicon (action);
  const gchar *stock_id;
  gboolean     ret = FALSE;

  stock_id = gtk_action_get_stock_id (action);

  G_GNUC_BEGIN_IGNORE_DEPRECATIONS;

  if (icon && !(stock_id && gtk_icon_factory_lookup_default (stock_id)))
    {
      GtkWidget *image;

      image = gtk_image_menu_item_ensure_image (image_menu_item);
      gtk_image_set_from_gicon (GTK_IMAGE (image), icon, GTK_ICON_SIZE_MENU);
      ret = TRUE;
    }

  G_GNUC_END_IGNORE_DEPRECATIONS;

  return ret;
}

static gboolean
activatable_update_icon_name (GtkImageMenuItem *image_menu_item, GtkAction *action)
{
  const gchar *icon_name = gtk_action_get_icon_name (action);

  if (icon_name)
    {
      GtkWidget *image;

      image = gtk_image_menu_item_ensure_image (image_menu_item);
      gtk_image_set_from_icon_name (GTK_IMAGE (image), icon_name, GTK_ICON_SIZE_MENU);
      return TRUE;
    }

  return FALSE;
}

static void
gtk_image_menu_item_update (GtkActivatable *activatable,
                            GtkAction      *action,
                            const gchar    *property_name)
{
  GtkImageMenuItem *image_menu_item;
  gboolean   use_appearance;

  image_menu_item = GTK_IMAGE_MENU_ITEM (activatable);

  parent_activatable_iface->update (activatable, action, property_name);

  use_appearance = gtk_activatable_get_use_action_appearance (activatable);
  if (!use_appearance)
    return;

  if (strcmp (property_name, "stock-id") == 0)
    activatable_update_stock_id (image_menu_item, action);
  else if (strcmp (property_name, "gicon") == 0)
    activatable_update_gicon (image_menu_item, action);
  else if (strcmp (property_name, "icon-name") == 0)
    activatable_update_icon_name (image_menu_item, action);
}

static void
gtk_image_menu_item_sync_action_properties (GtkActivatable *activatable,
                                            GtkAction      *action)
{
  GtkImageMenuItem *image_menu_item;
  gboolean   use_appearance;

  image_menu_item = GTK_IMAGE_MENU_ITEM (activatable);

  parent_activatable_iface->sync_action_properties (activatable, action);

  if (!action)
    return;

  use_appearance = gtk_activatable_get_use_action_appearance (activatable);
  if (!use_appearance)
    return;

  if (!activatable_update_stock_id (image_menu_item, action) &&
      !activatable_update_gicon (image_menu_item, action))
    activatable_update_icon_name (image_menu_item, action);

  gtk_image_menu_item_set_always_show_image (image_menu_item,
                                             gtk_action_get_always_show_image (action));
}


/**
 * gtk_image_menu_item_new:
 *
 * Creates a new #GtkImageMenuItem with an empty label.
 *
 * Returns: a new #GtkImageMenuItem
 *
 * Deprecated: 3.10: Use gtk_menu_item_new() instead.
 */
GtkWidget*
gtk_image_menu_item_new (void)
{
  return g_object_new (GTK_TYPE_IMAGE_MENU_ITEM, NULL);
}

/**
 * gtk_image_menu_item_new_with_label:
 * @label: the text of the menu item.
 *
 * Creates a new #GtkImageMenuItem containing a label.
 *
 * Returns: a new #GtkImageMenuItem.
 *
 * Deprecated: 3.10: Use gtk_menu_item_new_with_label() instead.
 */
GtkWidget*
gtk_image_menu_item_new_with_label (const gchar *label)
{
  return g_object_new (GTK_TYPE_IMAGE_MENU_ITEM,
                       "label", label,
                       NULL);
}

/**
 * gtk_image_menu_item_new_with_mnemonic:
 * @label: the text of the menu item, with an underscore in front of the
 *         mnemonic character
 *
 * Creates a new #GtkImageMenuItem containing a label. The label
 * will be created using gtk_label_new_with_mnemonic(), so underscores
 * in @label indicate the mnemonic for the menu item.
 *
 * Returns: a new #GtkImageMenuItem
 *
 * Deprecated: 3.10: Use gtk_menu_item_new_with_mnemonic() instead.
 */
GtkWidget*
gtk_image_menu_item_new_with_mnemonic (const gchar *label)
{
  return g_object_new (GTK_TYPE_IMAGE_MENU_ITEM,
                       "use-underline", TRUE,
                       "label", label,
                       NULL);
}

/**
 * gtk_image_menu_item_new_from_stock:
 * @stock_id: the name of the stock item.
 * @accel_group: (allow-none): the #GtkAccelGroup to add the menu items
 *   accelerator to, or %NULL.
 *
 * Creates a new #GtkImageMenuItem containing the image and text from a
 * stock item. Some stock ids have preprocessor macros like #GTK_STOCK_OK
 * and #GTK_STOCK_APPLY.
 *
 * If you want this menu item to have changeable accelerators, then pass in
 * %NULL for accel_group. Next call gtk_menu_item_set_accel_path() with an
 * appropriate path for the menu item, use gtk_stock_lookup() to look up the
 * standard accelerator for the stock item, and if one is found, call
 * gtk_accel_map_add_entry() to register it.
 *
 * Returns: a new #GtkImageMenuItem.
 *
 * Deprecated: 3.10: Use gtk_menu_item_new_with_mnemonic() instead.
 */
GtkWidget*
gtk_image_menu_item_new_from_stock (const gchar   *stock_id,
                                    GtkAccelGroup *accel_group)
{
  return g_object_new (GTK_TYPE_IMAGE_MENU_ITEM,
                       "label", stock_id,
                       "use-stock", TRUE,
                       "accel-group", accel_group,
                       NULL);
}

/**
 * gtk_image_menu_item_set_use_stock:
 * @image_menu_item: a #GtkImageMenuItem
 * @use_stock: %TRUE if the menuitem should use a stock item
 *
 * If %TRUE, the label set in the menuitem is used as a
 * stock id to select the stock item for the item.
 *
 * Since: 2.16
 *
 * Deprecated: 3.10
 */
void
gtk_image_menu_item_set_use_stock (GtkImageMenuItem *image_menu_item,
                                   gboolean          use_stock)
{
  GtkImageMenuItemPrivate *priv;

  g_return_if_fail (GTK_IS_IMAGE_MENU_ITEM (image_menu_item));

  priv = image_menu_item->priv;

  if (priv->use_stock != use_stock)
    {
      priv->use_stock = use_stock;

      gtk_image_menu_item_recalculate (image_menu_item);

      g_object_notify (G_OBJECT (image_menu_item), "use-stock");
    }
}

/**
 * gtk_image_menu_item_get_use_stock:
 * @image_menu_item: a #GtkImageMenuItem
 *
 * Checks whether the label set in the menuitem is used as a
 * stock id to select the stock item for the item.
 *
 * Returns: %TRUE if the label set in the menuitem is used as a
 *     stock id to select the stock item for the item
 *
 * Since: 2.16
 *
 * Deprecated: 3.10
 */
gboolean
gtk_image_menu_item_get_use_stock (GtkImageMenuItem *image_menu_item)
{
  g_return_val_if_fail (GTK_IS_IMAGE_MENU_ITEM (image_menu_item), FALSE);

  return image_menu_item->priv->use_stock;
}

/**
 * gtk_image_menu_item_set_always_show_image:
 * @image_menu_item: a #GtkImageMenuItem
 * @always_show: %TRUE if the menuitem should always show the image
 *
 * If %TRUE, the menu item will ignore the #GtkSettings:gtk-menu-images
 * setting and always show the image, if available.
 *
 * Use this property if the menuitem would be useless or hard to use
 * without the image.
 *
 * Since: 2.16
 *
 * Deprecated: 3.10
 */
void
gtk_image_menu_item_set_always_show_image (GtkImageMenuItem *image_menu_item,
                                           gboolean          always_show)
{
  GtkImageMenuItemPrivate *priv;

  g_return_if_fail (GTK_IS_IMAGE_MENU_ITEM (image_menu_item));

  priv = image_menu_item->priv;

  if (priv->always_show_image != always_show)
    {
      priv->always_show_image = always_show;

      if (priv->image)
        {
          if (show_image (image_menu_item))
            gtk_widget_show (priv->image);
          else
            gtk_widget_hide (priv->image);
        }

      g_object_notify (G_OBJECT (image_menu_item), "always-show-image");
    }
}

/**
 * gtk_image_menu_item_get_always_show_image:
 * @image_menu_item: a #GtkImageMenuItem
 *
 * Returns whether the menu item will ignore the #GtkSettings:gtk-menu-images
 * setting and always show the image, if available.
 *
 * Returns: %TRUE if the menu item will always show the image
 *
 * Since: 2.16
 *
 * Deprecated: 3.10
 */
gboolean
gtk_image_menu_item_get_always_show_image (GtkImageMenuItem *image_menu_item)
{
  g_return_val_if_fail (GTK_IS_IMAGE_MENU_ITEM (image_menu_item), FALSE);

  return image_menu_item->priv->always_show_image;
}


/**
 * gtk_image_menu_item_set_accel_group:
 * @image_menu_item: a #GtkImageMenuItem
 * @accel_group: the #GtkAccelGroup
 *
 * Specifies an @accel_group to add the menu items accelerator to
 * (this only applies to stock items so a stock item must already
 * be set, make sure to call gtk_image_menu_item_set_use_stock()
 * and gtk_menu_item_set_label() with a valid stock item first).
 *
 * If you want this menu item to have changeable accelerators then
 * you shouldnt need this (see gtk_image_menu_item_new_from_stock()).
 *
 * Since: 2.16
 *
 * Deprecated: 3.10
 */
void
gtk_image_menu_item_set_accel_group (GtkImageMenuItem *image_menu_item,
                                     GtkAccelGroup    *accel_group)
{
  GtkImageMenuItemPrivate    *priv;
  GtkStockItem             stock_item;

  /* Silent return for the constructor */
  if (!accel_group)
    return;

  g_return_if_fail (GTK_IS_IMAGE_MENU_ITEM (image_menu_item));
  g_return_if_fail (GTK_IS_ACCEL_GROUP (accel_group));

  priv = image_menu_item->priv;

  G_GNUC_BEGIN_IGNORE_DEPRECATIONS;

  if (priv->use_stock && priv->label && gtk_stock_lookup (priv->label, &stock_item))
    if (stock_item.keyval)
      {
        gtk_widget_add_accelerator (GTK_WIDGET (image_menu_item),
                                    "activate",
                                    accel_group,
                                    stock_item.keyval,
                                    stock_item.modifier,
                                    GTK_ACCEL_VISIBLE);

        g_object_notify (G_OBJECT (image_menu_item), "accel-group");
      }

  G_GNUC_END_IGNORE_DEPRECATIONS;

}

/**
 * gtk_image_menu_item_set_image:
 * @image_menu_item: a #GtkImageMenuItem.
 * @image: (allow-none): a widget to set as the image for the menu item.
 *
 * Sets the image of @image_menu_item to the given widget.
 * Note that it depends on the show-menu-images setting whether
 * the image will be displayed or not.
 *
 * Deprecated: 3.10
 */
void
gtk_image_menu_item_set_image (GtkImageMenuItem *image_menu_item,
                               GtkWidget        *image)
{
  GtkImageMenuItemPrivate *priv;

  g_return_if_fail (GTK_IS_IMAGE_MENU_ITEM (image_menu_item));

  priv = image_menu_item->priv;

  if (image == priv->image)
    return;

  if (priv->image)
    gtk_container_remove (GTK_CONTAINER (image_menu_item),
                          priv->image);

  priv->image = image;

  if (image == NULL)
    return;

  gtk_widget_set_parent (image, GTK_WIDGET (image_menu_item));
  g_object_set (image,
                "visible", show_image (image_menu_item),
                "no-show-all", TRUE,
                NULL);
  gtk_image_set_pixel_size (GTK_IMAGE (image), 16);

  g_object_notify (G_OBJECT (image_menu_item), "image");
}

/**
 * gtk_image_menu_item_get_image:
 * @image_menu_item: a #GtkImageMenuItem
 *
 * Gets the widget that is currently set as the image of @image_menu_item.
 * See gtk_image_menu_item_set_image().
 *
 * Returns: (transfer none): the widget set as image of @image_menu_item
 *
 * Deprecated: 3.10
 **/
GtkWidget*
gtk_image_menu_item_get_image (GtkImageMenuItem *image_menu_item)
{
  g_return_val_if_fail (GTK_IS_IMAGE_MENU_ITEM (image_menu_item), NULL);

  return image_menu_item->priv->image;
}

static void
gtk_image_menu_item_remove (GtkContainer *container,
                            GtkWidget    *child)
{
  GtkImageMenuItem *image_menu_item = GTK_IMAGE_MENU_ITEM (container);
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;

  if (child == priv->image)
    {
      gboolean widget_was_visible;

      widget_was_visible = gtk_widget_get_visible (child);

      gtk_widget_unparent (child);
      priv->image = NULL;

      if (widget_was_visible &&
          gtk_widget_get_visible (GTK_WIDGET (container)))
        gtk_widget_queue_resize (GTK_WIDGET (container));

      g_object_notify (G_OBJECT (image_menu_item), "image");
    }
  else
    {
      GTK_CONTAINER_CLASS (gtk_image_menu_item_parent_class)->remove (container, child);
    }
}

static void
show_image_change_notify (GtkImageMenuItem *image_menu_item)
{
  GtkImageMenuItemPrivate *priv = image_menu_item->priv;

  if (priv->image)
    {
      if (show_image (image_menu_item))
        gtk_widget_show (priv->image);
      else
        gtk_widget_hide (priv->image);
    }
}

static void
traverse_container (GtkWidget *widget,
                    gpointer   data)
{
  if (GTK_IS_IMAGE_MENU_ITEM (widget))
    show_image_change_notify (GTK_IMAGE_MENU_ITEM (widget));
  else if (GTK_IS_CONTAINER (widget))
    gtk_container_forall (GTK_CONTAINER (widget), traverse_container, NULL);
}

static void
gtk_image_menu_item_setting_changed (GtkSettings *settings)
{
  GList *list, *l;

  list = gtk_window_list_toplevels ();

  for (l = list; l; l = l->next)
    gtk_container_forall (GTK_CONTAINER (l->data),
                          traverse_container, NULL);

  g_list_free (list);
}

static void
gtk_image_menu_item_screen_changed (GtkWidget *widget,
                                    GdkScreen *previous_screen)
{
  GtkSettings *settings;
  gulong show_image_connection;

  if (!gtk_widget_has_screen (widget))
    return;

  settings = gtk_widget_get_settings (widget);

  show_image_connection =
    g_signal_handler_find (settings, G_SIGNAL_MATCH_FUNC, 0, 0,
                           NULL, gtk_image_menu_item_setting_changed, NULL);

  if (show_image_connection)
    return;

  g_signal_connect (settings, "notify::gtk-menu-images",
                    G_CALLBACK (gtk_image_menu_item_setting_changed), NULL);

  show_image_change_notify (GTK_IMAGE_MENU_ITEM (widget));
}
