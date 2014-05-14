#include <iostream>

#include "prefwindow.h"
#include "gettextstr.h"
#include <gtkmm/stock.h>



PrefWindow::PrefWindow()
: m_Table1(8,2),
  m_Table2(7,2),
  m_Table3(3,2),
  m_RadioButton1(_("Yes")),
  m_RadioButton2(_("No")),
  m_adjustment1(GMTdiff, -14.0, 14.0, 0.5, 1.0, 0.0),
  m_adjustment2(SeaLev, -10000.0, 10000.0, 0.5, 5.0, 0.0),
  m_adjustment3(FajrAng, -1000.0, 1000.0, 0.5, 5.0, 0.0),
  m_adjustment4(IshaAng, -1000.0, 1000.0, 0.5, 5.0, 0.0),
  m_Label1(_("You City:")),
  m_Label2(_("City Name:")),  
  m_Label3(_("Longitude:")),  
  m_Label4(_("Latitude:")),  
  m_Label5(_("Time zone:")),  
  m_Label6(_("Sea level:")),
  m_Label7(_("Daylight Saving:")),
  m_Label8(_("if you can't find your city\nplease check http://www.islamicfinder.org")),
  m_Label9(_("Calculation Method:")),
  m_Label10(_("Fajr angle:")),
  m_Label11(_("Isha angle:")),
  m_Label12(_("Fajr Interval:")),
  m_Label13(_("Isha Interval:")),
  m_Label14(_("Aser Mthhab:")),
  m_Label15(_("For more information please check\nhttp://www.islamicfinder.org/HelpPrayCal.html")),
  m_Label16(_("Radio Player:")),
  m_Label17(_("Player options:")),
  m_Label18(_("Channel:"))
{

  set_title(_("Preferences"));
	set_title(Glib::get_prgname());
  set_modal(true);
  property_window_position().set_value(Gtk::WIN_POS_CENTER);
  set_resizable(false);
  property_destroy_with_parent().set_value(true);
  set_border_width(10);
  set_default_size(400, 200);

   Gtk::MenuItem *meth1= NULL,*meth2= NULL,*meth3= NULL,*meth4= NULL,*meth5= NULL,*meth6= NULL;
   Gtk::MenuItem *FajrINVType1= NULL,*FajrINVType2= NULL,*FajrINVType3= NULL;
   Gtk::MenuItem *IshaINVType1= NULL,*IshaINVType2= NULL,*IshaINVType3= NULL;
   Gtk::MenuItem *MathhabType1= NULL,*MathhabType2= NULL;

   RadioMenu = Gtk::manage(new class Gtk::Menu());
   methodMenu = Gtk::manage(new class Gtk::Menu());
   FajrMenu = Gtk::manage(new class Gtk::Menu());
   IshaMenu = Gtk::manage(new class Gtk::Menu());
   MathhabMenu = Gtk::manage(new class Gtk::Menu());
   CitiesMenu = Gtk::manage(new class Gtk::Menu());

   RadioOption  = Gtk::manage(new class Gtk::OptionMenu());
   methodOption  = Gtk::manage(new class Gtk::OptionMenu());
   FajrOption  = Gtk::manage(new class Gtk::OptionMenu());
   IshaOption  = Gtk::manage(new class Gtk::OptionMenu());
   MathhabOption  = Gtk::manage(new class Gtk::OptionMenu());
   CitiesOption  = Gtk::manage(new class Gtk::OptionMenu());


for(gint i=0; i< NRADIOS;i++) 
   RadioMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(Radios[i].Name));

   RadioOption->set_flags(Gtk::CAN_FOCUS);
   RadioOption->set_history(0);
   RadioOption->set_menu(*RadioMenu);
   RadioOption->signal_changed().connect(SigC::slot(*this, &PrefWindow::on_radio_changed));

   MathhabMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("Shaf'i")));
   MathhabType1 = (Gtk::MenuItem *) &MathhabMenu->items().back();
   MathhabMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("Hanafi")));
   MathhabType2 = (Gtk::MenuItem *) &MathhabMenu->items().back();


   IshaMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("0 (not used)")));
   IshaINVType1 = (Gtk::MenuItem *) &IshaMenu->items().back();
   IshaMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("90")));
   IshaINVType2 = (Gtk::MenuItem *) &IshaMenu->items().back();
   IshaMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("120")));
   IshaINVType3 = (Gtk::MenuItem *) &IshaMenu->items().back();

   FajrMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("0 (not used)")));
   FajrINVType1 = (Gtk::MenuItem *) &FajrMenu->items().back();
   FajrMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("90")));
   FajrINVType2 = (Gtk::MenuItem *) &FajrMenu->items().back();
   FajrMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("120")));
   FajrINVType3 = (Gtk::MenuItem *) &FajrMenu->items().back();


   methodMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("Umm Al-Qura Committee")));
   meth1 = (Gtk::MenuItem *)&methodMenu->items().back();
   methodMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("Egyptian General Authority of Survey")));
   meth2 = (Gtk::MenuItem *) &methodMenu->items().back();
   methodMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("University Of Islamic Sciences, Karachi")));
   meth3 = (Gtk::MenuItem *) &methodMenu->items().back();
   methodMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("Muslim World League")));
   meth4 = (Gtk::MenuItem *) &methodMenu->items().back();
   methodMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("Islamic Society of North America")));
   meth5 = (Gtk::MenuItem *) &methodMenu->items().back();
   methodMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(_("Custom")));
   meth6 = (Gtk::MenuItem *) &methodMenu->items().back();

	methodMenu->set_active(PrayMethod);
	if (FajrINV == 0) FajrMenu->set_active(0); else if(FajrINV ==90)  FajrMenu->set_active(1);else if(FajrINV ==120)  FajrMenu->set_active(2);
	if (IshaINV == 0) IshaMenu->set_active(0); else if(IshaINV ==90)  IshaMenu->set_active(1);else if(IshaINV ==120)  IshaMenu->set_active(2);
	if (Mathhab == false) MathhabMenu->set_active(0); else if(Mathhab ==true)  MathhabMenu->set_active(1);

   meth1->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_meth1_activate));
   meth2->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_meth2_activate));
   meth3->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_meth3_activate));
   meth4->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_meth4_activate));
   meth5->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_meth5_activate));
   meth6->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_meth6_activate));


   MathhabOption->set_flags(Gtk::CAN_FOCUS);
   MathhabOption->set_history(0);
   MathhabOption->set_menu(*MathhabMenu);

   MathhabType1->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_MathhabType1_activate));
   MathhabType2->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_MathhabType2_activate));

   IshaOption->set_flags(Gtk::CAN_FOCUS);
   IshaOption->set_history(0);
   IshaOption->set_menu(*IshaMenu);

   IshaINVType1->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_IshaINVType1_activate));
   IshaINVType2->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_IshaINVType2_activate));
   IshaINVType3->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_IshaINVType3_activate));

   FajrOption->set_flags(Gtk::CAN_FOCUS);
   FajrOption->set_history(0);
   FajrOption->set_menu(*FajrMenu);

   FajrINVType1->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_FajrINVType1_activate));
   FajrINVType2->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_FajrINVType2_activate));
   FajrINVType3->signal_activate().connect(SigC::slot(*this, &PrefWindow::on_FajrINVType3_activate));

   methodOption->set_flags(Gtk::CAN_FOCUS);
   methodOption->set_history(0);
   methodOption->set_menu(*methodMenu);

  add(m_VBox);

  m_Notebook.set_border_width(15);
  m_Notebook.append_page(m_Table1, _("Location"));
  m_Notebook.append_page(m_Table2, _("Calculation Method"));
  m_Notebook.append_page(m_Table3, _("Radio"));


  Gtk::Button *m_Button_Quit = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-close")));
  m_Button_Quit->signal_clicked().connect(SigC::slot(*this, &Gtk::Widget::hide));
  m_ButtonBox.pack_start(*m_Button_Quit, Gtk::PACK_SHRINK);

  m_Table1.attach(m_Label1,0,1,0,1);
  m_Table1.attach(m_Label2,0,1,1,2);
  m_Table1.attach(m_Label3,0,1,2,3);
  m_Table1.attach(m_Label4,0,1,3,4);
  m_Table1.attach(m_Label5,0,1,4,5);
  m_Table1.attach(m_Label6,0,1,5,6);
  m_Table1.attach(m_Label7,0,1,6,7);

  m_Table1.attach(*CitiesOption,1,2,0,1);
  m_Table1.attach(m_Entry1,1,2,1,2);
  m_Table1.attach(m_Entry2,1,2,2,3);
  m_Table1.attach(m_Entry3,1,2,3,4);
  m_Table1.attach(m_SpinButton1,1,2,4,5);
  m_Table1.attach(m_SpinButton2,1,2,5,6);
  m_Table1.attach(m_HBox,1,2,6,7);

  m_Table1.attach(m_Label8,0,2,7,8);

  m_Table2.attach(m_Label9,0,1,0,1);
  m_Table2.attach(m_Label10,0,1,1,2);
  m_Table2.attach(m_Label11,0,1,2,3);
  m_Table2.attach(m_Label12,0,1,3,4);
  m_Table2.attach(m_Label13,0,1,4,5);
  m_Table2.attach(m_Label14,0,1,5,6);
  m_Table2.attach(m_Label15,0,2,6,7);


  m_Table2.attach(*methodOption,1,2,0,1);

  m_Table2.attach(m_SpinButton3,1,2,1,2);
  m_Table2.attach(m_SpinButton4,1,2,2,3);

  m_Table2.attach(*FajrOption,1,2,3,4);
  m_Table2.attach(*IshaOption,1,2,4,5);
  m_Table2.attach(*MathhabOption,1,2,5,6);

  m_Table3.attach(m_Label16,0,1,0,1);
  m_Table3.attach(m_Label17,0,1,1,2);
  m_Table3.attach(m_Label18,0,1,2,3);

  m_Table3.attach(m_Entry4,1,2,0,1);
  m_Table3.attach(m_Entry5,1,2,1,2);
  m_Table3.attach(*RadioOption,1,2,2,3);

	  m_Label8.set_use_markup(TRUE);
	  m_Label8.set_selectable(TRUE);
	  m_Label8.set_justify(Gtk::JUSTIFY_CENTER);
	  m_Label15.set_use_markup(TRUE);
	  m_Label15.set_selectable(TRUE);
	  m_Label15.set_justify(Gtk::JUSTIFY_CENTER);


