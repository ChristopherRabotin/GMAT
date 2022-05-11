/* Clipboard
 *
 * GdkClipboard is used for clipboard handling. This demo shows how to
 * copy and paste text, images, colors or files to and from the clipboard.
 *
 * You can also use Drag-And-Drop to copy the data from the source to the
 * target.
 */

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <string.h>
#include "demoimage.h"

static void
copy_button_clicked (GtkStack *source_stack,
                     gpointer  user_data)
{
  GdkClipboard *clipboard;
  const char *visible_child_name;
  GtkWidget *visible_child;

  clipboard = gtk_widget_get_clipboard (GTK_WIDGET (source_stack));

  visible_child = gtk_stack_get_visible_child (source_stack);
  visible_child_name = gtk_stack_get_visible_child_name (source_stack);

  if (strcmp (visible_child_name, "Text") == 0)
    {
      gdk_clipboard_set_text (clipboard, gtk_editable_get_text (GTK_EDITABLE (visible_child)));
    }
  else if (strcmp (visible_child_name, "Image") == 0)
    {
      GtkWidget *child;

      for (child = gtk_widget_get_first_child (visible_child); child; child = gtk_widget_get_next_sibling (child))
        {
          if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (child)))
            {
              GtkWidget *image = gtk_widget_get_first_child (child);
              GdkPaintable *paintable = gtk_image_get_paintable (GTK_IMAGE (image));

              if (GDK_IS_TEXTURE (paintable))
                gdk_clipboard_set (clipboard, GDK_TYPE_TEXTURE, paintable);
              else
                gdk_clipboard_set (clipboard, GDK_TYPE_PAINTABLE, paintable);
              break;
            }
        }
    }
  else if (strcmp (visible_child_name, "Color") == 0)
    {
      GdkRGBA color;

      gtk_color_chooser_get_rgba (GTK_COLOR_CHOOSER (visible_child), &color);
      gdk_clipboard_set (clipboard, GDK_TYPE_RGBA, &color);
    }
  else if (strcmp (visible_child_name, "File") == 0)
    {
      gdk_clipboard_set (clipboard, G_TYPE_FILE, g_object_get_data (G_OBJECT (visible_child), "file"), NULL);
    }
  else
    {
      g_print ("TODO");
    }
}

static void
present_value (GtkStack     *dest_stack,
               const GValue *value)
{
  GtkWidget *child;

  if (G_VALUE_HOLDS (value, G_TYPE_FILE))
    {
      GFile *file;

      gtk_stack_set_visible_child_name (dest_stack, "File");
      child = gtk_stack_get_visible_child (dest_stack);

      file = g_value_get_object (value);
      g_object_set (child, "label", g_file_peek_path (file), NULL);
    }
  else if (G_VALUE_HOLDS (value, GDK_TYPE_RGBA))
    {
      GdkRGBA *color;

      gtk_stack_set_visible_child_name (dest_stack, "Color");
      child = gtk_widget_get_first_child (gtk_stack_get_visible_child (dest_stack));

      color = g_value_get_boxed (value);
      g_object_set (child, "rgba", color, NULL);
    }
  else if (G_VALUE_HOLDS (value, GDK_TYPE_TEXTURE) ||
           G_VALUE_HOLDS (value, GDK_TYPE_PAINTABLE))
    {
      GdkPaintable *paintable;

      gtk_stack_set_visible_child_name (dest_stack, "Image");
      child = gtk_stack_get_visible_child (dest_stack);

      paintable = g_value_get_object (value);
      g_object_set (child, "paintable", paintable, NULL);
    }
  else if (G_VALUE_HOLDS (value, G_TYPE_STRING))
    {
      gtk_stack_set_visible_child_name (dest_stack, "Text");
      child = gtk_stack_get_visible_child (dest_stack);

      gtk_label_set_label (GTK_LABEL (child), g_value_get_string (value));
    }
}

static void
paste_received (GObject      *source_object,
                GAsyncResult *result,
                gpointer      user_data)
{
  GtkStack *dest_stack = user_data;
  GdkClipboard *clipboard;
  const GValue *value;
  GError *error = NULL;

  clipboard = GDK_CLIPBOARD (source_object);

  value = gdk_clipboard_read_value_finish (clipboard, result, &error);
  if (value)
    {
      present_value (dest_stack, value);
    }
  else
    {
      g_print ("%s\n", error->message);
      g_error_free (error);
    }
}

