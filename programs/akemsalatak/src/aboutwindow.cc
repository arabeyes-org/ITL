#include <iostream>

#include "aboutwindow.h"
#include "gettextstr.h"
#include <gtkmm/stock.h>
#include "logo_data.cc"

AboutWindow::AboutWindow()
{
	Glib::ustring TempStr;

  set_title(_("About"));
  set_modal(true);
  property_window_position().set_value(Gtk::WIN_POS_CENTER);
  set_resizable(false);
  property_destroy_with_parent().set_value(true);
  set_border_width(5);

   Glib::RefPtr<Gdk::PixbufLoader> _imgLogo_loader=Gdk::PixbufLoader::create();
   _imgLogo_loader->write(logo_data, sizeof logo_data);
   _imgLogo_loader->close();
   imgLogo = Gtk::manage(new class Gtk::Image(_imgLogo_loader->get_pixbuf()));

	TempStr.assign(_("<span  foreground=\"blue\" size=\"20000\">Akem Salatk "));
	TempStr.append(version);
	TempStr.append("</span>");
	labTitle.set_text(TempStr);

	labDisc.set_text(_("Akem Salatk is a Muslim GUI Prayer time in Unix/Linux"));
	TempStr.assign(_("Akem Salatk "));
	TempStr.append(version);
	TempStr.append(_(" Copyright (c) 2003 Arabeyes, Ahmad Twaijry\n\nMany thanks for Thamer Mahmoud for the prayer time code\n and another thanks for Fayez Al-Hargan for the Hijry code"));
	labCopy.set_text(TempStr);
	labURL.set_text(_("http://www.arabeyes.org/project.php?proj=itl"));

	  labTitle.set_use_markup(TRUE);
	  labTitle.set_selectable(TRUE);
	  labTitle.set_justify(Gtk::JUSTIFY_CENTER);
	  labDisc.set_selectable(TRUE);
	  labDisc.set_justify(Gtk::JUSTIFY_CENTER);
	  labCopy.set_selectable(TRUE);
	  labCopy.set_justify(Gtk::JUSTIFY_CENTER);
	  labURL.set_selectable(TRUE);
	  labURL.set_justify(Gtk::JUSTIFY_CENTER);


   Gtk::Button *button2 = Gtk::manage(new class Gtk::Button(Gtk::StockID("gtk-close")));

	button2->set_size_request(76,27);
	button2->set_flags(Gtk::CAN_FOCUS);
	button2->set_relief(Gtk::RELIEF_NORMAL);
	button2->set_size_request(76,27);
	button2->set_flags(Gtk::CAN_FOCUS);
	button2->set_relief(Gtk::RELIEF_NORMAL);


	   winHBox.pack_start(*button2, Gtk::PACK_EXPAND_PADDING, 0);

	winVBox2.pack_start(*imgLogo,  Gtk::PACK_EXPAND_PADDING,0);
  add(winVBox);
   winVBox.pack_start(winVBox2);
   winVBox.pack_start(labTitle, Gtk::PACK_EXPAND_PADDING);
   winVBox.pack_start(labDisc, Gtk::PACK_EXPAND_PADDING);
   winVBox.pack_start(labCopy, Gtk::PACK_EXPAND_PADDING);
   winVBox.pack_start(labURL, Gtk::PACK_EXPAND_PADDING);
   winVBox.pack_start(winHBox, Gtk::PACK_EXPAND_PADDING);
   winVBox.set_spacing(10);
   button2->signal_clicked().connect(SigC::slot(*this, &Gtk::Widget::hide));

  realize();
  show_all_children();
}

AboutWindow::~AboutWindow()
{
}

