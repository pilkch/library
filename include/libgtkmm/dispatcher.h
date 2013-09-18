#ifndef LIBGTKMM_NOTIFYMAINTHREAD_H
#define LIBGTKMM_NOTIFYMAINTHREAD_H

// Gtkmm headers
#include <gtkmm.h>

// Spitfire headers
#include <spitfire/util/queue.h>

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


  // ** cGtkmmRunOnMainThread
  //
  // Sends events to the main thread

  template <class T>
  class cGtkmmRunOnMainThread
  {
  public:
    cGtkmmRunOnMainThread();
    ~cGtkmmRunOnMainThread();

    void Create();

    #ifdef BUILD_DEBUG
    bool IsEmpty() { return queue.IsEmpty(); }
    #endif

    void PushEventToMainThread(T* pEvent);

    void ClearEventQueue();

  private:
    void OnNotify();

    cGtkmmNotifyMainThread notifyMainThread;

    spitfire::util::cSignalObject soAction;

    spitfire::util::cThreadSafeQueue<T> queue;
  };

  template <class T>
  cGtkmmRunOnMainThread<T>::cGtkmmRunOnMainThread() :
    soAction("cGtkmmRunOnMainThread<T>::soAction"),
    queue(soAction)
  {
  }

  template <class T>
  cGtkmmRunOnMainThread<T>::~cGtkmmRunOnMainThread()
  {
    ASSERT(queue.IsEmpty());
  }

  template <class T>
  void cGtkmmRunOnMainThread<T>::Create()
  {
    notifyMainThread.Create(*this, &cGtkmmRunOnMainThread<T>::OnNotify);
  }

  template <class T>
  void cGtkmmRunOnMainThread<T>::OnNotify()
  {
    ASSERT(!queue.IsEmpty());

    T* pEvent = queue.RemoveItemFromFront();
    if (pEvent != nullptr) {
      pEvent->RunEvent();
      spitfire::SAFE_DELETE(pEvent);
    }
  }

  template <class T>
  void cGtkmmRunOnMainThread<T>::PushEventToMainThread(T* pEvent)
  {
    // Add the event to the queue
    queue.AddItemToBack(pEvent);

    // Notify the main thread
    notifyMainThread.Notify();
  }

  template <class T>
  void cGtkmmRunOnMainThread<T>::ClearEventQueue()
  {
    ASSERT(spitfire::util::IsMainThread());

    while (true) {
      T* pEvent = queue.RemoveItemFromFront();
      if (pEvent == nullptr) break;

      pEvent->RunEvent();
      spitfire::SAFE_DELETE(pEvent);
    }
  }
}

#endif // LIBGTKMM_NOTIFYMAINTHREAD_H
