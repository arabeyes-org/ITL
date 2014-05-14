/* main.c - main program
 *
 * Copyright (C) 2006-2007 
 * 		Djihed Afifi <djihed@gmail.com>,
 * 		Abderrahim Kitouni <a.kitouni@gmail.com> 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <gtk/gtk.h>
#include <prayer.h>
#include <hijri.h>
#include <glib/gi18n.h>
#include <locale.h>

#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "defines.h"
#include "locations-xml.h" 
#include "config.h"

#define USE_TRAY_ICON   1
#define USE_NOTIFY	(USE_TRAY_ICON & HAVE_NOTIFY)

#if USE_GSTREAMER
#include <gst/gst.h>
#else
#include <xine.h>
#endif

#if USE_NOTIFY
#include <libnotify/notify.h>
#endif

#if USE_RSVG
#include <librsvg/rsvg.h>
#endif

#include "main.h"

/* Preferences */ 
static const gchar * 	program_name ;
static int 		next_prayer_id = -1;
static gboolean 	* start_hidden_arg = FALSE;

static MinbarConfig* config;

/* for prayer.h functions */
static Date 		* prayerDate;
static Location		* loc;
static Method		* calcMethod;
static Prayer 		ptList[6];

/* For libraries */
static GtkBuilder	* builder;
#if USE_GSTREAMER
/* For gstreamer */
static GstElement	*pipeline;
static GMainLoop	*loop;
static GstBus		*bus;
#else
static xine_t		*xine;
static xine_audio_port_t*audio_port;
static xine_stream_t	*stream = NULL;
#endif

static GtkFileFilter 	*filter_all;
static GtkFileFilter 	*filter_supported;

/* tray icon */
#if USE_TRAY_ICON
static GtkStatusIcon   	* status_icon;	
#endif
static GDate		* currentDate;

sDate 			* hijri_date;
static gchar 		* next_prayer_string;
static int 		calling_athan_for;
/* init moved for i18n */
gchar * hijri_month[13];
gchar * time_names[6];

#if USE_NOTIFY
NotifyNotification 	* notification;
#endif

/* qibla */
GtkWidget * qibla_drawing_area;
#if USE_RSVG
RsvgHandle * compass;
#endif

#if USE_TRAY_ICON
inline void set_status_tooltip()
{
	gchar * tooltiptext;
	tooltiptext = g_malloc(2000);
	g_snprintf(tooltiptext, 2000, "    %s \t\n\n"
					" %s:  %02d:%02d \n"
				 	" %s:  %02d:%02d \n"
					" %s:  %02d:%02d \n"
					" %s:  %02d:%02d \n"
					" %s:  %02d:%02d \n"
					" %s:  %02d:%02d"
					"\n\n"
					" %s   "
					,
					program_name,
			time_names[0], ptList[0].hour, ptList[0].minute,
			time_names[1], ptList[1].hour, ptList[1].minute,
			time_names[2], ptList[2].hour, ptList[2].minute,
			time_names[3], ptList[3].hour, ptList[3].minute,
			time_names[4], ptList[4].hour, ptList[4].minute,
			time_names[5], ptList[5].hour, ptList[5].minute, 
			next_prayer_string
		  );
	gtk_status_icon_set_tooltip_text(status_icon, tooltiptext);
	g_free(tooltiptext);

}
#endif

void update_remaining()
{
	/* converts times to minutes */
	int next_minutes = ptList[next_prayer_id].minute + ptList[next_prayer_id].hour*60;
	time_t 	result;
	struct 	tm * curtime;

	result 	= time(NULL);
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

	gchar * trbuf; /* Formatted for display within applet */
			/* leaving next_prayer_string unformatted for tooltip */
	trbuf = g_malloc(600);

	if (difference == 0)
	{
		g_snprintf(next_prayer_string, 400,
			_("Time for prayer: %s"), 
			time_names[next_prayer_id]);
	}
	else if (difference < 60 )
	{
		g_snprintf(next_prayer_string, 400,
				_("%d %s until %s prayer."),
				minutes,
				ngettext("minute", "minutes", minutes),
				time_names[next_prayer_id]);
	}
	else if (difference % 60 == 0)
	{
		g_snprintf(next_prayer_string, 400,
				_("%d %s until %s prayer."),
				hours,
				ngettext("hour", "hours", hours),
				time_names[next_prayer_id]);
	}
	else
	{
		g_snprintf(next_prayer_string, 400,
				_("%d %s and %d %s until %s prayer."),
				hours,
				ngettext("hour", "hours", hours),
				minutes,
				ngettext("minute", "minutes", minutes),
				time_names[next_prayer_id]);
	}
	g_snprintf(trbuf, 600,
			("%s%s%s"),
			REMAIN_MARKUP_START,
			next_prayer_string,
			REMAIN_MARKUP_END);

	gtk_label_set_markup((GtkLabel *) gtk_builder_get_object(builder, 
				"timeleftlabel"), trbuf);
	g_free(trbuf);
}

