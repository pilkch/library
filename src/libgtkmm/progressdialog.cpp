// libgtkmm headers
#include <libgtkmm/progressdialog.h>

namespace gtkmm
{
  cProgressDialog::cProgressDialog(Gtk::Window& parent) :
    Gtk::Dialog("ProgressDialog", parent, true),
    pProcess(nullptr),
    result(spitfire::util::PROCESS_RESULT::FAILED),
    m_Alignment(0.5, 0.5, 0, 0),
    pButtonCancel(nullptr)
  {
    set_border_width(5);

    set_resizable();

    Gtk::Box* pBox = get_vbox();

    pBox->set_border_width(10);

    pBox->pack_start(statusPrimary, Gtk::PACK_SHRINK);
    pBox->pack_start(statusSecondary, Gtk::PACK_SHRINK);

    pBox->pack_start(m_Alignment, Gtk::PACK_SHRINK, 5);

    m_Alignment.add(m_ProgressBar);
    m_ProgressBar.set_text("0%");
    m_ProgressBar.set_show_text(true);

    m_Alignment.add(progressBarSecondary);
    progressBarSecondary.set_text("0%");
    progressBarSecondary.set_show_text(true);
    progressBarSecondary.hide();

    pBox->pack_start(m_Separator, Gtk::PACK_SHRINK);

    // Add a cancel button
    pButtonCancel = add_button(Gtk::Stock::CANCEL, Gtk::ResponseType::RESPONSE_CANCEL);
    pButtonCancel->set_can_default();
    pButtonCancel->grab_default();
    set_default_response(Gtk::ResponseType::RESPONSE_CANCEL);

    signal_realize().connect(sigc::mem_fun(*this, &cProgressDialog::OnInit));
    //signal_response().connect(sigc::mem_fun(*this, &cProgressDialog::OnResponse));

    show_all_children();
  }

  void cProgressDialog::OnInit()
  {
    // TODO: Run the process on a second thread
    ASSERT(pProcess != nullptr);
    result = pProcess->Run();
  }

  bool cProgressDialog::_IsToStop() const
  {
    return false;
  }

  void cProgressDialog::_SetCancellable(bool bCancellable)
  {
    pButtonCancel->set_sensitive(bCancellable);
  }

  void cProgressDialog::_SetTextTitle(const string_t& sText)
  {
    set_title(sText);
  }

  void cProgressDialog::_SetTextPrimary(const string_t& sText)
  {
    statusPrimary.set_text(sText);
  }

  void cProgressDialog::_SetTextSecondary(const string_t& sText)
  {
    statusSecondary.set_text(sText);
  }

  void cProgressDialog::_SetPercentageCompletePrimary0To100(float_t fPercentageComplete0To100)
  {
    m_ProgressBar.set_fraction(fPercentageComplete0To100 / 100.0f);
    const string_t sPercentage = spitfire::string::ToString(int(fPercentageComplete0To100));
    m_ProgressBar.set_text(sPercentage + "%");
  }

  void cProgressDialog::_SetPercentageCompleteSecondary0To100(float_t fPercentageComplete0To100)
  {
    progressBarSecondary.show();
    progressBarSecondary.set_fraction(fPercentageComplete0To100 / 100.0f);
    const string_t sPercentage = spitfire::string::ToString(int(fPercentageComplete0To100));
    progressBarSecondary.set_text(sPercentage + "%");
  }

  void cProgressDialog::_SetPercentageCompletePrimaryIndeterminate()
  {
    m_ProgressBar.pulse();
  }

  void cProgressDialog::_SetPercentageCompleteSecondaryIndeterminate()
  {
    progressBarSecondary.pulse();
  }

  spitfire::util::PROCESS_RESULT cProgressDialog::Run(spitfire::util::cProcess& process)
  {
    pProcess = &process;

    run();
    return result;
  }
}
