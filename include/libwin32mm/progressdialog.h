#ifndef LIBWIN32MM_PROGRESSDIALOG_H
#define LIBWIN32MM_PROGRESSDIALOG_H

// libwin32mm headers
#include <libwin32mm/libwin32mm.h>
#include <libwin32mm/notify.h>
#include <libwin32mm/taskbar.h>
#include <libwin32mm/taskdialog.h>

// Spitfire headers
#include <spitfire/util/process.h>
#include <spitfire/util/thread.h>

namespace win32mm
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
    virtual ~cProgressDialogEvent() {}

    virtual void EventFunction(cProgressDialog& dialog) = 0;
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

  class cProgressDialog : public spitfire::util::cProcessInterface, private cRunProcessOnBackgroundThreadListener, private cTaskDialogListener
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

    cProgressDialog(cWindow& window, cTaskBar& taskBar);
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

    virtual void OnBackgroundThreadFinished(spitfire::util::PROCESS_RESULT result) override;

    virtual void OnCreate() override;  // Called when the task dialog is created
    virtual void OnDestroy() override; // Called when the task dialog is destroyed

    void Close();

    cWindow& window;
    cTaskBar& taskBar;

    cTaskDialog taskDialog;

    cRunProcessOnBackgroundThread processThread;

    spitfire::util::cProcess* pProcess;

    spitfire::util::PROCESS_RESULT result;

    float fProgress0To1;
    bool bCancelling;

    cRunOnMainThread<cProgressDialog, cProgressDialogEvent> notify;
  };
}

#endif // !LIBWIN32MM_PROGRESSDIALOG_H
