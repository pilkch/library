#ifndef GTKMM_ALERTDIALOG_H
#define GTKMM_ALERTDIALOG_H

// Gtkmm headers
#include <gtkmm.h>

// libgtkmm headers
#include <libgtkmm/libgtkmm.h>

namespace gtkmm
{
  enum class ALERT_RESULT {
    OK,
    NO,
    CANCEL
  };

  class cGtkmmAlertDialog
  {
  public:
    explicit cGtkmmAlertDialog(Gtk::Window& parent);

    void SetTitle(const string_t& sText);
    void SetDescription(const string_t& sText);
    void SetOk(const string_t& sText);
    void SetOther(const string_t& sText);
    void SetCancel();
    void SetCheckBox(const string_t& sText, bool bIsChecked);
    bool IsCheckBoxTicked() const;

    ALERT_RESULT Run();

  protected:
    void OnCheckBoxToggle();

    Gtk::Window& parent;

    string_t sTitle;
    string_t sDescription;
    string_t sOk;
    string_t sOther;
    string_t sCheckBox;
    bool bHasCancel;
    bool bCheckBoxValue;

    Gtk::CheckButton* pCheckBox;
  };
}

#endif // !GTKMM_ALERTDIALOG_H
