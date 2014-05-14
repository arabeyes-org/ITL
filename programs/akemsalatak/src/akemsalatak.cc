/******************************************************************
 * Program  : Akim Salatk
 * Desc       : a GUI program to display the Islamic Prayer and Hijry Date
 * File         : This file is the main file in the program. 
 *
 * Author     :  Ahmad AlTwaijry ( msx @ hush.com ) 053900357
 * CopyRight :  GPL
 *****************************************************************/

#define version "0.0.1"
#include <gtkmm.h>
#include "mainwindow.cc"

#if defined __GNUC__ && __GNUC__ < 3
#error This program will crash if compiled with g++ 2.x
// see the dynamic_cast bug in the gtkmm FAQ
#endif //
#include "config.h"
/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (GETTEXT_PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

int main(int argc, char *argv[])
{
#if defined(ENABLE_NLS)
   bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
   textdomain (GETTEXT_PACKAGE);
#endif //ENABLE_NLS

	Gtk::Main kit(argc, argv);
		MainWindow window;
	Gtk::Main::run(window);

    
    return 0;
}
