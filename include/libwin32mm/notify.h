#ifndef LIBWIN32MM_NOTIFYMAINTHREAD_H
#define LIBWIN32MM_NOTIFYMAINTHREAD_H

// Windows headers
#include <windows.h>

// Spitfire headers
#include <spitfire/util/queue.h>

// libwin32mm headers
#include <libwin32mm/window.h>

#define DIESEL_WM_USER_NOTIFY (WM_USER + 472)

namespace win32mm
{
  // ** cNotifyMainThread
  //
  // Runs a function on the main thread
  // NOTE: Generally cRunOnMainThread below should be used instead as it can process events on a queue

  class cNotifyMainThread
  {
  public:
    cNotifyMainThread();

    void Create(cWindow& window);

    template <class T>
    inline LRESULT CALLBACK ProcessEvents(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, T& t, void (T::*function)());

    void Notify();
    
  private:
    cWindow* pWindow;
  };
  
  cNotifyMainThread::cNotifyMainThread() :
    pWindow(nullptr)
  {
  }
  
  void cNotifyMainThread::Create(cWindow& window)
  {
    pWindow = &window;
  }

  template <class T>
  inline LRESULT CALLBACK cNotifyMainThread::ProcessEvents(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, T& t, void (T::*function)())
  {
    ASSERT(uMsg >= WM_USER);

    // Check if we should handle this message
    if (uMsg == DIESEL_WM_USER_NOTIFY) {
      ((t).*(function))();
      return 0;
    }

    return ::DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  inline void cNotifyMainThread::Notify()
  {
    ASSERT(pWindow != nullptr);

    // Notify the main thread
    ::PostMessage(pWindow->GetWindowHandle(), DIESEL_WM_USER_NOTIFY, 0, 0);
  }


  // ** cRunOnMainThread
  //
  // Sends events to the main thread

  template <class D, class T>
  class cRunOnMainThread
  {
  public:
    explicit cRunOnMainThread(D& data);
    ~cRunOnMainThread();

    void Create(cWindow& window);

    LRESULT CALLBACK ProcessEvents(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    #ifdef BUILD_DEBUG
    bool IsEmpty() { return queue.IsEmpty(); }
    #endif

    void PushEventToMainThread(T* pEvent);

    void ClearEventQueue();

  private:
    void OnNotify();

    D& data;

    cNotifyMainThread notifyMainThread;

    spitfire::util::cSignalObject soAction;

    spitfire::util::cThreadSafeQueue<T> queue;
  };

  template <class D, class T>
  cRunOnMainThread<D, T>::cRunOnMainThread(D& _data) :
    data(_data),
    soAction("cRunOnMainThread<T>::soAction"),
    queue(soAction)
  {
  }

  template <class D, class T>
  cRunOnMainThread<D, T>::~cRunOnMainThread()
  {
    ASSERT(queue.IsEmpty());
  }

  template <class D, class T>
  void cRunOnMainThread<D, T>::Create(cWindow& window)
  {
    notifyMainThread.Create(window);
  }

  template <class D, class T>
  LRESULT CALLBACK cRunOnMainThread<D, T>::ProcessEvents(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    return notifyMainThread.ProcessEvents(hwnd, uMsg, wParam, lParam, *this, &cRunOnMainThread<D, T>::OnNotify);
  }

  template <class D, class T>
  void cRunOnMainThread<D, T>::OnNotify()
  {
    ASSERT(!queue.IsEmpty());

    T* pEvent = queue.RemoveItemFromFront();
    if (pEvent != nullptr) {
      pEvent->EventFunction(data);
      spitfire::SAFE_DELETE(pEvent);
    }
  }

  template <class D, class T>
  void cRunOnMainThread<D, T>::PushEventToMainThread(T* pEvent)
  {
    // Add the event to the queue
    queue.AddItemToBack(pEvent);

    // Notify the main thread
    notifyMainThread.Notify();
  }

  template <class D, class T>
  void cRunOnMainThread<D, T>::ClearEventQueue()
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

#endif // LIBWIN32MM_NOTIFYMAINTHREAD_H