void update_date_label()
{
	gchar  *miladi, * dateString, * weekday, * time_s;
	dateString 	= g_malloc(500);
	miladi 		= g_malloc(200);
	weekday		= g_malloc(50);
	time_s		= g_malloc(50);

	g_date_strftime(weekday, 50, "%A", currentDate);

	time_t 	result;
	struct 	tm * curtime;
	result 	= time(NULL);
	curtime = localtime(&result);


	/* TRANSLATOR: this is a format string for strftime
	 *             see `man 3 strftime` for more details
	 *             copy it if you're unsure
	 *             This will print an example: 12 January 2007
	 */
	if (!g_date_strftime (miladi, 200, _("%d %B %G"), currentDate))
	{
		g_date_strftime (miladi, 200, "%d %B %G", currentDate);
	}

	/* TRANSLATOR: this is a format string for strftime
	 *             see `man 3 strftime` for more details
	 *             copy it if you're unsure
	 *             This will print an example: 19:17.
	 *             if you want to use 12 hour format, use: %I:%M %p
	 *             which will print something similar to: 7:17 pm  
	 */
	if (!strftime (time_s, 50, _("%H:%M"), curtime))
	{
		strftime (time_s, 50, "%H:%M", curtime);
	}
	
	hijri_date 	= g_malloc(sizeof(sDate));
	h_date(hijri_date, prayerDate->day, prayerDate->month, prayerDate->year);
	g_snprintf(dateString, 500, "%s %s%s %d %s %d \n %s \n%s%s", DATE_MARKUP_START,
	weekday, /* The comma may differ from language to language*/ _(","),
	hijri_date->day, hijri_month[hijri_date->month], 
	hijri_date->year, 
	miladi, 
	time_s,
	DATE_MARKUP_END);

	gtk_label_set_markup((GtkLabel *)gtk_builder_get_object(builder, 
				"currentdatelabel"), dateString);
	g_free(dateString);
	g_free(hijri_date);
	g_free(miladi);
	g_free(weekday);
}

void calculate_prayer_table()
{
	/* Update the values */
	loc->degreeLat 		= config->latitude;
	loc->degreeLong 	= config->longitude;
	loc->gmtDiff		= config->correction;	
	getPrayerTimes (loc, calcMethod, prayerDate, ptList);	
	next_prayer();
	update_remaining();
}

void play_events()
{
	time_t 	result;
	struct 	tm * curtime;
	result 	= time(NULL);
	curtime = localtime(&result);

	int cur_minutes = curtime->tm_hour * 60 + curtime->tm_min;

	int i;
	for (i = 0; i < 6; i++)
	{
		if ( i == 1 ) { continue ;} /* skip shorouk */
		/* covert to minutes */
		int pt_minutes = ptList[i].hour*60 + ptList[i].minute;
#if USE_NOTIFY		
		if ((cur_minutes + config->notification_time == pt_minutes ) && config->notification)
		{
			gchar * message;
			message = g_malloc(400);
			g_snprintf(message, 400, _("%d minutes until %s prayer."), 
					config->notification_time, time_names[i]); 
			show_notification(message);
			g_free(message);
		}
#endif
		if (cur_minutes == pt_minutes)
		{
			calling_athan_for = i;
			if(config->athan_enabled){play_athan_callback();}
#if USE_NOTIFY
			if(config->notification)
			{
				gchar * message;
				message = g_malloc(400);
				g_snprintf(message, 400, _("It is time for %s prayer."), time_names[i]); 

				show_notification(message);
				g_free(message);
			}
#endif		
		}
	}
}

