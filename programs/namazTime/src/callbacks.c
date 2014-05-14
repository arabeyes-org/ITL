#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

extern int dst;
extern int method;
extern int remind;
extern double gmtDiff;
extern double seaLevel;
extern double latitude;
extern double longitude;
extern char* locationName;
extern GtkWidget* mainWindow, *prefsWindow;

void toggle_window(GtkWidget* widget, GdkEventButton* event){
   static short status = 1;
   
   GtkWidget* window = mainWindow;
   if (status) gtk_widget_hide_all(window);
   else gtk_widget_show_all(window);
   status = status ^ 1;
}

gboolean on_mainWindow_delete_event(GtkWidget *widget, GdkEvent *event,
                                    gpointer user_data){
   toggle_window(widget, NULL);
   return TRUE;
}

void on_cancelbutton1_clicked(GtkButton *btn, gpointer user_data){
   gtk_widget_hide(prefsWindow);
   gtk_widget_destroy(prefsWindow);
}


void on_okbutton1_clicked(GtkButton *btn, gpointer user_data){
   char* tmp;
   GtkWidget* entry, *widget, *button;
 
   widget = prefsWindow;
   gtk_widget_hide(widget);

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

   gtk_widget_destroy(widget);
}
