#include <string.h>

#define USE_GCONF 1

#include <gtk/gtk.h>
#include <gconf/gconf-client.h>
#include <panel-applet.h>
#include <prayer.h>

#include "prefs.h"

void menu_item_set_label(GtkMenuItem *, const gchar *);
void next_prayer(void);
void update_date(void);
gboolean update_data(gpointer);
void update_remaining(void);
void prefs_cb(BonoboUIComponent *, gpointer, const gchar *);
void about_cb(BonoboUIComponent *, gpointer, const gchar *);
gboolean on_menubar_pressed(GtkWidget *, GdkEventButton *, gpointer);

GtkWidget *menu;
GtkWidget *prayer_times_label[6];

Prayer ptList[6];
gint next_prayer_id;
GDate *currentDate;
Date *prayerDate;
static gchar *next_prayer_string;

gchar *names[] = {
    "Subuh",
    "Fajar",
    "Dzuhur",
    "Ashar",
    "Maghrib",
    "Isya"
};

void
menu_item_set_label(GtkMenuItem *menu_item, const gchar* label)
{
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(menu_item));
    if (!child)
    {
        child = gtk_label_new(label);
        gtk_container_add(GTK_CONTAINER(menu_item), child);
    }

    gtk_label_set_label(GTK_LABEL(child), label);
}

// Taken from minbar
void 
next_prayer(void)
{   
    /* current time */
    time_t result;
    struct tm * curtime;
    result      = time(NULL);
    curtime     = localtime(&result);

    int i;
    for (i = 0; i < 6; i++)
    {
        if ( i == 1 ) { continue ;} /* skip shorouk */
        next_prayer_id = i;
        if(ptList[i].hour > curtime->tm_hour || 
            (ptList[i].hour == curtime->tm_hour && 
            ptList[i].minute >= curtime->tm_min))
        {
            return;
        }
    }

    next_prayer_id = 0; 
}

// Taken from minbar
void 
update_date(void)
{
    GTimeVal * curtime  = g_malloc(sizeof(GTimeVal));

    currentDate         = g_date_new();
    g_get_current_time(curtime);
    g_date_set_time_val(currentDate, curtime);
    g_free(curtime);

    /* Setting current day */
    prayerDate      = g_malloc(sizeof(Date));
    prayerDate->day     = g_date_get_day(currentDate);
    prayerDate->month   = g_date_get_month(currentDate);
    prayerDate->year    = g_date_get_year(currentDate);
    //update_date_label();
    g_free(currentDate);
}

// Taken and modified from minbar
void 
update_remaining(void)
{
    /* converts times to minutes */
    int next_minutes = ptList[next_prayer_id].minute + ptList[next_prayer_id].hour*60;
    time_t  result;
    struct  tm * curtime;

    result  = time(NULL);
    curtime = localtime(&result);
    int cur_minutes = curtime->tm_min + curtime->tm_hour * 60; 
    if(ptList[next_prayer_id].hour < curtime->tm_hour)
    {
        /* salat is on next day (subh, and even Isha sometimes) after midnight */
        next_minutes += 60*24;
    }

    int difference = next_minutes - cur_minutes;
    int hours = difference / 60;
    int minutes = difference % 60;

    if (difference == 0)
    {
        g_snprintf(next_prayer_string, 400,
            _("Time for %s"), 
            names[next_prayer_id]);
    }
    else
    {
        g_snprintf(next_prayer_string, 400, "%s -%d:%02d", names[next_prayer_id], hours, minutes);
    }
}

gboolean
update_data(gpointer data)
{
    next_prayer();
    update_remaining();
    menu_item_set_label(GTK_MENU_ITEM(menu), next_prayer_string);

    return TRUE;
}

void
prefs_cb(BonoboUIComponent *ui_container,
             gpointer           data,
             const gchar       *cname)
{
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(NULL, 
        GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "This should call Minbar's preferences dialog.");

    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

}

// Taken and modified from fast-user-switch-applet
void
about_cb(BonoboUIComponent *ui_container,
             gpointer           data,
             const gchar       *cname)
{
  static const gchar *authors[] = {
    "Fajran Iman Rusadi <fajran@ubuntu.com>",
    NULL
  };
  static gchar *license[] = {
    N_("The Minbar Applet is free software; you can redistribute it and/or modify "
       "it under the terms of the GNU General Public License as published by "
       "the Free Software Foundation; either version 2 of the License, or "
       "(at your option) any later version."),
    N_("This program is distributed in the hope that it will be useful, "
       "but WITHOUT ANY WARRANTY; without even the implied warranty of "
       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
       "GNU General Public License for more details."),
    N_("You should have received a copy of the GNU General Public License "
       "along with this program; if not, write to the Free Software "
        "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA "),
    NULL
  };
  
  gchar *license_i18n;

  license_i18n = g_strjoinv ("\n\n", license);

  gtk_show_about_dialog(NULL,
      "program-name", "Minbar Applet",
      "version", VERSION,
      "copyright", "Copyright \xc2\xa9 2008 Fajran Iman Rusadi.",
      "comments", _("GNOME Panel Applet for Minbar"),
      "authors", authors,
      "license", license_i18n,
      "wrap-license", TRUE,
      "translator-credits", _("translator-credits"),
      "logo-icon-name", "stock_people",
      "website", "http://github.com/fajran/minbar-applet/",
      "website-label", "Minbar Applet Website",
      NULL
   );

  g_free (license_i18n);
}

