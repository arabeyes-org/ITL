#ifndef GTKMM_PREFWINDOW_H
#define GTKMM_PREFWINDOW_H

#include <gtkmm.h>
#include "gettextstr.h"
#include "cities.cc"

class PrefWindow : public Gtk::Window
{
public:
  PrefWindow();
  virtual ~PrefWindow();
//  virtual void on_combo_changed();
  virtual void PrefWindow::on_meth1_activate();
  virtual void PrefWindow::on_meth2_activate();
  virtual void PrefWindow::on_meth3_activate();
  virtual void PrefWindow::on_meth4_activate();
  virtual void PrefWindow::on_meth5_activate();
  virtual void PrefWindow::on_meth6_activate();
  virtual void PrefWindow::on_FajrINVType1_activate();
  virtual void PrefWindow::on_FajrINVType2_activate();
  virtual void PrefWindow::on_FajrINVType3_activate();
  virtual void PrefWindow::on_IshaINVType1_activate();
  virtual void PrefWindow::on_IshaINVType2_activate();
  virtual void PrefWindow::on_IshaINVType3_activate();
  virtual void PrefWindow::on_MathhabType1_activate();
  virtual void PrefWindow::on_MathhabType2_activate();
  virtual void PrefWindow::on_cities_changed();
  virtual void PrefWindow::on_radio_changed();
  virtual void PrefWindow::enable_or_disable_ent();
protected:
   Gtk::OptionMenu *FajrOption;
   Gtk::OptionMenu *IshaOption;
   Gtk::OptionMenu *MathhabOption;
   Gtk::OptionMenu *methodOption;
   Gtk::OptionMenu *CitiesOption;
   Gtk::OptionMenu *RadioOption;

   Gtk::Menu *RadioMenu;
   Gtk::Menu *methodMenu;
   Gtk::Menu *FajrMenu;
   Gtk::Menu *IshaMenu;
   Gtk::Menu *MathhabMenu;
   Gtk::Menu *CitiesMenu;

//  std::list<Glib::ustring> listStrings;
  Gtk::VBox m_VBox;
  Gtk::HBox m_HBox;
  Gtk::Notebook m_Notebook;
  Gtk::Label m_Label1, m_Label2,m_Label3, m_Label4,m_Label5, m_Label6,m_Label7,m_Label8;
  Gtk::Label m_Label9, m_Label10,m_Label11, m_Label12,m_Label13, m_Label14, m_Label15;
  Gtk::Label m_Label16, m_Label17, m_Label18;
  Gtk::Entry m_Entry1, m_Entry2,m_Entry3;
  Gtk::Entry m_Entry4, m_Entry5;
  Gtk::SpinButton m_SpinButton1,m_SpinButton2;
  Gtk::SpinButton m_SpinButton3,m_SpinButton4;
  Gtk::RadioButton m_RadioButton1,m_RadioButton2;
//  Gtk::Combo m_Combo1;
  Gtk::Adjustment m_adjustment1,m_adjustment2;
  Gtk::Adjustment m_adjustment3,m_adjustment4;
  Gtk::Tooltips m_Tips2,m_Tips3,m_Tips4,m_Tips5,m_Tips6,m_Tips7;
  Gtk::Table m_Table1,m_Table2,m_Table3;
  Gtk::HButtonBox m_ButtonBox;

};

#endif //GTKMM_PREFWINDOW_H