void next_prayer()
{	
	/* current time */
	time_t result;
	struct tm * curtime;
	result 		= time(NULL);
	curtime 	= localtime(&result);

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

void update_date()
{
	GTimeVal * curtime 	= g_malloc(sizeof(GTimeVal));

	currentDate 		= g_date_new();
	g_get_current_time(curtime);
	g_date_set_time_val(currentDate, curtime);
	g_free(curtime);

	/* Setting current day */
	prayerDate 		= g_malloc(sizeof(Date));
	prayerDate->day 	= g_date_get_day(currentDate);
	prayerDate->month 	= g_date_get_month(currentDate);
	prayerDate->year 	= g_date_get_year(currentDate);
	update_date_label();
	g_free(currentDate);
}

void update_calendar()
{
	gtk_calendar_select_month((GtkCalendar *) gtk_builder_get_object(builder, "prayer_calendar"),
			prayerDate->month - 1, prayerDate->year);
	gtk_calendar_select_day((GtkCalendar *) gtk_builder_get_object(builder, "prayer_calendar"),
			prayerDate->day);
}

void prayer_calendar_callback()
{
	guint * year = g_malloc(sizeof(guint));
	guint * month = g_malloc(sizeof(guint));
	guint * day = g_malloc(sizeof(guint));

	gtk_calendar_get_date((GtkCalendar *) gtk_builder_get_object(builder, "prayer_calendar"),
			year, month, day);

	Prayer calendarPtList[6];
	Date * cDate;	
	cDate 		= g_malloc(sizeof(Date));
	cDate->day 	= (int) *day;
	cDate->month 	= (int) (*month) +1;
	cDate->year 	= (int) *year;

	getPrayerTimes (loc, calcMethod, cDate, calendarPtList);
	g_free(cDate);
	update_prayer_labels(calendarPtList, "salatlabelc", FALSE);
	g_free(year);
	g_free(month);
	g_free(day);
}

/* This is cool: this will change the label next to the notification 
 * spin button the preferences window according to the value of 
 * the spin button
 * */
void minute_label_callback(GtkWidget *widget, gpointer user_data)
{
	gtk_label_set_text((GtkLabel *) 
		gtk_builder_get_object(builder, "minutes_label"),
		ngettext("minute", "minutes", 
			gtk_spin_button_get_value((GtkSpinButton *)
				widget))
		);
}

void update_prayer_labels(Prayer * ptList, gchar * prefix, gboolean coloured)
{
	/* getting labels and putting time strings */
	gchar * timestring;
	gchar * timelabel;
	timestring 	= g_malloc(50);
	timelabel	= g_malloc(20);

	int i;
	for (i=0; i < 6; i++)
	{
		g_snprintf(timelabel, 20, "%s%d", prefix, i);
		if( i == 1 && coloured)
		{
			g_snprintf(timestring, 50, "%s%02d:%02d%s", 
				MARKUP_FAINT_START, ptList[i].hour, 
				ptList[i].minute, MARKUP_FAINT_END);
		}
		else if ( i == next_prayer_id && coloured)
		{
			g_snprintf(timestring, 50, "%s%02d:%02d%s", 
				MARKUP_SPECIAL_START, ptList[i].hour, 
				ptList[i].minute, MARKUP_SPECIAL_END);
		}
		else
		{
			g_snprintf(timestring, 50, "%s%02d:%02d%s", 
				MARKUP_NORMAL_START, ptList[i].hour, 
				ptList[i].minute, MARKUP_NORMAL_END);
		}
	
		gtk_label_set_markup((GtkLabel *) gtk_builder_get_object(builder, timelabel),
				timestring);
	}
	
	g_free(timestring);
	g_free(timelabel);
}

/* needed post loading preferences.*/
void init_vars()
{
	/* Allocate memory for variables */
	loc 			= g_malloc(sizeof(Location));
	next_prayer_string = g_malloc(400);
		
	
	update_date();

	/* Location variables */
	loc->degreeLat 		= config->latitude;
	loc->degreeLong 	= config->longitude;
	loc->gmtDiff 		= config->correction;
	loc->dst		= 0;
	loc->seaLevel 		= 0;
	loc->pressure 		= 1010;
	loc->temperature	= 10;
}

void on_enabledathanmenucheck_toggled_callback(GtkWidget *widget,
				gpointer user_data)
{
	config->athan_enabled = gtk_check_menu_item_get_active((GtkCheckMenuItem * ) widget);

	gtk_toggle_button_set_active((GtkToggleButton * )
			gtk_builder_get_object( builder, "enabledathancheck"),
			config->athan_enabled);
	gtk_check_menu_item_set_active((GtkCheckMenuItem * )
			gtk_builder_get_object( builder, "playathan"),
			config->athan_enabled);

	config_save(config);
}

void on_enabledathancheck_toggled_callback(GtkWidget *widget,
				gpointer user_data)
{
	config->athan_enabled = gtk_toggle_button_get_active((GtkToggleButton * ) widget);
	
	gtk_toggle_button_set_active((GtkToggleButton * )
			gtk_builder_get_object( builder, "enabledathancheck"),
			config->athan_enabled);
	gtk_check_menu_item_set_active((GtkCheckMenuItem * )
			gtk_builder_get_object( builder, "playathan"),
			config->athan_enabled);

	config_save(config);
}


void on_notifmenucheck_toggled_callback(GtkWidget *widget, 
		gpointer user_data)
{
	config->notification = gtk_check_menu_item_get_active((GtkCheckMenuItem * ) widget);

	gtk_toggle_button_set_active((GtkToggleButton * )
			gtk_builder_get_object( builder, "yesnotif"),
			
			config->notification);
	gtk_check_menu_item_set_active((GtkCheckMenuItem * )
			gtk_builder_get_object( builder, "notifmenucheck"),
			config->notification);

	config_save(config);
}

void on_editcityokbutton_clicked_callback(GtkWidget *widget,
	       				gpointer user_data) 
{

	GtkWidget*  entrywidget;	
	/* Setting what was found to editcity dialog*/
	entrywidget 	= (GtkWidget *) gtk_builder_get_object( builder, "longitude");	
	config->longitude 		=  gtk_spin_button_get_value((GtkSpinButton *)entrywidget);

	entrywidget 	= (GtkWidget *) gtk_builder_get_object( builder, "latitude");
	config->latitude 		=  gtk_spin_button_get_value((GtkSpinButton *)entrywidget);

	entrywidget 	= (GtkWidget *) gtk_builder_get_object( builder, "cityname");
	g_stpcpy(config->city, gtk_entry_get_text((GtkEntry *)entrywidget)); 

	entrywidget 	= (GtkWidget *) gtk_builder_get_object( builder, "correction");
	config->correction 	=  (double)gtk_spin_button_get_value((GtkSpinButton *)entrywidget);

	entrywidget 	= (GtkWidget *) gtk_builder_get_object( builder, "yesnotif");
	config->notification 		=  gtk_toggle_button_get_active((GtkToggleButton *)entrywidget);

	entrywidget 	= (GtkWidget *) gtk_builder_get_object( builder, "notiftime");
	config->notification_time 	=  (int)gtk_spin_button_get_value((GtkSpinButton *)entrywidget);

	entrywidget 	= (GtkWidget *) gtk_builder_get_object( builder, "startHidden");
	config->start_hidden 	=  gtk_toggle_button_get_active((GtkToggleButton *)entrywidget);

	entrywidget 	= (GtkWidget *) gtk_builder_get_object( builder, "methodcombo");
	config->method 		=  (int)gtk_combo_box_get_active((GtkComboBox *)entrywidget)  + 1;

	if(config->method < 0 || config->method > 6 ) { config->method = 5; }
	getMethod(config->method, calcMethod);

	config->athan_subh = gtk_file_chooser_get_filename ((GtkFileChooser *) (gtk_builder_get_object(builder, "athan_subh_chooser")));
	config->athan_normal = gtk_file_chooser_get_filename ((GtkFileChooser *) (gtk_builder_get_object(builder, "athan_chooser")));

	config_save(config);

	/* Now hide the cityedit dialog */
	gtk_widget_hide((GtkWidget *) gtk_builder_get_object(builder, "editcity"));

	/* And set the city string in the main window */
	gtk_label_set_text((GtkLabel *)
			(gtk_builder_get_object(builder, "locationname"))
			,(const gchar *)config->city);

	/* Now calculate new timetable */
	calculate_prayer_table();
	/* And set the new labels */
	update_prayer_labels(ptList, "salatlabel", TRUE);
	gtk_widget_queue_draw(qibla_drawing_area);
	prayer_calendar_callback();
}


void init_prefs ()
{
	if( config->method < 0 || config->method > 6)
	{
		g_printerr(_("Invalid calculation method in preferences, using 5: Muslim world League \n"));
	}

	calcMethod 		= g_malloc(sizeof(Method));
	getMethod(config->method, calcMethod);


	GtkWidget*  entrywidget;	
	
	/* Setting what was found to editcity dialog*/
	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "latitude");	
	gtk_spin_button_set_value((GtkSpinButton *)entrywidget, config->latitude);

	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "longitude");	
	gtk_spin_button_set_value((GtkSpinButton *)entrywidget, config->longitude);

	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "cityname");	
	gtk_entry_set_text((GtkEntry *)entrywidget, config->city);

	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "correction");	
	gtk_spin_button_set_value((GtkSpinButton *)entrywidget, config->correction);

	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "yesnotif");	
	gtk_toggle_button_set_active((GtkToggleButton *)entrywidget, config->notification);
	
	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "notiftime");	
	gtk_spin_button_set_value((GtkSpinButton *)entrywidget, config->notification_time);

	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "methodcombo");	
	gtk_combo_box_set_active((GtkComboBox *)entrywidget, config->method-1);

	/* Set the play athan check box */
	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "enabledathancheck");
	gtk_toggle_button_set_active((GtkToggleButton *) entrywidget, config->athan_enabled);
	gtk_check_menu_item_set_active((GtkCheckMenuItem * )
			gtk_builder_get_object( builder, "playathan"),
			config->athan_enabled);

	/* notitication menu item */
	gtk_check_menu_item_set_active((GtkCheckMenuItem * )
			gtk_builder_get_object( builder, "notifmenucheck"),
			config->notification);

	/* Start minimised checkbox */
	gtk_toggle_button_set_active((GtkToggleButton * )
			gtk_builder_get_object( builder, "startHidden"),
			config->start_hidden);

	/* And set the city string in the main window */
	gtk_label_set_text((GtkLabel *)
			(gtk_builder_get_object(builder, "locationname")),
		       	(const gchar *)config->city);

	/* show on start up? */
	GtkWidget * mainwindow = (GtkWidget *) gtk_builder_get_object(builder, "mainWindow");
	
