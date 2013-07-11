#ifndef LIBGTKMM_ABOUT_H
#define LIBGTKMM_ABOUT_H

// gtkmm headers
#include <gtkmm.h>

// Spitfire headers
#include <spitfire/util/datetime.h>

namespace gtkmm
{
  // ** cGtkmmAboutDialog

  class cGtkmmAboutDialog
  {
  public:
    void Run(Gtk::Window& parent);
  };

  inline void cGtkmmAboutDialog::Run(Gtk::Window& parent)
  {
    Gtk::AboutDialog dialog;
    dialog.set_transient_for(parent);
    dialog.set_title("About");
    dialog.set_program_name(BUILD_APPLICATION_NAME);
    dialog.set_version(BUILD_APPLICATION_VERSION_STRING);
    spitfire::util::cDateTime dateTime;
    std::ostringstream o;
    o<<BUILD_APPLICATION_COPYRIGHT_OWNER<<", "<<dateTime.GetYear();
    dialog.set_copyright(o.str().c_str());
    //dialog.set_license("license text will be here...");
    dialog.set_website(BUILD_APPLICATION_WEBSITE);
    //dialog.set_translator_credits("translators");
    dialog.run();
  }
}

#endif // LIBGTKMM_ABOUT_H