m_SpinButton1.configure(m_adjustment1, 1.0, 2);
m_SpinButton2.configure(m_adjustment2, 1.0, 2);

m_SpinButton3.configure(m_adjustment3, 1.0, 2);
m_SpinButton4.configure(m_adjustment4, 1.0, 2);

      Gtk::RadioButton::Group group = m_RadioButton1.get_group();
      m_RadioButton2.set_group(group);

m_HBox.pack_start(m_RadioButton1);
m_HBox.pack_start(m_RadioButton2);

//  m_Table1.set_row_spacings(6);
//  m_Table1.set_col_spacings(6);
  m_Table1.set_spacings(10);
  m_VBox.pack_start(m_Notebook);
  m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);

  m_Tips2.set_tip(m_Entry1, _("Your city name"));
  m_Tips3.set_tip(m_Entry2, _("Longitude in decimal degree"));
  m_Tips4.set_tip(m_Entry3, _("Latitude in decimal degree"));
  m_Tips5.set_tip(m_SpinButton1, _("GMT difference"));
  m_Tips6.set_tip(m_SpinButton2, _("Height above Sea level (in meters)"));
  m_Tips7.set_tip(m_RadioButton1, _("Daylight savings time.\nSetting this to yes should add 1 hour to all the calculated prayer times"));
  m_Tips7.set_tip(m_RadioButton2, _("Daylight savings time.\nSetting this to yes should add 1 hour to all the calculated prayer times"));
