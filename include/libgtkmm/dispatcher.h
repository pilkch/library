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

  template <class D, class T>
  class cGtkmmRunOnMainThread
  {
  public:
    explicit cGtkmmRunOnMainThread(D& data);
    ~cGtkmmRunOnMainThread();

    void Create();

    #ifdef BUILD_DEBUG
    bool IsEmpty() { return queue.IsEmpty(); }
    #endif

    void PushEventToMainThread(T* pEvent);

    void ClearEventQueue();

  private:
    void OnNotify();

    D& data;

    cGtkmmNotifyMainThread notifyMainThread;

    spitfire::util::cSignalObject soAction;

    spitfire::util::cThreadSafeQueue<T> queue;
  };

  template <class D, class T>
  cGtkmmRunOnMainThread<D, T>::cGtkmmRunOnMainThread(D& _data) :
    data(_data),
    soAction("cGtkmmRunOnMainThread<T>::soAction"),
    queue(soAction)
  {
  }

  template <class D, class T>
  cGtkmmRunOnMainThread<D, T>::~cGtkmmRunOnMainThread()
  {
    ASSERT(queue.IsEmpty());
  }

  template <class D, class T>
  void cGtkmmRunOnMainThread<D, T>::Create()
  {
    notifyMainThread.Create(*this, &cGtkmmRunOnMainThread<D, T>::OnNotify);
  }

  template <class D, class T>
  void cGtkmmRunOnMainThread<D, T>::OnNotify()
  {
    ASSERT(!queue.IsEmpty());

    T* pEvent = queue.RemoveItemFromFront();
    if (pEvent != nullptr) {
      pEvent->EventFunction(data);
      spitfire::SAFE_DELETE(pEvent);
    }
  }

  template <class D, class T>
  void cGtkmmRunOnMainThread<D, T>::PushEventToMainThread(T* pEvent)
  {
    // Add the event to the queue
    queue.AddItemToBack(pEvent);

    // Notify the main thread
    notifyMainThread.Notify();
  }

  template <class D, class T>
  void cGtkmmRunOnMainThread<D, T>::ClearEventQueue()
  {
    ASSERT(spitfire::util::IsMainThread());

    while (true) {
      T* pEvent = queue.RemoveItemFromFront();
      if (pEvent == nullptr) break;

      pEvent->EventFunction(data);
      spitfire::SAFE_DELETE(pEvent);
    }
  }
}

#endif // LIBGTKMM_NOTIFYMAINTHREAD_H