#if USE_TRAY_ICON
	if(!config->start_hidden && !start_hidden_arg)
	{
		gtk_widget_show(mainwindow);
	}
	else
	{
		gtk_widget_hide(mainwindow);
	}
#else
	gtk_widget_show(mainwindow);
#endif
	/* set UI vars */
	/* Check existence of file */
	FILE * testfile;
	testfile = fopen( config->athan_subh, "r");
	if(testfile != NULL)
	{
	fclose(testfile);
	gtk_file_chooser_set_filename  ((GtkFileChooser *) 
			(gtk_builder_get_object(builder, "athan_subh_chooser")),
			(const gchar *) config->athan_subh);
	}
	else
	{
		calling_athan_for = 0;
		set_file_status(FALSE);
	}
	setup_file_filters();
	gtk_file_chooser_add_filter ((GtkFileChooser *) 
			(gtk_builder_get_object(builder, "athan_subh_chooser")),
	       		filter_supported);

	gtk_file_chooser_add_filter ((GtkFileChooser *) 
			(gtk_builder_get_object(builder, "athan_subh_chooser")),
	       		filter_all);

	testfile = fopen( config->athan_normal, "r");
	if(testfile != NULL)
	{

	fclose(testfile);
	gtk_file_chooser_set_filename  ((GtkFileChooser *) 
			(gtk_builder_get_object(builder, "athan_chooser")),
			(const gchar *) config->athan_normal);
	}
	else
	{
		calling_athan_for = 1;
		set_file_status(FALSE);
	}
	setup_file_filters();
	gtk_file_chooser_add_filter ((GtkFileChooser *) 
			(gtk_builder_get_object(builder, "athan_chooser")),
	       		filter_supported);

	gtk_file_chooser_add_filter ((GtkFileChooser *) 
			(gtk_builder_get_object(builder, "athan_chooser")),
	       		filter_all);


}

gboolean no_stream_errors;
void play_athan_callback()
{

	/* Stop previously played file */
	stop_athan_callback();
#if USE_GSTREAMER
	int returned = init_pipelines();
	if(returned < 0)
	{
		exit(-1);
	}
#else
	stream = xine_stream_new(xine, audio_port, NULL);
#endif
	gchar * athanuri; 
	/* set filename property on the file source. Also add a message
	 * handler. */

	no_stream_errors = TRUE;
	if(calling_athan_for == 0)
	{
		athanuri  = gtk_file_chooser_get_uri  
		((GtkFileChooser *) (gtk_builder_get_object(builder, "athan_subh_chooser")));
	}
	else
	{
		athanuri  = gtk_file_chooser_get_uri  
		((GtkFileChooser *) (gtk_builder_get_object(builder, "athan_chooser")));
	}
#if USE_GSTREAMER
	g_object_set (G_OBJECT (pipeline), "uri", athanuri, NULL);

	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);

	/* Now set to playing and iterate. */
	gst_element_set_state (pipeline, GST_STATE_PLAYING);
	g_main_loop_run (loop);

  	/* clean up nicely */
  	gst_element_set_state (pipeline, GST_STATE_NULL);
  	gst_object_unref (GST_OBJECT (pipeline));
#else
	xine_open(stream, athanuri);
	xine_play(stream, 0, 0);
#endif

}

void play_subh_athan_callback ()
{
	calling_athan_for = 0;
	play_athan_callback();
}

void play_normal_athan_callback ()
{
	calling_athan_for = 2;
	play_athan_callback();
}

void stop_athan_callback()
{
	/* clean up nicely */
#if USE_GSTREAMER
	if(GST_IS_ELEMENT (pipeline))
	{		
		gst_element_set_state (pipeline, GST_STATE_NULL);
		gst_object_unref (GST_OBJECT (pipeline));
	}
#else
	if (stream)
	{
	  xine_dispose(stream);
	  stream = NULL;
	}
#endif
}

#if USE_GSTREAMER