/*
	for(gint i=0; i< NCITIES;i++)  listStrings.push_back(Cities[i].CityName);
  m_Combo1.set_popdown_strings(listStrings);
//  m_Combo1.set_value_in_list(true,false);
  Gtk::Entry* pEntry = m_Combo1.get_entry();
   pEntry->set_text(CityName);
  m_Combo1.get_entry()->signal_changed().connect( SigC::slot(*this, &PrefWindow::on_combo_changed) );
*/
for(gint i=0; i< NCITIES;i++) 
   CitiesMenu->items().push_back(Gtk::Menu_Helpers::MenuElem(Cities[i].CityName));
   CitiesMenu->set_active(cityIndex);

  CitiesOption->set_flags(Gtk::CAN_FOCUS);
  CitiesOption->set_history(0);
  CitiesOption->set_menu(*CitiesMenu);
  CitiesOption->signal_changed().connect(SigC::slot(*this, &PrefWindow::on_cities_changed));

  m_Entry1.set_text(CityName);
  m_Entry2.set_text(DegreeLong);
  m_Entry3.set_text(DegreeLat);

  m_Entry4.set_text(RadioPlayer);
  m_Entry5.set_text(RadioPlayerArg);

  m_SpinButton1.set_value(GMTdiff);
  m_SpinButton2.set_value(SeaLev);
  m_RadioButton1.set_active(DST);
  m_RadioButton2.set_active(!DST);

  m_SpinButton3.set_value(FajrAng);
  m_SpinButton4.set_value(IshaAng);

  enable_or_disable_ent();
  realize();
  show_all_children();
}

PrefWindow::~PrefWindow()
{
     CityName= m_Entry1.get_text();
     DegreeLong= m_Entry2.get_text();
     DegreeLat= m_Entry3.get_text();
     GMTdiff=m_SpinButton1.get_value();
     SeaLev=m_SpinButton2.get_value();
     DST=  m_RadioButton1.get_active();
     RadioPlayer= m_Entry4.get_text();
     RadioPlayerArg= m_Entry5.get_text();

	if(PrayMethod==5) {
	    FajrAng = m_SpinButton3.get_value();
	    IshaAng = m_SpinButton4.get_value();
	}

}
void PrefWindow::on_radio_changed()
{
QuranChannel = RadioOption->get_history();
}

