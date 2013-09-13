#ifndef GTKMM_PROGRESSDIALOG_H
#define GTKMM_PROGRESSDIALOG_H

// Gtkmm headers
#include <gtkmm.h>

// libgtkmm headers
#include <libgtkmm/libgtkmm.h>

// Spitfire headers
#include <spitfire/util/process.h>

namespace gtkmm
{
  class cProgressDialog : public Gtk::Dialog, public spitfire::util::cProcessInterface
  {
  public:
    explicit cProgressDialog(Gtk::Window& parent);
    virtual ~cProgressDialog() {}

    spitfire::util::PROCESS_RESULT Run(spitfire::util::cProcess& process);

  private:
    virtual bool _IsToStop() const override;

    virtual void _SetCancellable(bool bCancellable) override;

    virtual void _SetTextTitle(const string_t& sText) override;
    virtual void _SetTextPrimary(const string_t& sText) override;
    virtual void _SetTextSecondary(const string_t& sText) override;

    virtual void _SetPercentageCompletePrimary0To100(float_t fPercentageComplete0To100) override;
    virtual void _SetPercentageCompleteSecondary0To100(float_t fPercentageComplete0To100) override;
    virtual void _SetPercentageCompletePrimaryIndeterminate() override;
    virtual void _SetPercentageCompleteSecondaryIndeterminate() override;

    void OnInit();

    spitfire::util::cProcess* pProcess;

    spitfire::util::PROCESS_RESULT result;

    Gtk::Alignment m_Alignment;
    Gtk::Label statusPrimary;
    Gtk::Label statusSecondary;
    Gtk::ProgressBar m_ProgressBar;
    Gtk::ProgressBar progressBarSecondary;
    Gtk::Separator m_Separator;
    Gtk::Button* pButtonCancel;
  };
}

#endif // !GTKMM_PROGRESSDIALOG_H