static void
paste_button_clicked (GtkStack *dest_stack,
                      gpointer  user_data)
{
  GdkClipboard *clipboard;
  GdkContentFormats *formats;

  clipboard = gtk_widget_get_clipboard (GTK_WIDGET (dest_stack));
  formats = gdk_clipboard_get_formats (clipboard);

  if (gdk_content_formats_contain_gtype (formats, GDK_TYPE_TEXTURE))
    gdk_clipboard_read_value_async (clipboard, GDK_TYPE_TEXTURE, 0, NULL, paste_received, dest_stack);
  else if (gdk_content_formats_contain_gtype (formats, GDK_TYPE_PAINTABLE))
    gdk_clipboard_read_value_async (clipboard, GDK_TYPE_PAINTABLE, 0, NULL, paste_received, dest_stack);
  else if (gdk_content_formats_contain_gtype (formats, GDK_TYPE_RGBA))
    gdk_clipboard_read_value_async (clipboard, GDK_TYPE_RGBA, 0, NULL, paste_received, dest_stack);
  else if (gdk_content_formats_contain_gtype (formats, G_TYPE_FILE))
    gdk_clipboard_read_value_async (clipboard, G_TYPE_FILE, 0, NULL, paste_received, dest_stack);
  else if (gdk_content_formats_contain_gtype (formats, G_TYPE_STRING))
    gdk_clipboard_read_value_async (clipboard, G_TYPE_STRING, 0, NULL, paste_received, dest_stack);
}

static void
update_copy_button_sensitivity (GtkWidget *source_stack)
{
  GtkButton *copy_button;
  const char *visible_child_name;
  GtkWidget *visible_child;
  gboolean sensitive;

  copy_button = GTK_BUTTON (g_object_get_data (G_OBJECT (source_stack), "copy-button"));

  visible_child = gtk_stack_get_visible_child (GTK_STACK (source_stack));
  visible_child_name = gtk_stack_get_visible_child_name (GTK_STACK (source_stack));
  if (strcmp (visible_child_name, "Text") == 0)
    {
      sensitive = strlen (gtk_editable_get_text (GTK_EDITABLE (visible_child))) > 0;
    }
  else if (strcmp (visible_child_name, "Color") == 0 ||
           strcmp (visible_child_name, "Image") == 0)
    {
      sensitive = TRUE;
    }
  else if (strcmp (visible_child_name, "File") == 0)
    {
      sensitive = g_object_get_data (G_OBJECT (visible_child), "file") != NULL;
    }
  else
    {
      sensitive = FALSE;
    }

  gtk_widget_set_sensitive (GTK_WIDGET (copy_button), sensitive);
}

static void
source_changed_cb (GtkButton  *copy_button,
                   GParamSpec *pspec,
                   GtkWidget  *source_stack)
{
  update_copy_button_sensitivity (source_stack);
}

static void
text_changed_cb (GtkButton  *copy_button,
                 GParamSpec *pspec,
                 GtkWidget  *entry)
{
  update_copy_button_sensitivity (gtk_widget_get_ancestor (entry, GTK_TYPE_STACK));
}

static void
file_button_set_file (GtkButton *button,
                      GFile     *file)
{
  gtk_label_set_label (GTK_LABEL (gtk_button_get_child (button)), g_file_peek_path (file));
  g_object_set_data_full (G_OBJECT (button), "file", g_object_ref (file), g_object_unref);
}

static void
file_chooser_response (GtkNativeDialog *dialog,
                       int              response,
                       GtkButton       *button)
{
  gtk_native_dialog_hide (dialog);

  if (response == GTK_RESPONSE_ACCEPT)
    {
      GFile *file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));
      file_button_set_file (button, file);
      g_object_unref (file);

      update_copy_button_sensitivity (gtk_widget_get_ancestor (GTK_WIDGET (button), GTK_TYPE_STACK));
    }

  gtk_native_dialog_destroy (dialog);
}

static void
open_file_cb (GtkWidget *button)
{
  GtkFileChooserNative *chooser;

  chooser = gtk_file_chooser_native_new ("Choose a file",
                                         GTK_WINDOW (gtk_widget_get_ancestor (button, GTK_TYPE_WINDOW)),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Open",
                                         "_Cancel");

  g_signal_connect (chooser, "response", G_CALLBACK (file_chooser_response), button);
  gtk_native_dialog_show (GTK_NATIVE_DIALOG (chooser));
}