gboolean bus_call (GstBus     *bus,
	  GstMessage *msg,
	  gpointer    data)
{
	switch (GST_MESSAGE_TYPE (msg)) {
		case GST_MESSAGE_EOS:
			/* End of Stream */
			g_main_loop_quit (loop);
			break;
		case GST_MESSAGE_ERROR: {
			gchar *debug;
			GError *err;

			gst_message_parse_error (msg, &err, &debug);
			g_free (debug);

			g_print (_("Error: %s\n"), err->message);
			g_error_free (err);
			
			g_main_loop_quit (loop);
			no_stream_errors= FALSE;
			break;
		}
		default:
			break;
		}
	
	set_file_status(no_stream_errors);
	return TRUE;
}
#endif
void set_file_status(gboolean status)
{
	gchar * label_name = g_malloc(100);
	gchar * label_status = g_malloc(100);
	
	if(calling_athan_for == 0)
		strcpy(label_name, "athansubhstatusimage");
	else
		strcpy(label_name, "athanstatusimage");

	if(status)
		strcpy(label_status, GTK_STOCK_APPLY);
	else
		strcpy(label_status, GTK_STOCK_DIALOG_WARNING);

	gtk_image_set_from_stock((GtkImage *) (gtk_builder_get_object(builder, label_name)),
			label_status, GTK_ICON_SIZE_BUTTON); 

	g_free(label_name);
	g_free(label_status);
}

#if USE_GSTREAMER
int init_pipelines()
{
	/* create elements */
	pipeline	= gst_element_factory_make ("playbin", "play");

	if (!pipeline) {
		g_print ("pipeline could not be created\n");
                /* FIXME: returning -1 crashes.  */
		return -1;
	}
	return 1;
}
#endif

void setup_file_filters (void)
{
	filter_all = gtk_file_filter_new ();
	gtk_file_filter_set_name (filter_all, _("All files"));
	gtk_file_filter_add_pattern (filter_all, "*");
	g_object_ref (filter_all);

	filter_supported = gtk_file_filter_new ();
	gtk_file_filter_set_name (filter_supported,
		_("Supported files"));
#if USE_GSTREAMER
	gtk_file_filter_add_mime_type (filter_supported, "application/ogg");
	gtk_file_filter_add_mime_type (filter_supported, "audio/*");
#else
	char* xine_supported = xine_get_mime_types(xine);
	char* result = strtok(xine_supported, ":");
	while (result != NULL)
	{
	  gtk_file_filter_add_mime_type (filter_supported, result);
	  strtok(NULL, ";");
	  result = strtok(NULL, ":");
	}
#endif
	g_object_ref (filter_supported);
}

/* Interval to update prayer times if time/date changes */
gboolean update_interval(gpointer data)
{
	update_date(); 
	calculate_prayer_table(); 
	update_prayer_labels(ptList, "salatlabel", TRUE);
	
	play_events();
#if USE_TRAY_ICON
	set_status_tooltip();
#endif
	
	return TRUE;
}

/* System tray icon */
#if USE_TRAY_ICON
void load_system_tray()
{
	status_icon 	= gtk_status_icon_new_from_icon_name
		("minbar");
	
	g_signal_connect ((GtkStatusIcon * ) (status_icon), "popup_menu", 
			G_CALLBACK(tray_icon_right_clicked_callback) , NULL);
	g_signal_connect ((GtkStatusIcon * ) (status_icon), "activate", 
			G_CALLBACK(tray_icon_clicked_callback) , NULL);	
}
#endif


void check_quit_callback(GtkWidget *widget, gpointer data)
{
	if(config->close_closes)
		gtk_main_quit();
	else
		gtk_widget_hide((GtkWidget *) gtk_builder_get_object(builder, "mainWindow"));
}

void quit_callback ( GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}

void tray_icon_right_clicked_callback (GtkWidget *widget, gpointer data)
{
	GtkMenu * popup_menu = (GtkMenu * )(gtk_builder_get_object(builder, "traypopup")); 
	
	gtk_menu_set_screen (GTK_MENU (popup_menu), NULL);
	
	gtk_menu_popup (GTK_MENU (popup_menu), NULL, NULL, NULL, NULL,
			2, gtk_get_current_event_time());
}

void show_window_clicked_callback (GtkWidget *widget, gpointer data)
{
	if(gtk_widget_get_visible((GtkWidget *) gtk_builder_get_object(builder, "mainWindow")))
	{
		gtk_widget_hide((GtkWidget *) gtk_builder_get_object(builder, "mainWindow"));
	}
	else
	{
		gtk_window_present((GtkWindow *)gtk_builder_get_object(builder, "mainWindow"));
	}
}

void tray_icon_clicked_callback ( GtkWidget *widget, gpointer data)
{
	if(gtk_window_is_active((GtkWindow *)gtk_builder_get_object(builder, "mainWindow")))
	{
		gtk_widget_hide((GtkWidget *) gtk_builder_get_object(builder, "mainWindow"));
	}
	else
	{
		gtk_window_present((GtkWindow *)gtk_builder_get_object(builder, "mainWindow"));
	}
}

/* quit callback */
void close_callback( GtkWidget *widget,
	    gpointer data)
{
	gtk_widget_hide((GtkWidget *) gtk_builder_get_object(builder, "mainWindow"));

}

/**** Notification Balloons ****/
#if USE_NOTIFY
void show_notification(gchar * message)
{
	notify_notification_update(notification,
				program_name,
				message,
				"minbar");
	notify_notification_show(notification, NULL);
}

void create_notification()
{
	notification = notify_notification_new
                                            (program_name,
                                             NULL,
#if !defined(NOTIFY_VERSION_MINOR) || (NOTIFY_VERSION_MAJOR == 0 && NOTIFY_VERSION_MINOR < 7)
                                             NULL,
                                             NULL);
	notify_notification_attach_to_status_icon (notification, status_icon );
#else
                                             NULL);
