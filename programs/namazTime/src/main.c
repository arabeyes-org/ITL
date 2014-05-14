#ifdef HAVE_CONFIG_H
   #include <config.h>
#endif

#include "times.h"
#include <gtk/gtk.h>
#include "eggtrayicon.h"

#include "interface.h"
#include "support.h"
#include "callbacks.h"

#define TRAYICON_IMAGE "qibla.png"

extern int dst;
extern int method;
extern int remind;
extern double gmtDiff;
extern double seaLevel;
extern double latitude;
extern double longitude;
extern char* locationName;
GtkWidget* mainWindow, *prefsWindow;

static void on_prefsDialog_response(GtkDialog* dialog, gint response){
   GtkWidget* entry, *widget, *button;
 
   widget = prefsWindow;
   gtk_widget_hide(widget);

   if (response == GTK_RESPONSE_OK){
      char* tmp;
      entry = lookup_widget(GTK_WIDGET(widget), "locName_entry");
      locationName = g_strdup(gtk_entry_get_text((GtkEntry*)entry));
      entry = lookup_widget(GTK_WIDGET(widget), "lat_entry");
      latitude = g_ascii_strtod(gtk_entry_get_text((GtkEntry*)entry), NULL);
      entry = lookup_widget(GTK_WIDGET(widget), "long_entry");
      longitude = g_ascii_strtod(gtk_entry_get_text((GtkEntry*)entry), NULL);
      entry = lookup_widget(GTK_WIDGET(widget), "gmtdiff_entry");
      gmtDiff = g_ascii_strtod(gtk_entry_get_text((GtkEntry*)entry), NULL);
      entry = lookup_widget(GTK_WIDGET(widget), "sealevel_entry");
      seaLevel = g_ascii_strtod(gtk_entry_get_text((GtkEntry*)entry), NULL);

      button = lookup_widget(GTK_WIDGET(widget), "dst_checkbox");
      dst = (gtk_toggle_button_get_active((GtkToggleButton*)button) == 
                                          TRUE)? 1 : 0;
      button = lookup_widget(GTK_WIDGET(widget), "mathhab");
      method = gtk_combo_box_get_active((GtkComboBox*)button);
      button = lookup_widget(GTK_WIDGET(widget), "reminderOptionBox");
      remind = (gtk_toggle_button_get_active((GtkToggleButton*)button) == 
                                             TRUE)? 1 : 0;
      reCalculateTimes();
      saveSettings();
   }
   gtk_widget_destroy(widget);
}

static void prefs_window(){
   char* tmp;
   GtkWidget* widget, *entry, *button;
   widget = create_prefsWindow();

   entry = lookup_widget(GTK_WIDGET(widget), "locName_entry");
   gtk_entry_set_text((GtkEntry*)entry, locationName);
   entry = lookup_widget(GTK_WIDGET(widget), "lat_entry");
   asprintf(&tmp, "%f", latitude);
   gtk_entry_set_text((GtkEntry*)entry, tmp);
   free(tmp);
   asprintf(&tmp, "%f", longitude);
   entry = lookup_widget(GTK_WIDGET(widget), "long_entry");
   gtk_entry_set_text((GtkEntry*)entry, tmp);
   free(tmp);
   asprintf(&tmp, "%f", gmtDiff);
   entry = lookup_widget(GTK_WIDGET(widget), "gmtdiff_entry");
   gtk_entry_set_text((GtkEntry*)entry, tmp);
   free(tmp);
   asprintf(&tmp, "%f", seaLevel);
   entry = lookup_widget(GTK_WIDGET(widget), "sealevel_entry");
   gtk_entry_set_text((GtkEntry*)entry, tmp);
   free(tmp);
   
   button = lookup_widget(GTK_WIDGET(widget), "dst_checkbox");
   gtk_toggle_button_set_active((GtkToggleButton*)button, 
                                (dst==0)? FALSE : TRUE);

   button = lookup_widget(GTK_WIDGET(widget), "reminderOptionBox");
   gtk_toggle_button_set_active((GtkToggleButton*)button, 
                                (remind==0)? FALSE : TRUE);
   button = lookup_widget(GTK_WIDGET(widget), "mathhab");
   gtk_combo_box_set_active((GtkComboBox*)button, method);
   gtk_widget_show(widget);

   prefsWindow = widget;
}

static void on_about_response(GtkWindow* window, gint response){
   gtk_widget_hide((GtkWidget*)window);
   gtk_widget_destroy(window);
}

static void about_window(){
   GtkWidget* window;
   window = create_aboutWindow();
   g_signal_connect(window, "button-press-event", 
                    G_CALLBACK(on_about_response), NULL);
   gtk_widget_show(window);
}

static void show_menu(GdkEventButton* event){
   GtkWidget* menu, *item;

   menu = gtk_menu_new();
   item = gtk_image_menu_item_new_from_stock(GTK_STOCK_PREFERENCES, NULL);
   g_signal_connect_swapped(item, "activate", G_CALLBACK(prefs_window), NULL);
   gtk_menu_attach(GTK_MENU(menu), item, 0, 1, 0, 1);
   gtk_widget_show(item);
   item = gtk_image_menu_item_new_from_stock(GTK_STOCK_DIALOG_INFO, NULL);
   g_signal_connect_swapped(item, "activate", G_CALLBACK(about_window), NULL);
   gtk_widget_show(item);
   gtk_menu_attach(GTK_MENU(menu), item, 0, 1, 1, 2);
   item = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
   g_signal_connect_swapped(item, "activate", G_CALLBACK(gtk_main_quit), NULL);
   gtk_menu_attach(GTK_MENU(menu), item, 0, 1, 2, 3);
   gtk_widget_show(item);
   gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 
                  event->button, event->time);
}

static void icon_clicked_cb(GtkWidget* button, 
                            GdkEventButton* event, 
                            gpointer data){
   if (event->type != GDK_BUTTON_PRESS)
      return;
   if (event->button == 1)
      toggle_window(GTK_WIDGET(button), event);
   else if (event->button == 3)
      show_menu(event);
}

int main(int argc, char** argv){
   GdkPixbuf* pixbuf;
   GtkWidget* box, *image;
   EggTrayIcon* tray_icon;

#ifdef ENABLE_NLS
   bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
   bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
   textdomain (GETTEXT_PACKAGE);
#endif

   gtk_set_locale();
   gtk_init(&argc, &argv);
   add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
   add_pixmap_directory (PACKAGE_SOURCE_DIR "/pixmaps");

   mainWindow = create_mainWindow();
   initTimes();

   tray_icon = egg_tray_icon_new("prayertimes");
   box = gtk_event_box_new();
   image = gtk_image_new();
   g_signal_connect(box, "button-press-event", 
                    G_CALLBACK(icon_clicked_cb), NULL);
   gtk_container_add(GTK_CONTAINER(box), image);
   gtk_container_add(GTK_CONTAINER(tray_icon), box);
   gtk_widget_show_all(GTK_WIDGET(tray_icon));
   g_object_ref(tray_icon);
   pixbuf = create_pixbuf(TRAYICON_IMAGE);
   gtk_image_set_from_pixbuf(GTK_IMAGE(image), pixbuf);
   g_object_unref(pixbuf);

   gtk_widget_show(mainWindow);

   gtk_main();
   return 0;
}
