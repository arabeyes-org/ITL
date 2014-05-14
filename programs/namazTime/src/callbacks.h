#include <gtk/gtk.h>

void toggle_window(GtkWidget* widget, GdkEventButton* event);

gboolean on_mainWindow_delete_event(GtkWidget* widget, GdkEvent* event,
                                    gpointer user_data);
void on_cancelbutton1_clicked(GtkButton* button, gpointer user_data);
void on_okbutton1_clicked(GtkButton* button, gpointer user_data);

