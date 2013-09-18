// libgtkmm headers
#include <libgtkmm/progressdialog.h>

namespace gtkmm
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

  cProgressDialogEvent::cProgressDialogEvent(cProgressDialog& _dialog) :
    dialog(_dialog)
  {
  }


  class cProgressDialogEventSetCancellable : public cProgressDialogEvent
  {
  public:
    cProgressDialogEventSetCancellable(cProgressDialog& dialog, bool bCancellable);

  private:
    virtual void RunEvent() override;

    bool bCancellable;
  };

  class cProgressDialogEventSetTextTitle : public cProgressDialogEvent
  {
  public:
    cProgressDialogEventSetTextTitle(cProgressDialog& dialog, const string_t sTitle);

  private:
    virtual void RunEvent() override;

    string_t sTitle;
  };

  class cProgressDialogEventSetTextPrimary : public cProgressDialogEvent
  {
  public:
    cProgressDialogEventSetTextPrimary(cProgressDialog& dialog, const string_t sPrimary);

  private:
    virtual void RunEvent() override;

    string_t sPrimary;
  };

  class cProgressDialogEventSetTextSecondary : public cProgressDialogEvent
  {
  public:
    cProgressDialogEventSetTextSecondary(cProgressDialog& dialog, const string_t sSecondary);

  private:
    virtual void RunEvent() override;

    string_t sSecondary;
  };

  class cProgressDialogEventSetPercentageCompletePrimary0To100 : public cProgressDialogEvent
  {
  public:
    cProgressDialogEventSetPercentageCompletePrimary0To100(cProgressDialog& dialog, float fPercentageComplete0To100);

  private:
    virtual void RunEvent() override;

    float fPercentageComplete0To100;
  };

  class cProgressDialogEventSetPercentageCompleteSecondary0To100 : public cProgressDialogEvent
  {
  public:
    cProgressDialogEventSetPercentageCompleteSecondary0To100(cProgressDialog& dialog, float fPercentageComplete0To100);

  private:
    virtual void RunEvent() override;

    float fPercentageComplete0To100;
  };


  class cProgressDialogEventSetPercentageCompletePrimaryIndeterminate : public cProgressDialogEvent
  {
  public:
    cProgressDialogEventSetPercentageCompletePrimaryIndeterminate(cProgressDialog& dialog);

  private:
    virtual void RunEvent() override;
  };

  class cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate : public cProgressDialogEvent
  {
  public:
    cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate(cProgressDialog& dialog);

  private:
    virtual void RunEvent() override;
  };

  class cProgressDialogEventBackgroundThreadFinished : public cProgressDialogEvent
  {
  public:
    cProgressDialogEventBackgroundThreadFinished(cProgressDialog& dialog, spitfire::util::PROCESS_RESULT result);

  private:
    virtual void RunEvent() override;

    spitfire::util::PROCESS_RESULT result;
  };


  cProgressDialogEventSetCancellable::cProgressDialogEventSetCancellable(cProgressDialog& dialog, bool _bCancellable) :
    cProgressDialogEvent(dialog),
    bCancellable(_bCancellable)
  {
  }

  void cProgressDialogEventSetCancellable::RunEvent()
  {
    dialog.SetCancellable(bCancellable);
  }


  cProgressDialogEventSetTextTitle::cProgressDialogEventSetTextTitle(cProgressDialog& dialog, const string_t _sTitle) :
    cProgressDialogEvent(dialog),
    sTitle(_sTitle)
  {
  }

  void cProgressDialogEventSetTextTitle::RunEvent()
  {
    dialog.SetTextTitle(sTitle);
  }


  cProgressDialogEventSetTextPrimary::cProgressDialogEventSetTextPrimary(cProgressDialog& dialog, const string_t _sPrimary) :
    cProgressDialogEvent(dialog),
    sPrimary(_sPrimary)
  {
  }

  void cProgressDialogEventSetTextPrimary::RunEvent()
  {
    dialog.SetTextPrimary(sPrimary);
  }


  cProgressDialogEventSetTextSecondary::cProgressDialogEventSetTextSecondary(cProgressDialog& dialog, const string_t _sSecondary) :
    cProgressDialogEvent(dialog),
    sSecondary(_sSecondary)
  {
  }

  void cProgressDialogEventSetTextSecondary::RunEvent()
  {
    dialog.SetTextSecondary(sSecondary);
  }


  cProgressDialogEventSetPercentageCompletePrimary0To100::cProgressDialogEventSetPercentageCompletePrimary0To100(cProgressDialog& dialog, float _fPercentageComplete0To100) :
    cProgressDialogEvent(dialog),
    fPercentageComplete0To100(_fPercentageComplete0To100)
  {
  }

  void cProgressDialogEventSetPercentageCompletePrimary0To100::RunEvent()
  {
    dialog.SetPercentageCompletePrimary0To100(fPercentageComplete0To100);
  }

  cProgressDialogEventSetPercentageCompleteSecondary0To100::cProgressDialogEventSetPercentageCompleteSecondary0To100(cProgressDialog& dialog, float _fPercentageComplete0To100) :
    cProgressDialogEvent(dialog),
    fPercentageComplete0To100(_fPercentageComplete0To100)
  {
  }

  void cProgressDialogEventSetPercentageCompleteSecondary0To100::RunEvent()
  {
    dialog.SetPercentageCompleteSecondary0To100(fPercentageComplete0To100);
  }

  cProgressDialogEventSetPercentageCompletePrimaryIndeterminate::cProgressDialogEventSetPercentageCompletePrimaryIndeterminate(cProgressDialog& dialog) :
    cProgressDialogEvent(dialog)
  {
  }

  void cProgressDialogEventSetPercentageCompletePrimaryIndeterminate::RunEvent()
  {
    dialog.SetPercentageCompletePrimaryIndeterminate();
  }

  cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate::cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate(cProgressDialog& dialog) :
    cProgressDialogEvent(dialog)
  {
  }

  void cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate::RunEvent()
  {
    dialog.SetPercentageCompleteSecondaryIndeterminate();
  }

  cProgressDialogEventBackgroundThreadFinished::cProgressDialogEventBackgroundThreadFinished(cProgressDialog& dialog, spitfire::util::PROCESS_RESULT _result) :
    cProgressDialogEvent(dialog),
    result(_result)
  {
  }

  void cProgressDialogEventBackgroundThreadFinished::RunEvent()
  {
    dialog.OnBackgroundThreadFinished(result);
  }


  // ** cProgressDialog

  cProgressDialog::cProgressDialog(Gtk::Window& parent) :
    Gtk::Dialog("ProgressDialog", parent, true),
    pProcess(nullptr),
    result(spitfire::util::PROCESS_RESULT::FAILED),
    pButtonCancel(nullptr),
    bCancelling(false)
  {
    set_border_width(5);

    set_resizable();

    Gtk::Box* pBox = get_vbox();

    pBox->set_border_width(10);

    pBox->pack_start(statusPrimary, Gtk::PACK_SHRINK);

    pBox->pack_start(progressBarPrimary, Gtk::PACK_SHRINK, 5);

    progressBarPrimary.set_text("0%");
    progressBarPrimary.set_show_text(true);

    pBox->pack_start(statusSecondary, Gtk::PACK_SHRINK);

    pBox->pack_start(progressBarSecondary, Gtk::PACK_SHRINK, 5);

    progressBarSecondary.set_text("0%");
    progressBarSecondary.set_show_text(true);
    progressBarSecondary.hide();

    pBox->pack_start(separator, Gtk::PACK_SHRINK);

    // Add a cancel button
    pButtonCancel = add_button(Gtk::Stock::CANCEL, Gtk::ResponseType::RESPONSE_CANCEL);
    pButtonCancel->set_can_default();
    pButtonCancel->grab_default();
    set_default_response(Gtk::ResponseType::RESPONSE_CANCEL);

    signal_realize().connect(sigc::mem_fun(*this, &cProgressDialog::OnInit));
    signal_response().connect(sigc::mem_fun(*this, &cProgressDialog::OnResponse));

    notify.Create();

    show_all_children();
  }

  void cProgressDialog::OnInit()
  {
    ASSERT(pProcess != nullptr);
    processThread.RunProcess(*this, *pProcess);
  }

  bool cProgressDialog::_IsToStop() const
  {
    return processThread.IsToStop();
  }

  void cProgressDialog::_SetCancellable(bool bCancellable)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetCancellable(*this, bCancellable));
    } else pButtonCancel->set_sensitive(bCancellable);
  }

  void cProgressDialog::_SetTextTitle(const string_t& sText)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetTextTitle(*this, sText));
    } else set_title(sText);
  }

  void cProgressDialog::_SetTextPrimary(const string_t& sText)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetTextPrimary(*this, sText));
    } else statusPrimary.set_text(sText);
  }

  void cProgressDialog::_SetTextSecondary(const string_t& sText)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetTextSecondary(*this, sText));
    } else statusSecondary.set_text(sText);
  }

  void cProgressDialog::_SetPercentageCompletePrimary0To100(float_t fPercentageComplete0To100)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetPercentageCompletePrimary0To100(*this, fPercentageComplete0To100));
    } else {
      progressBarPrimary.show();
      progressBarPrimary.set_fraction(fPercentageComplete0To100 / 100.0f);
      const string_t sPercentage = spitfire::string::ToString(int(fPercentageComplete0To100));
      progressBarPrimary.set_text(sPercentage + "%");
    }
  }

  void cProgressDialog::_SetPercentageCompleteSecondary0To100(float_t fPercentageComplete0To100)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetPercentageCompleteSecondary0To100(*this, fPercentageComplete0To100));
    } else {
      progressBarSecondary.show();
      progressBarSecondary.set_fraction(fPercentageComplete0To100 / 100.0f);
      const string_t sPercentage = spitfire::string::ToString(int(fPercentageComplete0To100));
      progressBarSecondary.set_text(sPercentage + "%");
    }
  }

  void cProgressDialog::_SetPercentageCompletePrimaryIndeterminate()
  {

    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetPercentageCompletePrimaryIndeterminate(*this));
    } else {
      progressBarPrimary.show();
      progressBarPrimary.set_text("");
      progressBarPrimary.pulse();
    }
  }

  void cProgressDialog::_SetPercentageCompleteSecondaryIndeterminate()
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventSetPercentageCompleteSecondaryIndeterminate(*this));
    } else {
      progressBarSecondary.show();
      progressBarSecondary.set_text("");
      progressBarSecondary.pulse();
    }
  }

  void cProgressDialog::OnBackgroundThreadFinished(spitfire::util::PROCESS_RESULT _result)
  {
    if (!spitfire::util::IsMainThread()) {
      notify.PushEventToMainThread(new cProgressDialogEventBackgroundThreadFinished(*this, _result));
    } else {
      result = _result;

      if (!bCancelling) {
        bCancelling = true;
        response(Gtk::ResponseType::RESPONSE_OK); //Gtk::ResponseType::RESPONSE_ACCEPT delete me ****************************
      }
    }
  }

  void cProgressDialog::OnResponse(int iResponseID)
  {
    if (iResponseID == Gtk::ResponseType::RESPONSE_CANCEL) {
      LOG<<"cProgressDialog::OnResponse Stopping process"<<std::endl;
      bCancelling = true;

      // Stop the thread soon
      processThread.StopThreadSoon();

      ASSERT(pButtonCancel != nullptr);

      pButtonCancel->set_sensitive(false); // Disable the Cancel button because we are already cancelling

      // Wait for the process to stop
      processThread.StopThreadNow();
    }

    notify.ClearEventQueue();
  }

  spitfire::util::PROCESS_RESULT cProgressDialog::Run(spitfire::util::cProcess& process)
  {
    pProcess = &process;

    run();

    ASSERT(notify.IsEmpty());

    return result;
  }
}
