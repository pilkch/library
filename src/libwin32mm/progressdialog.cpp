// libwin32mm headers
#include <libwin32mm/progressdialog.h>
#include <libwin32mm/taskdialog.h>

namespace win32mm
{
  // ** cRunProcessOnBackgroundThread

  cRunProcessOnBackgroundThread::cRunProcessOnBackgroundThread() :
    spitfire::util::cThread(soAction, TEXT("cRunProcessOnBackgroundThread::cThread")),
    soAction(TEXT("cRunProcessOnBackgroundThread::soAction")),
    pListener(nullptr),
    pProcess(nullptr)
  {
  }

  void cRunProcessOnBackgroundThread::RunProcess(cRunProcessOnBackgroundThreadListener& listener, spitfire::util::cProcess& process)
  {
    pListener = &listener;
    pProcess = &process;

    Run();
  }

  void cRunProcessOnBackgroundThread::ThreadFunction()
  {
    ASSERT(pListener != nullptr);
    ASSERT(pProcess != nullptr);

    // Run the process
    const spitfire::util::PROCESS_RESULT result = pProcess->Run();

    // Notify the listener
    pListener->OnBackgroundThreadFinished(result);
  }


  // ** cProgressDialogEvent

  class cProgressDialogEventSetCancellable : public cProgressDialogEvent
  {
  public:
    explicit cProgressDialogEventSetCancellable(bool bCancellable);

  private:
    virtual void EventFunction(cProgressDialog& dialog) override;

    bool bCancellable;
  };

  class cProgressDialogEventSetTextTitle : public cProgressDialogEvent
  {
  public:
    explicit cProgressDialogEventSetTextTitle(const string_t sTitle);

  private:
    virtual void EventFunction(cProgressDialog& dialog) override;

    string_t sTitle;
  };

  class cProgressDialogEventSetTextPrimary : public cProgressDialogEvent
  {
  public:
    explicit cProgressDialogEventSetTextPrimary(const string_t sPrimary);

  private:
    virtual void EventFunction(cProgressDialog& dialog) override;

    string_t sPrimary;
  };

  class cProgressDialogEventSetTextSecondary : public cProgressDialogEvent
  {
  public:
    explicit cProgressDialogEventSetTextSecondary(const string_t sSecondary);

  private:
    virtual void EventFunction(cProgressDialog& dialog) override;

    string_t sSecondary;
  };

  class cProgressDialogEventSetPercentageCompletePrimary0To100 : public cProgressDialogEvent
  {
  public:
    explicit cProgressDialogEventSetPercentageCompletePrimary0To100(float fPercentageComplete0To100);

  private:
    virtual void EventFunction(cProgressDialog& dialog) override;

    float fPercentageComplete0To100;
  };

  class cProgressDialogEventSetPercentageCompleteSecondary0To100 : public cProgressDialogEvent
  {
  public:
    explicit cProgressDialogEventSetPercentageCompleteSecondary0To100(float fPercentageComplete0To100);

  private:
    virtual void EventFunction(cProgressDialog& dialog) override;

    float fPercentageComplete0To100;
  };


  class cProgressDialogEventSetPercentageCompletePrimaryIndeterminate : public cProgressDialogEvent
  {
  public:

  private:
    virtual void EventFunction(cProgressDialog& dialog) override;
  };

  class cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate : public cProgressDialogEvent
  {
  public:

  private:
    virtual void EventFunction(cProgressDialog& dialog) override;
  };

  class cProgressDialogEventBackgroundThreadFinished : public cProgressDialogEvent
  {
  public:
    explicit cProgressDialogEventBackgroundThreadFinished(spitfire::util::PROCESS_RESULT result);

  private:
    virtual void EventFunction(cProgressDialog& dialog) override;

    spitfire::util::PROCESS_RESULT result;
  };


  cProgressDialogEventSetCancellable::cProgressDialogEventSetCancellable(bool _bCancellable) :
    bCancellable(_bCancellable)
  {
  }

  void cProgressDialogEventSetCancellable::EventFunction(cProgressDialog& dialog)
  {
    dialog.SetCancellable(bCancellable);
  }


  cProgressDialogEventSetTextTitle::cProgressDialogEventSetTextTitle(const string_t _sTitle) :
    sTitle(_sTitle)
  {
  }

  void cProgressDialogEventSetTextTitle::EventFunction(cProgressDialog& dialog)
  {
    dialog.SetTextTitle(sTitle);
  }


  cProgressDialogEventSetTextPrimary::cProgressDialogEventSetTextPrimary(const string_t _sPrimary) :
    sPrimary(_sPrimary)
  {
  }

  void cProgressDialogEventSetTextPrimary::EventFunction(cProgressDialog& dialog)
  {
    dialog.SetTextPrimary(sPrimary);
  }


  cProgressDialogEventSetTextSecondary::cProgressDialogEventSetTextSecondary(const string_t _sSecondary) :
    sSecondary(_sSecondary)
  {
  }

  void cProgressDialogEventSetTextSecondary::EventFunction(cProgressDialog& dialog)
  {
    dialog.SetTextSecondary(sSecondary);
  }


  cProgressDialogEventSetPercentageCompletePrimary0To100::cProgressDialogEventSetPercentageCompletePrimary0To100(float _fPercentageComplete0To100) :
    fPercentageComplete0To100(_fPercentageComplete0To100)
  {
  }