static void
update_paste_button_sensitivity (GdkClipboard *clipboard,
                                 GtkWidget    *paste_button)
{
  GdkContentFormats *formats;
  gboolean sensitive = FALSE;

  formats = gdk_clipboard_get_formats (clipboard);

  if (gdk_content_formats_contain_gtype (formats, G_TYPE_FILE) ||
      gdk_content_formats_contain_gtype (formats, GDK_TYPE_RGBA) ||
      gdk_content_formats_contain_gtype (formats, GDK_TYPE_TEXTURE) ||
      gdk_content_formats_contain_gtype (formats, GDK_TYPE_PAINTABLE) ||
      gdk_content_formats_contain_gtype (formats, G_TYPE_STRING))
    sensitive = TRUE;

  gtk_widget_set_sensitive (paste_button, sensitive);
}

static void
unset_clipboard_handler (gpointer data)
{
  GdkClipboard *clipboard = gtk_widget_get_clipboard (GTK_WIDGET (data));

  g_signal_handlers_disconnect_by_func (clipboard, update_paste_button_sensitivity, data);
}

static gboolean
on_drop (GtkStack      *dest_stack,
         const GValue  *value,
         double         x,
         double         y,
         gpointer       data)
{
  present_value (dest_stack, value);
  return TRUE;
}

static GdkContentProvider *
drag_prepare (GtkDragSource *drag_source,
              double         x,
              double         y,
              gpointer       data)
{
  GtkWidget *button;
  GValue value = G_VALUE_INIT;

  button = gtk_event_controller_get_widget (GTK_EVENT_CONTROLLER (drag_source));

  if (GTK_IS_TOGGLE_BUTTON (button))
    {
      GtkWidget *image = gtk_widget_get_first_child (button);
      GdkPaintable *paintable = gtk_image_get_paintable (GTK_IMAGE (image));

      if (GDK_IS_TEXTURE (paintable))
        {
          g_value_init (&value, GDK_TYPE_TEXTURE);
          g_value_set_object (&value, paintable);
        }
      else
        {
          g_value_init (&value, GDK_TYPE_PAINTABLE);
          g_value_set_object (&value, paintable);
        }
    }
  else
    {
      GFile *file = g_object_get_data (G_OBJECT (button), "file");

      if (file)
        {
          g_value_init (&value, G_TYPE_FILE);
          g_value_set_object (&value, file);
        }
      else
        return NULL;
    }

  return gdk_content_provider_new_for_value (&value);
}

GtkWidget *
do_clipboard (GtkWidget *do_widget)
{
  static GtkWidget *window = NULL;

  if (!window)
    {
      GtkBuilderScope *scope;
      GtkBuilder *builder;
      GtkWidget *button;

      scope = gtk_builder_cscope_new ();
      gtk_builder_cscope_add_callback_symbols (GTK_BUILDER_CSCOPE (scope),
                                               "copy_button_clicked", G_CALLBACK (copy_button_clicked),
                                               "paste_button_clicked", G_CALLBACK (paste_button_clicked),
                                               "source_changed_cb", G_CALLBACK (source_changed_cb),
                                               "text_changed_cb", G_CALLBACK (text_changed_cb),
                                               "open_file_cb", G_CALLBACK (open_file_cb),
                                               "on_drop", G_CALLBACK (on_drop),
                                               "drag_prepare", G_CALLBACK (drag_prepare),
                                               NULL);

      builder = gtk_builder_new ();
      gtk_builder_set_scope (builder, scope);
      gtk_builder_add_from_resource (builder, "/clipboard/clipboard.ui", NULL);

      window = GTK_WIDGET (gtk_builder_get_object (builder, "window"));
      g_object_add_weak_pointer (G_OBJECT (window), (gpointer *)&window);
      gtk_window_set_display (GTK_WINDOW (window), gtk_widget_get_display (do_widget));

      button = GTK_WIDGET (gtk_builder_get_object (builder, "copy_button"));
      g_object_set_data (gtk_builder_get_object (builder, "source_stack"), "copy-button", button);

      button = GTK_WIDGET (gtk_builder_get_object (builder, "paste_button"));
      g_signal_connect (gtk_widget_get_clipboard (button), "changed",
                        G_CALLBACK (update_paste_button_sensitivity), button);
      g_object_set_data_full (G_OBJECT (button), "clipboard-handler", button, unset_clipboard_handler);

      g_object_unref (builder);
      g_object_unref (scope);
    }

  if (!gtk_widget_get_visible (window))
    gtk_widget_show (window);
  else
    gtk_window_destroy (GTK_WINDOW (window));

  return window;
}