void PrefWindow::on_cities_changed()
{
cityIndex=CitiesOption->get_history();
  m_Entry1.set_text(Cities[CitiesOption->get_history()].CityName);
  m_Entry2.set_text(Cities[CitiesOption->get_history()].DegreeLong);
  m_Entry3.set_text(Cities[CitiesOption->get_history()].DegreeLat);
  m_SpinButton1.set_value(Cities[CitiesOption->get_history()].GMTdiff);
  m_SpinButton2.set_value(Cities[CitiesOption->get_history()].SeaLev);
  m_RadioButton1.set_active(Cities[CitiesOption->get_history()].DST);
  m_RadioButton2.set_active(!Cities[CitiesOption->get_history()].DST);

}
/*
void PrefWindow::on_combo_changed()
{
  Gtk::Entry* pEntry = m_Combo1.get_entry();
  if(!pEntry) return ;
  

    Glib::ustring text = pEntry->get_text();

 //We seem to get 2 signals, one when the text is empty.
    if(!(text.empty()))
	for(gint i=0; i< NCITIES;i++)
		if ( text.compare(Cities[i].CityName) == 0) {
  m_Entry1.set_text(Cities[i].CityName);
  m_Entry2.set_text(Cities[i].DegreeLong);
  m_Entry3.set_text(Cities[i].DegreeLat);
  m_SpinButton1.set_value(Cities[i].GMTdiff);
  m_SpinButton2.set_value(Cities[i].SeaLev);
  m_RadioButton1.set_active(Cities[i].DST);
  m_RadioButton2.set_active(!Cities[i].DST);
	}
  
}
*/


void PrefWindow::on_meth1_activate()
{  
    FajrAng = 19;
    IshaAng = 19;
    FajrINV=0;
    IshaINV=90;
    PrayMethod=0;
enable_or_disable_ent();
}

void PrefWindow::on_meth2_activate()
{  

    FajrAng = 19.5;
    IshaAng = 17.5;
    FajrINV=0;
    IshaINV=0;
    PrayMethod=1;
enable_or_disable_ent();
}

void PrefWindow::on_meth3_activate()
{  
  
    FajrAng = 18;
    IshaAng = 18;
    FajrINV=0;
    IshaINV=0;
    PrayMethod=2;
enable_or_disable_ent();
}

void PrefWindow::on_meth4_activate()
{  
    FajrAng = 18;
    IshaAng = 17;
    FajrINV=0;
    IshaINV=0;
    PrayMethod=3;
enable_or_disable_ent();
}

void PrefWindow::on_meth5_activate()
{  
    FajrAng = 15;
    IshaAng = 15;
    FajrINV=0;
    IshaINV=0;
    PrayMethod=4;
enable_or_disable_ent();
}

void PrefWindow::on_MathhabType1_activate()
{
	Mathhab=false;
}

void PrefWindow::on_MathhabType2_activate()
{
	Mathhab=true;
}



void PrefWindow::on_IshaINVType1_activate()
{
	IshaINV=0;
}

void PrefWindow::on_IshaINVType2_activate()
{
	IshaINV=90;
}


void PrefWindow::on_IshaINVType3_activate()
{
	IshaINV=120;
}
void PrefWindow::on_FajrINVType1_activate()
{
	FajrINV=0;
}

void PrefWindow::on_FajrINVType2_activate()
{
	FajrINV=90;
}


void PrefWindow::on_FajrINVType3_activate()
{
	FajrINV=120;
}

void PrefWindow::on_meth6_activate()
{  
    PrayMethod=5;
	enable_or_disable_ent();
}

void PrefWindow::enable_or_disable_ent() {

	switch (PrayMethod) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		IshaOption->set_sensitive(false);
		FajrOption->set_sensitive(false);
		m_SpinButton3.set_sensitive(false);
		m_SpinButton4.set_sensitive(false);
			break;
		case 5:
		IshaOption->set_sensitive(true);
		FajrOption->set_sensitive(true);
		m_SpinButton3.set_sensitive(true);
		m_SpinButton4.set_sensitive(true);
		FajrAng =  m_SpinButton3.get_value();
		IshaAng = m_SpinButton4.get_value();

		if (FajrOption->get_history()==0) FajrINV =0; 
	else if (FajrOption->get_history()==1) FajrINV=90; 
	else if (FajrOption->get_history()==2) FajrINV=120;


		if (IshaOption->get_history()==0) IshaINV =0; 
	else if (IshaOption->get_history()==1) IshaINV=90; 
	else if (IshaOption->get_history()==2) IshaINV=120;
			break;

	}

}

