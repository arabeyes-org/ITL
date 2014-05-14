#include <iostream>

#include "gettextstr.h"
#include "mainwindow.h"
#include "kaba_gif_data.cc"
#include "cities.cc"
#include <gtkmm/stock.h>
#include <sys/stat.h>


MainWindow::MainWindow()
:  PrefImage(Gtk::Stock::PREFERENCES, Gtk::ICON_SIZE_MENU),
QuitImage(Gtk::Stock::QUIT, Gtk::ICON_SIZE_MENU),
AboutImage(Gtk::Stock::DIALOG_QUESTION, Gtk::ICON_SIZE_MENU)
{

  Ini.assign(Glib::get_home_dir());
  Ini.append("/.");
  Ini.append(Glib::get_prgname());
  Ini.append("/");
  mkdir(Ini.data(),0740);

	dragging=isTop=isRadio=FALSE;
	drag_x = 0;
	drag_y = 0;
	QuranChannel=0;

/* S T A R T   O F    
--- P r o g r a m       S e t t i n g s ---
*/

    Glib::ustring  IniFile;
 FILE *Fp;

IniFile.assign(Ini.data());
IniFile.append("settings");

 Fp = fopen(IniFile.data(), "r");
if(!Fp) {
	TimeMod=0;

	RadioPlayer.assign("/usr/bin/mplayer");
	RadioPlayerArg.assign("-cache 300 --display :0");

     cityIndex=0;
     CityName= Cities[cityIndex].CityName;
     DegreeLong=Cities[cityIndex].DegreeLong;
     DegreeLat=Cities[cityIndex].DegreeLat;
     GMTdiff=Cities[cityIndex].GMTdiff;
     SeaLev=Cities[cityIndex].SeaLev;
     DST=Cities[cityIndex].DST;
    FajrAng = 19;
    IshaAng = 19;
    FajrINV=0;
    IshaINV=90;
    PrayMethod=0;
   


}else {

   char tmp1[50],tmp2[10],tmp3[10],tmp4[255],tmp5[255];

//  fscanf(Fp, "%d\n%%s\n%s\n%s\n%[^\n]\n%d\n%f\n%f\n%d\n%d\n%d\n%[^\n]\n%[^\n]\n%d",&TimeMod ,&tmp1,&tmp2,&tmp3,&dtmp1, &SeaLev, &DST, &FajrAng, &IshaAng, &FajrINV, &IshaINV, &PrayMethod,&tmp4  , &tmp5 , &cityIndex);

fscanf(Fp,"%d\n",&TimeMod);
fscanf(Fp,"%[^\n]\n",&tmp1);
fscanf(Fp,"%[^\n]\n",&tmp2);
fscanf(Fp,"%[^\n]\n",&tmp3);
fscanf(Fp,"%lf\n",&GMTdiff);
fscanf(Fp,"%lf\n",&SeaLev);
fscanf(Fp,"%d\n",&DST);
fscanf(Fp,"%lf\n",&FajrAng);
fscanf(Fp,"%lf\n",&IshaAng);
fscanf(Fp,"%d\n",&FajrINV);
fscanf(Fp,"%d\n",&IshaINV);
fscanf(Fp,"%d\n",&PrayMethod);
fscanf(Fp,"%[^\n]\n",&tmp4);
fscanf(Fp,"%[^\n]\n",&tmp5);
fscanf(Fp,"%d\n",&cityIndex);

 CityName.assign(tmp1);
DegreeLong.assign(tmp2);
DegreeLat.assign(tmp3);
RadioPlayer.assign(tmp4);
RadioPlayerArg.assign(tmp5);
  fclose(Fp);

 fprintf(stdout, "\n\n\n");

 fprintf(stdout, "%d\n",TimeMod);
 fprintf(stdout, "%s\n",CityName.data());
 fprintf(stdout, "%s\n",DegreeLong.data());
 fprintf(stdout, "%s\n",DegreeLat.data());
 fprintf(stdout, "%f\n",GMTdiff);
 fprintf(stdout, "%f\n",SeaLev);
 fprintf(stdout, "%d\n",DST);
 fprintf(stdout, "%f\n",FajrAng);
 fprintf(stdout, "%f\n",IshaAng);
 fprintf(stdout, "%d\n",FajrINV);
 fprintf(stdout, "%d\n",IshaINV);
 fprintf(stdout, "%d\n",PrayMethod);
 fprintf(stdout, "%s\n",RadioPlayer.data());
 fprintf(stdout, "%s\n",RadioPlayerArg.data());
 fprintf(stdout, "%d\n",cityIndex);

/*fprintf(stdout,"\nName: -%s-",CityName.data());
fprintf(stdout,"\nRadioPlayer: -%s-",RadioPlayer.data());
fprintf(stdout,"\nRadioPlayerArg: --");
fprintf(stdout,"\nRadioPlayerArg: -%s-",RadioPlayerArg.data());*/



}
/* E N D   O F    
--- P r o g r a m       S e t t i n g s ---
*/

	Set_Time_Mod();

  set_title(_("Akem Salatk"));
  set_skip_taskbar_hint(TRUE);
  set_skip_pager_hint(TRUE);



   Glib::RefPtr<Gdk::PixbufLoader> _imgKaba_loader=Gdk::PixbufLoader::create();
   _imgKaba_loader->write(kaba_gif_data, sizeof kaba_gif_data);
   _imgKaba_loader->close();
   imgKaba = Gtk::manage(new class Gtk::Image(_imgKaba_loader->get_pixbuf()));
   

  add(m_VBox);
   m_VBox.pack_start(PicEvent, Gtk::PACK_SHRINK);
   m_VBox.pack_start(TimeEvent, Gtk::PACK_SHRINK);

   PicEvent.add(*imgKaba);
   TimeEvent.add(labTimes);

//Gdk::POINTER_MOTION_MASK  | Gdk::POINTER_MOTION_HINT_MASK
  PicEvent.set_events(Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK  | Gdk::POINTER_MOTION_HINT_MASK);
  PicEvent.signal_motion_notify_event().connect(
     SigC::slot(*this, &MainWindow::window_motion) );
  PicEvent.signal_button_press_event().connect(
    SigC::slot(*this, &MainWindow::on_PICEvent_button_press) );
  PicEvent.signal_button_release_event().connect(
    SigC::slot(*this, &MainWindow::on_PICEvent_button_release) );

  PicTips.set_tip(PicEvent, _("The Holy Kaba"));

  TimeEvent.set_events(Gdk::BUTTON_PRESS_MASK);
  TimeEvent.signal_button_press_event().connect(
    SigC::slot(*this, &MainWindow::on_TimeEvent_button_press) );

  TimeTips.set_tip(TimeEvent, _("Click here to change the mode"));

  //Fill menu:
  {
  	Gtk::Menu::MenuList& menulist = m_Menu_Popup.items();

	menulist.push_back( Gtk::Menu_Helpers::CheckMenuElem( "Quran Radio/TV", Gtk::Menu::AccelKey("<control>r"),
      SigC::slot(*this, &MainWindow::on_menu_quran) ) );

   menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  	menulist.push_back( Gtk::Menu_Helpers::MenuElem(_("_Hijry Calendar"),
      SigC::slot(*this, &MainWindow::on_menu_hijrycalendar) ) );

  	menulist.push_back( Gtk::Menu_Helpers::MenuElem(_("Prayers _Time"),
      SigC::slot(*this, &MainWindow::on_menu_prayerstime) ) );

   menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());


	menulist.push_back( Gtk::Menu_Helpers::CheckMenuElem( _("_On Top"), Gtk::Menu::AccelKey("<control>o"),
      SigC::slot(*this, &MainWindow::on_menu_top) ) );

   menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  	menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem(_("_Preferences"), Gtk::Menu::AccelKey("<control>p"),PrefImage,
      SigC::slot(*this, &MainWindow::on_menu_preferences) ) );


  	menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem(_("_About"), AboutImage,
      SigC::slot(*this, &MainWindow::on_menu_about) ) );

   menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

  	menulist.push_back( Gtk::Menu_Helpers::ImageMenuElem(_("Q_uit"), Gtk::Menu::AccelKey("<control>q"),QuitImage,
      SigC::slot(*this, &MainWindow::on_menu_quit) ) );


  }
  m_Menu_Popup.accelerate(*this);


  realize();
  get_window()->set_decorations (Gdk::WMDecoration ());
  show_all_children();
}

MainWindow::~MainWindow()
{
    Glib::ustring  IniFile;
 FILE *Fp;

IniFile.assign(Ini.data());
IniFile.append("settings");

 Fp = fopen(IniFile.data(), "w");
 fprintf(Fp, "%d\n",TimeMod);
 fprintf(Fp, "%s\n",CityName.data());
 fprintf(Fp, "%s\n",DegreeLong.data());
 fprintf(Fp, "%s\n",DegreeLat.data());
 fprintf(Fp, "%f\n",GMTdiff);
 fprintf(Fp, "%f\n",SeaLev);
 fprintf(Fp, "%d\n",DST);
 fprintf(Fp, "%f\n",FajrAng);
 fprintf(Fp, "%f\n",IshaAng);
 fprintf(Fp, "%d\n",FajrINV);
 fprintf(Fp, "%d\n",IshaINV);
 fprintf(Fp, "%d\n",PrayMethod);
 fprintf(Fp, "%s\n",RadioPlayer.data());
 fprintf(Fp, "%s\n",RadioPlayerArg.data());
 fprintf(Fp, "%d\n",cityIndex);
 fclose(Fp);
}