#endif
	notify_notification_set_timeout (notification, 8000);
}
#endif

/**** Main ****/
int main(int argc, char *argv[]) 
{
	/* init gettext */

	setlocale (LC_ALL, "");
	bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	program_name = _("Minbar Prayer Times");

	hijri_month[0]	= _("skip");
	hijri_month[1]	= _("Muharram");
	hijri_month[2]	= _("Safar");
	hijri_month[3]	= _("Rabi I");
	hijri_month[4]	= _("Rabi II");
	hijri_month[5]	= _("Jumada I");
	hijri_month[6]	= _("Jumada II");
	hijri_month[7]	= _("Rajab");
	hijri_month[8]	= _("Shaaban");
	hijri_month[9]	= _("Ramadan");
	hijri_month[10]	= _("Shawwal");
	hijri_month[11]	= _("Thul-Qiaadah");
	hijri_month[12]	= _("Thul-Hijja");

	time_names[0]	= _("Subh");
	time_names[1]	= _("Shorook");
	time_names[2]	= _("Dhuhr");
	time_names[3]	= _("Asr");
	time_names[4]	= _("Maghreb");
	time_names[5]	= _("Isha'a");

	/* init libraries */
	gtk_init(&argc, &argv);

#if USE_GSTREAMER	
	/* initialize GStreamer */
	gst_init (&argc, &argv);
	loop = g_main_loop_new (NULL, FALSE);
#else
	xine = xine_new();
	xine_init(xine);
	audio_port = xine_open_audio_driver(xine , "auto", NULL);
#endif

	/* init config */
	config_init();
	config = config_read();

	/* command line options */
	GOptionEntry options[] = 
	{
		{"hide", 'h', 0, G_OPTION_ARG_NONE, &start_hidden_arg, 	
			_("Hide main window on start up."), NULL },
		{ NULL }
	};
	
	GOptionContext *context = NULL;
	context = g_option_context_new (NULL);
  	g_option_context_add_main_entries (context, options, PACKAGE_NAME);
  	g_option_context_set_help_enabled (context, TRUE);

	g_option_context_parse (context, &argc, &argv, NULL);
  	g_option_context_free (context);
	
#if USE_NOTIFY
	notify_init(program_name);
#endif

	/* load the interface */
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, g_build_filename(MINBAR_DATADIR, UI_MAIN_INTERFACE, NULL), NULL);
	/* connect the signals in the interface */
	gtk_builder_connect_signals(builder, NULL);
	
	/* Set up some widgets and options that are not stored in the gtkbuilder xml */
	setup_widgets();
	
	/* System tray icon */
#if USE_TRAY_ICON
	load_system_tray();
#endif
		
	/* Initialize preferences and variables */	
	init_prefs();
	init_vars();

	/* calculate the time table, and update the labels, other inits */
	calculate_prayer_table();
	update_prayer_labels(ptList, "salatlabel", TRUE);
	update_calendar();
	prayer_calendar_callback();
	gtk_widget_queue_draw(qibla_drawing_area);
#if USE_TRAY_ICON
	/* set system tray tooltip text */
	set_status_tooltip();
#if USE_NOTIFY
	/* Used to balloon tray notifications */
	create_notification();
#endif
#endif
	/* start athan playing, time updating interval */
	g_timeout_add(60000, update_interval, NULL);

	/* start the event loop */
  	gtk_main();
	return 0;
}


gboolean draw_qibla (GtkWidget *widget, cairo_t *context, gpointer data)
{
	GtkAllocation allocation;

	gtk_widget_get_allocation(widget, &allocation);
	double height =  (double) allocation.height;
	double width  =  (double) allocation.width;

	double actual;

	double qibla = getNorthQibla(loc);
	/* it was deg, convert to rad */
	double qiblarad = - (qibla * G_PI) / 180;

	gchar * qiblabuf;

	cairo_save(context); /* for transforms */

	/* if the place is Makkah itself, don't draw */
	if((int)(config->latitude * 10 ) == 214 && (int)(config->longitude * 10 ) == 397) /* be less restrictive */
	{
	  qiblabuf = g_malloc(100);
	  g_snprintf(qiblabuf, 100,
		     "%s",
		     _("In Makkah!")
		     );
	}
	else
	{
#if USE_RSVG
		actual = height*580/680 < width ? height*580/680 : width;

		/* center the compass */
		cairo_translate(context, (width-actual)/2, (height-(actual*680/580))/2);
		cairo_scale(context, actual/580, actual/580);
		rsvg_handle_render_cairo_sub(compass, context, "#compass");

		/* the ibra */

		cairo_rotate(context, qiblarad);
		double dx = (388 * sin(qiblarad) + 290 * cos(qiblarad)) - 290;
		double dy = (388 * cos(qiblarad) - 290 * sin(qiblarad)) - 388;
		cairo_translate(context, dx, dy); /* recenter */

		rsvg_handle_render_cairo_sub(compass, context, "#ibra");
#else
		actual = height < width ? height : width;

		double nq = (actual - 10) / 2;

		/* transform: translate to center and make the y axis point to qibla */
		cairo_translate(context, width/2, height/2);
		cairo_rotate(context, qiblarad + G_PI);

		/* the circle background */
		cairo_set_source_rgba(context, 0, 0, 0, 0.08);
		cairo_arc(context, 0, 0, nq, 0, 2*G_PI);
		cairo_fill(context);

		/* the circle */
		cairo_set_source_rgb(context, 0, 1, 0);
		cairo_arc(context, 0, 0, nq, 0, 2*G_PI);
		cairo_stroke(context);

		/* the center dot */
		cairo_set_source_rgb(context, 0, 0, 1);
		cairo_arc(context, 0, 0, 2.5, 0, 2*G_PI);
		cairo_fill(context);

		/* the arrow */

		cairo_set_line_width(context, 2.0);
		cairo_move_to(context, 0, 0);
		cairo_line_to(context, 0, nq);
		cairo_stroke(context);

		cairo_move_to(context, 0, nq);
		cairo_rel_line_to(context, 3, -5);
		cairo_rel_line_to(context, -6, 0);
		cairo_close_path(context);
		cairo_fill(context);
#endif
	qiblabuf = g_malloc(300);
	g_snprintf(qiblabuf, 300,
		   "%s\n%d %s",
		   _("Qibla direction"),
		   (int) rint(fabs(qibla)),
		   (floor(qibla) == 0 || rint(fabs(qibla)) == 180) ? "" : qibla < 0 ? _("East") : _("West")
		   );
	}

	gtk_widget_set_tooltip_text(widget, qiblabuf);

	/* restore the transform */
	cairo_restore(context);

	g_free(qiblabuf);

	return TRUE;
}

