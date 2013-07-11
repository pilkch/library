// Standard headers
#include <iostream>

// libgtkmm headers
#include <libgtkmm/alertdialog.h>

namespace gtkmm
{
  cGtkmmAlertDialog::cGtkmmAlertDialog(Gtk::Window& _parent) :
    parent(_parent),
    sOk(TEXT("Ok")),
    bHasCancel(false),
    bCheckBoxValue(false),
    pCheckBox(nullptr)
  {
  }

  void cGtkmmAlertDialog::SetTitle(const string_t& sText)
  {
    sTitle = sText;
  }

  void cGtkmmAlertDialog::SetDescription(const string_t& sText)
  {
    sDescription = sText;
  }

  void cGtkmmAlertDialog::SetOk(const string_t& sText)
  {
    sOk = sText;
  }

  void cGtkmmAlertDialog::SetOther(const string_t& sText)
  {
    sOther = sText;
  }

  void cGtkmmAlertDialog::SetCancel()
  {
    bHasCancel = true;
  }

  void cGtkmmAlertDialog::SetCheckBox(const string_t& sText, bool bIsChecked)
  {
    sCheckBox = sText;
    bCheckBoxValue = bIsChecked;
  }

  bool cGtkmmAlertDialog::IsCheckBoxTicked() const
  {
    return bCheckBoxValue;
  }

  void cGtkmmAlertDialog::OnCheckBoxToggle()
  {
    bCheckBoxValue = pCheckBox->get_active();
  }

  ALERT_RESULT cGtkmmAlertDialog::Run()
  {
    const bool bUseMarkup = false;
    const bool bIsModal = true;
    Gtk::MessageDialog dialog(parent, spitfire::string::ToUTF8(sTitle).c_str(), bUseMarkup, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE, bIsModal);
    dialog.set_secondary_text(spitfire::string::ToUTF8(sDescription).c_str());

    // Add a cancel button
    if (bHasCancel) dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

    // Add the other button
    if (!sOther.empty()) dialog.add_button(spitfire::string::ToUTF8(sOther).c_str(), 1001);

    // Add the ok button
    dialog.add_button(spitfire::string::ToUTF8(sOk).c_str(), Gtk::RESPONSE_OK);

    if (!sCheckBox.empty()) {
      Gtk::Box* vbox = dialog.get_vbox();

      // This is some filler text, needs to change before release
      pCheckBox = new Gtk::CheckButton(spitfire::string::ToUTF8(sCheckBox).c_str());
      vbox->pack_end(*pCheckBox, true, true);
      pCheckBox->show();
      pCheckBox->set_active(bCheckBoxValue);

      pCheckBox->signal_toggled().connect(sigc::mem_fun(this, &cGtkmmAlertDialog::OnCheckBoxToggle));
    }

    dialog.set_resizable(true);

    ALERT_RESULT result = ALERT_RESULT::CANCEL;

    const int iResult = dialog.run();
    switch (iResult) {
      case Gtk::RESPONSE_OK: {
        result = ALERT_RESULT::OK;
        break;
      }
      case Gtk::RESPONSE_CANCEL: {
        result = ALERT_RESULT::CANCEL;
        break;
      }
      default: {
        result = ALERT_RESULT::NO;
        break;
      }
    }

    return result;
  }
}
