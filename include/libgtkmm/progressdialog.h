#ifndef GTKMM_PROGRESSDIALOG_H
#define GTKMM_PROGRESSDIALOG_H

// Gtkmm headers
#include <gtkmm.h>

// libgtkmm headers
#include <libgtkmm/libgtkmm.h>
#include <libgtkmm/dispatcher.h>

// Spitfire headers
#include <spitfire/util/process.h>
#include <spitfire/util/thread.h>

namespace gtkmm
{
  // ** cRunProcessOnBackgroundThreadListener

  class cRunProcessOnBackgroundThreadListener
  {
  public:
    virtual ~cRunProcessOnBackgroundThreadListener() {}

    virtual void OnBackgroundThreadFinished(spitfire::util::PROCESS_RESULT result) = 0;
  };


  // ** cRunProcessOnBackgroundThread

  class cRunProcessOnBackgroundThread : public spitfire::util::cThread
  {
  public:
    cRunProcessOnBackgroundThread();

    void RunProcess(cRunProcessOnBackgroundThreadListener& listener, spitfire::util::cProcess& process);

    private:
      virtual void ThreadFunction() override;

      spitfire::util::cSignalObject soAction;

      cRunProcessOnBackgroundThreadListener* pListener;

      spitfire::util::cProcess* pProcess;
  };


  class cProgressDialog;

  // ** cProgressDialogEvent

  class cProgressDialogEvent
  {
  public:
    cProgressDialogEvent(cProgressDialog& dialog);
    virtual ~cProgressDialogEvent() {}

    virtual void RunEvent() = 0;

  protected:
    cProgressDialog& dialog;
  };


  class cProgressDialogEventSetCancellable;

  class cProgressDialogEventSetTextTitle;
  class cProgressDialogEventSetTextPrimary;
  class cProgressDialogEventSetTextSecondary;

  class cProgressDialogEventSetPercentageCompletePrimary0To100;
  class cProgressDialogEventSetPercentageCompleteSecondary0To100;
  class cProgressDialogEventSetPercentageCompletePrimaryIndeterminate;
  class cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate;

  class cProgressDialogEventBackgroundThreadFinished;


  // ** cProgressDialog

  class cProgressDialog : public Gtk::Dialog, public spitfire::util::cProcessInterface, private cRunProcessOnBackgroundThreadListener
  {
  public:
    friend class cProgressDialogEventSetCancellable;

    friend class cProgressDialogEventSetTextTitle;
    friend class cProgressDialogEventSetTextPrimary;
    friend class cProgressDialogEventSetTextSecondary;

    friend class cProgressDialogEventSetPercentageCompletePrimary0To100;
    friend class cProgressDialogEventSetPercentageCompleteSecondary0To100;
    friend class cProgressDialogEventSetPercentageCompletePrimaryIndeterminate;
    friend class cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate;

    friend class cProgressDialogEventBackgroundThreadFinished;

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
    void OnResponse(int iResponseID);

    virtual void OnBackgroundThreadFinished(spitfire::util::PROCESS_RESULT result) override;

    cRunProcessOnBackgroundThread processThread;

    spitfire::util::cProcess* pProcess;

    spitfire::util::PROCESS_RESULT result;

    Gtk::Label statusPrimary;
    Gtk::Label statusSecondary;
    Gtk::ProgressBar progressBarPrimary;
    Gtk::ProgressBar progressBarSecondary;
    Gtk::Separator separator;
    Gtk::Button* pButtonCancel;

    gtkmm::cGtkmmRunOnMainThread<cProgressDialogEvent> notify;

    bool bCancelling;
  };
}

#endif // !GTKMM_PROGRESSDIALOG_H