void window_state_event_callback (GtkWidget *widget, 
		GdkEventWindowState *event)
{
	if ((event->new_window_state & GDK_WINDOW_STATE_ICONIFIED) &&
		( event->changed_mask & GDK_WINDOW_STATE_ICONIFIED ))
	{	
			gtk_widget_hide((GtkWidget *) gtk_builder_get_object(builder, "mainWindow"));
	}
}

void setup_widgets()
{
	GtkWidget * mainwindow = (GtkWidget *) gtk_builder_get_object(builder, "mainWindow");

	/* set the prayer names in the time table */
	/* done here so we don't duplicate translation */
	gchar * labeltext;
	labeltext = g_malloc(100);
	
	g_snprintf(labeltext, 100, "<b>%s:</b>", time_names[0]);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "subh"), labeltext);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "subhc"), labeltext);

	g_snprintf(labeltext, 100, "<b>%s:</b>", time_names[1]);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "shourouk"), labeltext);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "shouroukc"), labeltext);

	g_snprintf(labeltext, 100, "<b>%s:</b>", time_names[2]);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "duhr"), labeltext);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "duhrc"), labeltext);

	g_snprintf(labeltext, 100, "<b>%s:</b>", time_names[3]);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "asr"), labeltext);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "asrc"), labeltext);

	g_snprintf(labeltext, 100, "<b>%s:</b>", time_names[4]);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "maghreb"), labeltext);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "maghrebc"), labeltext);

	g_snprintf(labeltext, 100, "<b>%s:</b>", time_names[5]);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "isha"), labeltext);
	gtk_label_set_markup((GtkLabel *)	gtk_builder_get_object(builder, "ishac"), labeltext);

	g_free(labeltext);

#if USE_TRAY_ICON
	/* hide on minimise*/	

	g_signal_connect (mainwindow, "window-state-event", 
			G_CALLBACK (window_state_event_callback), NULL);
#else
	GtkWidget * closebutton = gtk_builder_get_object(builder, "closebutton");
	gtk_widget_hide(closebutton);
#endif

#if USE_NOTIFY
	/* Hide install notice */
	GtkWidget * label = (GtkWidget *)gtk_builder_get_object(builder, "installnotifnotice");
	gtk_widget_hide((GtkWidget *) label);
#else
	/* disable notification options */
	GtkWidget * check = (GtkWidget *) gtk_builder_get_object(builder, "yesnotif");
	gtk_widget_set_sensitive (check, FALSE);
	GtkWidget * notif_t =  (GtkWidget *) gtk_builder_get_object(builder, "notiftime");
	gtk_widget_set_sensitive (notif_t, FALSE);
	GtkWidget * notif_c =  (GtkWidget *) gtk_builder_get_object(builder, "notifmenucheck");
	gtk_widget_set_sensitive (notif_c, FALSE);
#endif

	qibla_drawing_area = (GtkWidget *)gtk_builder_get_object(builder, "qibla_drawing_area");
#if USE_RSVG
	gchar * filename = g_build_filename(MINBAR_DATADIR ,"Compass.svg", NULL);
	compass = rsvg_handle_new_from_file(filename, NULL); /* err */
	g_free(filename);
#endif
}

gboolean locations_loaded = FALSE;

void load_locations_callback()
{
	if (!locations_loaded)
	{
		 setup_locations_applet();
		 locations_loaded = TRUE;
	}
	gtk_widget_show((GtkWidget *) gtk_builder_get_object(builder, "locationsDialog"));
}

GtkWidget * tree;
void setup_locations_applet()
{
	GtkTreeStore *model;
	/*GtkTreeSelection *selection;*/
	GtkWidget *scrolled_window;
	GtkTreeViewColumn *column;
	GtkCellRenderer *cell_renderer;
	/*WeatherLocation *current_location;*/

       	scrolled_window	= (GtkWidget*) gtk_builder_get_object(builder, "location_list_scroll");

	model 		= gtk_tree_store_new (GWEATHER_XML_NUM_COLUMNS, G_TYPE_STRING, G_TYPE_POINTER);
	tree 		= gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
	
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (tree), FALSE);

	/* Add a colum for the locations */
	cell_renderer 	= gtk_cell_renderer_text_new ();
	column 		= gtk_tree_view_column_new_with_attributes ("not used", cell_renderer,
				       "text", GWEATHER_XML_COL_LOC, NULL);
	gtk_tree_view_append_column ((GtkTreeView *)tree, column);
	gtk_tree_view_set_expander_column (GTK_TREE_VIEW (tree), column);

	gtk_container_add (GTK_CONTAINER (scrolled_window), tree);
	gtk_widget_show (tree);
	gtk_widget_show (scrolled_window);

	/* current_location = weather_location_clone (gw_applet->gweather_pref.location);*/ 
	/* load locations from xml file */
	if (gweather_xml_load_locations ((GtkTreeView *)tree, NULL))
	{
		GtkWidget *d;
		d = gtk_message_dialog_new (NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
			    _("Failed to load the Locations XML "
			      "database.  Please report this as "
			      "a bug."));
		gtk_dialog_run (GTK_DIALOG (d));
		gtk_widget_destroy (d);
	}
}

