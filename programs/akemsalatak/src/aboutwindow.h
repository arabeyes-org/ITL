#ifndef GTKMM_ABOUTWINDOW_H
#define GTKMM_ABOUTWINDOW_H

#include <gtkmm.h>
#include "gettextstr.h"


class AboutWindow : public Gtk::Window
{
public:
  AboutWindow();
  virtual ~AboutWindow();

protected:
	Gtk::VBox winVBox,winVBox2;
	Gtk::HBox winHBox,hbox1;
	Gtk::Alignment alignment1;
	Gtk::Label labTitle,labDisc,labCopy,labURL,label1;
	Gtk::Image *imgLogo;

};

#endif //GTKMM_ABOUTWINDOW_H