gboolean 
on_menubar_pressed(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    if (event->button != 1)
    {
        g_signal_stop_emission_by_name(widget, "button-press-event");
    }

    return FALSE;
}

static gboolean
minbar_applet_fill (PanelApplet *applet,
   const gchar *iid,
   gpointer data)
{

    if (strcmp (iid, "OAFIID:MinbarApplet") != 0)
        return FALSE;


    /*
     * menu bar
     */

    GtkWidget *menubar;
    menubar = gtk_menu_bar_new();

    menu = gtk_menu_item_new();

    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menu);

    gtk_container_add(GTK_CONTAINER(applet), menubar);

    g_signal_connect(G_OBJECT(menubar), "button-press-event", G_CALLBACK(on_menubar_pressed), NULL);

    /*
     * sub menus
     */

    GtkWidget *submenu;

    GtkWidget *hbox, *label_name;
    submenu = gtk_menu_new();


    GtkWidget *menu_prayer_time;
    int i;
    for (i=0; i<6; i++) {
        menu_prayer_time = gtk_menu_item_new();

        hbox = gtk_hbox_new(FALSE, 50);
        label_name = gtk_label_new(names[i]);
        prayer_times_label[i] = gtk_label_new("00:00");

        gtk_box_pack_start(GTK_BOX(hbox), label_name, FALSE, FALSE, 0);
        gtk_box_pack_end(GTK_BOX(hbox), prayer_times_label[i], FALSE, FALSE, 0);

        gtk_container_add(GTK_CONTAINER(menu_prayer_time), hbox);

        gtk_menu_append(GTK_MENU(submenu), menu_prayer_time);
    }

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu), submenu);

    /*
     * contextual menu
     */

    static const BonoboUIVerb menu_verbs[] = {
        BONOBO_UI_VERB ("MinbarPreferences", prefs_cb),
        BONOBO_UI_VERB ("MinbarAbout", about_cb),
        BONOBO_UI_VERB_END
    };
  
    panel_applet_setup_menu_from_file(applet, DATADIR, "minbar-applet.xml", NULL, menu_verbs, NULL);

    /*
     * gconf thingies
     */

    GConfClient *client;
    client = gconf_client_get_default();

    gfloat city_lat, city_lon, city_height, city_correction;
    gint method;
    gchar *city_name;

    city_lat = gconf_client_get_float(client, PREF_CITY_LAT, NULL);
    city_lon = gconf_client_get_float(client, PREF_CITY_LON, NULL);
    city_height = gconf_client_get_float(client, PREF_CITY_HEIGHT, NULL);
    city_correction = gconf_client_get_float(client, PREF_CITY_CORRECTION, NULL);
    city_name = gconf_client_get_string(client, PREF_CITY_NAME, NULL);
    method  = gconf_client_get_int(client, PREF_PREF_METHOD, NULL);

    /*
     * Locations
     */

    Method *calcMethod = g_malloc(sizeof(Method));
    getMethod(method, calcMethod);

    Location *loc = g_malloc(sizeof(Location));

    loc->degreeLong = city_lon;
    loc->degreeLat = city_lat;
    loc->gmtDiff = city_correction;
    loc->dst = 0;
    loc->seaLevel = 0;
    loc->pressure = 1010;
    loc->temperature = 10;


    update_date();

    /*
     * Prayer time
     */

    getPrayerTimes(loc, calcMethod, prayerDate, ptList);

    gchar *prayer_time_text[6];
    for (i=0; i<6; i++) {
        prayer_time_text[i] = g_malloc(sizeof(gchar) * 6);
        g_snprintf(prayer_time_text[i], 6, "%d:%02d", ptList[i].hour, ptList[i].minute); 

        gtk_label_set_text(GTK_LABEL(prayer_times_label[i]), prayer_time_text[i]);
    }

    /*
     * Next prayer
     */

    next_prayer_string = g_malloc(sizeof(gchar) * 400);

    update_data(NULL);


    /*
     * wrapping up
     */

    gtk_widget_show_all (GTK_WIDGET (applet));

    g_timeout_add(60000, update_data, NULL);
    return TRUE;
}

PANEL_APPLET_BONOBO_FACTORY ("OAFIID:MinbarApplet_Factory",
                             PANEL_TYPE_APPLET,
                             "The Hello World Applet",
                             "0",
                             minbar_applet_fill,
                             NULL);