void locationok_callback()
{
	if(!tree)
		return;

	GtkTreeSelection * selection;
	selection = gtk_tree_view_get_selection ((GtkTreeView *)tree);
		
	GtkTreeModel *model;	
	GtkTreeIter iter;

	if (!gtk_tree_selection_get_selected (selection, &model, &iter))
		return;

	WeatherLocation *loc = NULL;

	gtk_tree_model_get (model, &iter, GWEATHER_XML_COL_POINTER, &loc, -1);

	if (!loc)
		return;
	gtk_widget_hide((GtkWidget *) gtk_builder_get_object(builder, "locationsDialog"));
	/*g_print("%s, %f,%f \n", loc->name, loc->longitude, loc->latitude);*/

	config->latitude = (loc->latitude * 180) / M_PI;
	config->longitude = (loc->longitude * 180) / M_PI;
	config->city = g_strdup(loc->name);  /* is this ok or should I use g_strdup ? */

	/* update the editcity dialog (copied from init_vars) */

		GtkWidget*  entrywidget;	

	/* Setting what was found to editcity dialog*/
	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "latitude");	
	gtk_spin_button_set_value((GtkSpinButton *)entrywidget, config->latitude);

	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "longitude");	
	gtk_spin_button_set_value((GtkSpinButton *)entrywidget, config->longitude);

	entrywidget = (GtkWidget *) gtk_builder_get_object( builder, "cityname");	
	gtk_entry_set_text((GtkEntry *)entrywidget, config->city);
}



void
find_entry_changed (GtkEditable *entry/*, GWeatherPref *pref*/)
{
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	GtkTreePath *path;
	GtkWidget *nextbutton;
	const gchar *location;

	nextbutton = (GtkWidget *) gtk_builder_get_object(builder, "findnextbutton");

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree));

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));
	gtk_tree_model_get_iter_first (model, &iter);

	location = gtk_entry_get_text (GTK_ENTRY (entry));

	if (find_location (model, &iter, location, TRUE)) 
	{
		gtk_widget_set_sensitive (nextbutton , TRUE);

		path = gtk_tree_model_get_path (model, &iter);
		gtk_tree_view_expand_to_path (GTK_TREE_VIEW(tree), path);
		gtk_tree_selection_select_iter (selection, &iter);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(tree), path, NULL, TRUE, 0.5, 0);

		gtk_tree_path_free (path);
	} 
	else 
	{
		gtk_widget_set_sensitive (nextbutton, FALSE);
	}
}


/* shamelessly copied from gweather code */
gboolean
find_location (GtkTreeModel *model, GtkTreeIter *iter, const gchar *location, gboolean go_parent)
{
	GtkTreeIter iter_child;
	GtkTreeIter iter_parent;
	gchar *aux_loc;
	gboolean valid;
	int len;

	len = strlen (location);

	if (len <= 0) {
		return FALSE;
	}
	
	do {
		
		gtk_tree_model_get (model, iter, GWEATHER_XML_COL_LOC, &aux_loc, -1);

		if (g_ascii_strncasecmp (aux_loc, location, len) == 0) {
			g_free (aux_loc);
			return TRUE;
		}

		if (gtk_tree_model_iter_has_child (model, iter)) {
			gtk_tree_model_iter_nth_child (model, &iter_child, iter, 0);

			if (find_location (model, &iter_child, location, FALSE)) {
				/* Manual copying of the iter */
				iter->stamp = iter_child.stamp;
				iter->user_data = iter_child.user_data;
				iter->user_data2 = iter_child.user_data2;
				iter->user_data3 = iter_child.user_data3;

				g_free (aux_loc);
				
				return TRUE;
			}
		}
		g_free (aux_loc);

		valid = gtk_tree_model_iter_next (model, iter);		
	} while (valid);

	if (go_parent) {
		iter_parent = *iter;
		if (gtk_tree_model_iter_parent (model, iter, &iter_parent) && gtk_tree_model_iter_next (model, iter)) {
			return find_location (model, iter, location, TRUE);
		}
	}
	return FALSE;
}

void
find_next_clicked (GtkButton *button)
{
	GtkTreeModel *model;
	GtkEntry *entry;
	GtkTreeSelection *selection;
	GtkTreeIter iter;
	GtkTreeIter iter_parent;
	GtkTreePath *path;
	const gchar *location;

	model = gtk_tree_view_get_model (GTK_TREE_VIEW(tree));
	entry = GTK_ENTRY (gtk_builder_get_object(builder, "location_search_entry"));

	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree));

	if (gtk_tree_selection_count_selected_rows (selection) >= 1) {
		gtk_tree_selection_get_selected (selection, &model, &iter);
		/* Select next or select parent */
		if (!gtk_tree_model_iter_next (model, &iter)) {
			iter_parent = iter;
			if (!gtk_tree_model_iter_parent (model, &iter, &iter_parent) || !gtk_tree_model_iter_next (model, &iter))
				gtk_tree_model_get_iter_first (model, &iter);
		}

	} else {
		gtk_tree_model_get_iter_first (model, &iter);
	}

	location = gtk_entry_get_text (entry);

	if (find_location (model, &iter, location, TRUE)) {
		gtk_widget_set_sensitive ((GtkWidget *)button, TRUE);

		path = gtk_tree_model_get_path (model, &iter);
		gtk_tree_view_expand_to_path (GTK_TREE_VIEW(tree), path);
		gtk_tree_selection_select_path (selection, path);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(tree), path, NULL, TRUE, 0.5, 0);

		gtk_tree_path_free (path);
	} else {
		gtk_widget_set_sensitive ((GtkWidget * )button, FALSE);
	}
}


