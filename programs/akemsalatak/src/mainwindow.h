#ifndef GTKMM_MAINWINDOW_H
#define GTKMM_MAINWINDOW_H

    Glib::ustring  Ini;
    Glib::ustring CityName;
    Glib::ustring DegreeLong;
    Glib::ustring DegreeLat;
    double GMTdiff;
    double SeaLev;
    bool DST;
    double FajrAng;  
    double IshaAng;
    int FajrINV;
    int IshaINV;
    bool Mathhab;          /* Shaf'i =false and Hanafi = true */
    int PrayMethod;
    Glib::ustring RadioPlayer,RadioPlayerArg;
    int cityIndex=0;
    gint QuranChannel;

#include "gettextstr.h"
#include "quran.cc"
#include <gtkmm.h>

#include "aboutwindow.cc"
#include "prefwindow.cc"



class MainWindow : public Gtk::Window
{
public:
  MainWindow();
  virtual ~MainWindow();

protected:
  virtual bool on_TimeEvent_button_press(GdkEventButton *event);
  virtual bool on_PICEvent_button_press(GdkEventButton *event);
  virtual bool on_PICEvent_button_release(GdkEventButton *event);
  virtual bool window_motion(GdkEventMotion* event);
  virtual void Change_Time_Mode(void);
  virtual void on_menu_quran(void);
  virtual void on_menu_hijrycalendar(void);
  virtual void on_menu_prayerstime(void);
  virtual void on_menu_preferences(void);
  virtual void on_menu_about(void);
  virtual void on_menu_quit(void);
  virtual void on_menu_top(void);
   virtual void MainWindow::Set_Time_Mod(void);

	bool dragging,isRadio,isTop;
	double drag_x,drag_y;
	gint TimeMod;


	Gtk::VBox m_VBox;
	Gtk::Label labTimes,labPic;
	Gtk::Image *imgKaba;
	Gtk::EventBox TimeEvent,PicEvent;
	Gtk::Tooltips PicTips,TimeTips;
	Gtk::Menu m_Menu_Popup;
	Gtk::Image PrefImage,QuitImage,AboutImage;


};
void MainWindow::Set_Time_Mod(void) 
{
		switch(TimeMod){
			case 0:
			  labTimes.set_text(_("<span  foreground=\"red\">Fajr:     04:28</span>\nDohr:     11:39\nAser:     14:34\nMgrb:     16:53\nIsha:     18:57"));
			break;
			case 1:
			  labTimes.set_text(_("<span  foreground=\"red\">1</span>"));
			break;
			case 2:
			  labTimes.set_text(_("<span  foreground=\"green\">2</span>"));
			break;
		}
//	  labTimes.set_selectable(TRUE);
	  labTimes.set_use_markup(TRUE);
	  labTimes.set_justify(Gtk::JUSTIFY_CENTER);


}
void MainWindow::Change_Time_Mode(void) 
{
		TimeMod++;
		if ( TimeMod==3 ) TimeMod=0;
		MainWindow::Set_Time_Mod();

}
void MainWindow::on_menu_top(void)
	{
	if(isTop == FALSE) {
	get_window()->raise ();
	}else{
	get_window()->lower ();
	}
  realize();
isTop=!isTop;

	}

void MainWindow::on_menu_quran(void)
	{
	Glib::ustring RadioCMD;
RadioCMD.assign(RadioPlayer);
RadioCMD.append(" ");
RadioCMD.append(RadioPlayerArg);
RadioCMD.append(" ");
RadioCMD.append(Radios[QuranChannel].URL);
RadioCMD.append(" &");

	if (isRadio == true) {
RadioCMD.assign("killall -9 ");
RadioCMD.append(basename(RadioPlayer.data()));
	}
system(RadioCMD.data());
isRadio=!isRadio;
}

void MainWindow::on_menu_hijrycalendar(void)
	{

	}

void MainWindow::on_menu_prayerstime(void)
	{


	}

void MainWindow::on_menu_preferences(void)
	{
	PrefWindow PrefWindow;
	Gtk::Main::run(PrefWindow);
	}

void MainWindow::on_menu_about(void)
	{
	AboutWindow Aboutwindow;
	Gtk::Main::run(Aboutwindow);
	}

void MainWindow::on_menu_quit(void)
	{
 hide();
	}



bool MainWindow::on_TimeEvent_button_press(GdkEventButton* event)
{
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 1) ) Change_Time_Mode();
  return true;
}


bool MainWindow::window_motion (GdkEventMotion* event) 
{
	gint x;
	gint y;
	gint mx, my;
	double x_delta;
	double y_delta;

	if (!dragging) return TRUE;
	get_size(mx,my);
	x_delta = event->x_root - drag_x;
	y_delta = event->y_root - drag_y;
	get_window()->get_root_origin (x, y);
	get_window()->move_resize ((gint) floor (x + x_delta), (gint) floor (y + y_delta),mx,my);

	drag_x = event->x_root;
	drag_y = event->y_root;

	return TRUE;
}
 


bool MainWindow::on_PICEvent_button_press(GdkEventButton* event)
{
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 2) ){
		drag_x = event->x_root;
		drag_y = event->y_root;
		dragging=TRUE;
	}
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) m_Menu_Popup.popup(event->button, event->time);

  return true;
}
bool MainWindow::on_PICEvent_button_release(GdkEventButton* event)
{
  dragging=FALSE;
  return true;
}
#endif //GTKMM_MAINWINDOW_H
