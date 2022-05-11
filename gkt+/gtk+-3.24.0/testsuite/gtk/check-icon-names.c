#include <gtk/gtk.h>

static char *icon_names[] = {
  /*** stock icons, from gtkiconfactory.c:get_default_icons() ***/
  "dialog-password",
  "dialog-error",
  "dialog-information",
  "dialog-question",
  "dialog-warning",
  "gtk-dnd", /* internal icon */
  "gtk-dnd-multiple", /* internal icon */
  "gtk-apply", /* deprecated stock id */
  "gtk-cancel", /* deprecated stock id */
  "gtk-no", /* deprecated stock id */
  "gtk-ok", /* deprecated stock id */
  "gtk-yes", /* deprecated stock id */
  "window-close",
  "list-add",
  "format-justify-center",
  "format-justify-fill",
  "format-justify-left",
  "format-justify-right",
  "go-bottom",
  "media-optical",
  "gtk-convert", /* deprecated stock id */
  "edit-copy",
  "edit-cut",
  "go-down",
  "system-run",
  "application-exit",
  "go-first",
  "gtk-select-font", /* deprecated stock id */
  "view-fullscreen",
  "view-restore",
  "drive-harddisk",
  "help-contents",
  "go-home",
  "go-jump",
  "go-last",
  "go-previous",
  "image-missing",
  "network-idle",
  "document-new",
  "document-open",
  "gtk-orientation-portrait", /* internal icon */
  "gtk-orientation-landscape", /* internal icon */
  "gtk-orientation-reverse-portrait", /* internal icon */
  "gtk-orientation-reverse-landscape", /* internal icon */
  "gtk-page-setup", /* internal icon */
  "edit-paste",
  "gtk-preferences", /* deprecated stock id */
  "document-print",
  "document-print-preview",
  "printer-error",
  /* "printer-paused", */
  /* "printer-info", */
  /* "printer-warning", */
  "document-properties",
  "edit-redo",
  "list-remove",
  "view-refresh",
  "document-revert",
  "go-next",
  "document-save",
  "media-floppy",
  "document-save-as",
  "edit-find",
  "edit-find-replace",
  "view-sort-descending",
  "view-sort-ascending",
  "tools-check-spelling",
  "process-stop",
  "format-text-bold",
  "format-text-italic",
  "format-text-strikethrough",
  "format-text-underline",
  "format-indent-more",
  "format-indent-less",
  "go-top",
  "edit-delete",
  "gtk-undelete", /* deprecated stock id */
  "edit-undo",
  "go-up",
  "text-x-generic",
  "folder",
  "help-about",
  "gtk-connect", /* deprecated stock id */
  "gtk-disconnect", /* deprecated stock id */
  "gtk-edit", /* deprecated stock id */
  "gtk-caps-lock-warning", /* internal icon */
  "media-seek-forward",
  "media-skip-forward",
  "media-playback-pause",
  "media-playback-start",
  "media-skip-backward",
  "media-record",
  "media-seek-backward",
  "media-playback-stop",
  "gtk-index", /* deprecated stock id */
  "zoom-original",
  "zoom-in",
  "zoom-out",
  "zoom-fit-best",
  "edit-select-all",
  "edit-clear",
  "gtk-select-color", /* deprecated stock id */
  "gtk-color-picker", /* deprecated stock id */

  /*** Icons used in code or templates, sorted alphabetically ***/
  "audio-volume-high",
  "audio-volume-high-symbolic",
  "audio-volume-low",
  "audio-volume-low-symbolic",
  "audio-volume-medium",
  "audio-volume-medium-symbolic",
  "audio-volume-muted",
  "audio-volume-muted-symbolic",
  "changes-allow-symbolic",
  "changes-prevent-symbolic",
  "dialog-password-symbolic",
  "dialog-warning-symbolic",
  "document-open-symbolic",
  "edit-clear-symbolic",
  "edit-find-symbolic",
  "list-add-symbolic",
  "list-remove-symbolic",
  "open-menu-symbolic",
  "pan-down-symbolic",
  "pan-end-symbolic",
  "pan-start-symbolic",
  "pan-up-symbolic",
  "user-trash-full-symbolic",
  "user-trash-symbolic",
  "window-close-symbolic",
  "window-maximize-symbolic",
  "window-minimize-symbolic",
  "window-restore-symbolic"
};

static void
test_icon_existence (gconstpointer icon_name)
{
  GtkIconInfo *info;

  /* Not using generic fallback and builtins here, as we explicitly want to check the
   * icon theme.
   * The icon size is randomly chosen.
   */
  info = gtk_icon_theme_lookup_icon (gtk_icon_theme_get_default (), icon_name, 16, GTK_ICON_LOOKUP_DIR_LTR);
  if (info == NULL)
    {
      g_test_message ("Failed to look up icon for \"%s\"", (char *) icon_name);
      g_test_fail ();
      return;
    }

  g_object_unref (info);
}

int
main (int argc, char *argv[])
{
  guint i;
  char *test_name;
  char *theme;

  gtk_test_init (&argc, &argv);

  g_object_get (gtk_settings_get_default (), "gtk-icon-theme-name", &theme, NULL);
  g_test_message ("Testing icon theme: %s", theme);
  g_free (theme);

  for (i = 0; i < G_N_ELEMENTS (icon_names); i++)
    {
      test_name = g_strdup_printf ("/check-icon-names/%s", icon_names[i]);
      g_test_add_data_func (test_name, icon_names[i], test_icon_existence);
      g_free (test_name);
    }

  return g_test_run();
}
