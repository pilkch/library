#ifndef LIBGTKMM_NOTIFYMAINTHREAD_H
#define LIBGTKMM_NOTIFYMAINTHREAD_H

// Gtkmm headers
#include <gtkmm.h>

namespace gtkmm
{
  // ** cGtkmmNotifyMainThread
  //
  // Runs a function on the main thread

  class cGtkmmNotifyMainThread
  {
  public:
    template <class T>
    void Create(T& t, void (T::*function)());

    void Notify();

  private:
    Glib::Dispatcher signal;
  };

  // ** cGtkmmNotifyMainThread

  template <class T>
  void cGtkmmNotifyMainThread::Create(T& t, void (T::*function)())
  {
    // Register our event handler
    signal.connect(sigc::mem_fun(t, function));
  }

  inline void cGtkmmNotifyMainThread::Notify()
  {
    // Emit our signal
    signal();
  }
}

#endif // LIBGTKMM_NOTIFYMAINTHREAD_H