  void cProgressDialogEventSetPercentageCompletePrimary0To100::EventFunction(cProgressDialog& dialog)
  {
    dialog.SetPercentageCompletePrimary0To100(fPercentageComplete0To100);
  }

  cProgressDialogEventSetPercentageCompleteSecondary0To100::cProgressDialogEventSetPercentageCompleteSecondary0To100(float _fPercentageComplete0To100) :
    fPercentageComplete0To100(_fPercentageComplete0To100)
  {
  }

  void cProgressDialogEventSetPercentageCompleteSecondary0To100::EventFunction(cProgressDialog& dialog)
  {
    dialog.SetPercentageCompleteSecondary0To100(fPercentageComplete0To100);
  }

  void cProgressDialogEventSetPercentageCompletePrimaryIndeterminate::EventFunction(cProgressDialog& dialog)
  {
    dialog.SetPercentageCompletePrimaryIndeterminate();
  }

  void cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate::EventFunction(cProgressDialog& dialog)
  {
    dialog.SetPercentageCompleteSecondaryIndeterminate();
  }

  cProgressDialogEventBackgroundThreadFinished::cProgressDialogEventBackgroundThreadFinished(spitfire::util::PROCESS_RESULT _result) :
    result(_result)
  {
  }

  void cProgressDialogEventBackgroundThreadFinished::EventFunction(cProgressDialog& dialog)
  {
    dialog.OnBackgroundThreadFinished(result);
  }


  // ** cProgressDialog

  cProgressDialog::cProgressDialog(cWindow& _window, cTaskBar& _taskBar) :
    window(_window),
    taskBar(_taskBar),
    pProcess(nullptr),
    result(spitfire::util::PROCESS_RESULT::FAILED),
    notify(*this),
    bCancelling(false)
  {
  }

  void cProgressDialog::OnCreate()
  {
    ASSERT(pProcess != nullptr);
    processThread.RunProcess(*this, *pProcess);
  }

  void cProgressDialog::OnDestroy()
  {
    Close();
  }

  bool cProgressDialog::_IsToStop() const
  {
    return processThread.IsToStop();
  }

  void cProgressDialog::_SetCancellable(bool bCancellable)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetCancellable(bCancellable));
    } else taskDialog.SetCancellable(bCancellable);
  }

  void cProgressDialog::_SetTextTitle(const string_t& sText)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetTextTitle(sText));
    } else taskDialog.SetCaption(sText);
  }

  void cProgressDialog::_SetTextPrimary(const string_t& sText)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetTextPrimary(sText));
    } else taskDialog.SetInstructions(sText);
  }

  void cProgressDialog::_SetTextSecondary(const string_t& sText)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetTextSecondary(sText));
    } else taskDialog.SetContent(sText);
  }

  void cProgressDialog::_SetPercentageCompletePrimary0To100(float_t fPercentageComplete0To100)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetPercentageCompletePrimary0To100(fPercentageComplete0To100));
    } else {
      taskDialog.SetProgress0To100(fPercentageComplete0To100);

      taskBar.SetProgress0To100(fPercentageComplete0To100);
    }
  }

  void cProgressDialog::_SetPercentageCompleteSecondary0To100(float_t fPercentageComplete0To100)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetPercentageCompleteSecondary0To100(fPercentageComplete0To100));
    } else {
      //progressBarSecondary.show();
      //progressBarSecondary.set_fraction(fPercentageComplete0To100 / 100.0f);
    }
  }

  void cProgressDialog::_SetPercentageCompletePrimaryIndeterminate()
  {

    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetPercentageCompletePrimaryIndeterminate);
    } else {
      taskDialog.SetProgressIndeterminateOn();

      taskBar.SetProgressIndeterminateOn();
    }
  }

  void cProgressDialog::_SetPercentageCompleteSecondaryIndeterminate()
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate);
    } else {
      //progressBarSecondary.show();
      //progressBarSecondary.set_text("");
      //progressBarSecondary.pulse();
    }
  }

  void cProgressDialog::OnBackgroundThreadFinished(spitfire::util::PROCESS_RESULT _result)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventBackgroundThreadFinished(_result));
    } else {
      result = _result;

      if (!bCancelling) {
        bCancelling = true;

        Close();
      }
    }
  }

  void cProgressDialog::Close()
  {
    // Stop the thread soon
    processThread.StopThreadSoon();

    if (bCancelling) {
      // Close the task dialog
      taskDialog.Close();
    }

    // Wait for the process to stop
    processThread.StopThreadNow();

    // Clear the event queue
    notify.ClearEventQueue();
  }

  spitfire::util::PROCESS_RESULT cProgressDialog::Run(spitfire::util::cProcess& process)
  {
    pProcess = &process;

    notify.Create();

    cTaskDialogProgressDialogSettings progressDialogSettings;

    progressDialogSettings.sCaption = TEXT("Caption");
    progressDialogSettings.sInstructions = TEXT("Instructions");
    progressDialogSettings.sContent = TEXT("Content");

    cTaskDialogSettings settings;
    settings.SetProgressDialogSettings(progressDialogSettings);

    taskDialog.SetListener(*this);

    taskDialog.Run(window, settings);

    ASSERT(notify.IsEmpty());

    notify.Destroy();

    taskBar.SetProgressOff();

    return result;
  }
}
