#ifndef LIBWIN32MM_NOTIFYMAINTHREAD_H
#define LIBWIN32MM_NOTIFYMAINTHREAD_H

// Windows headers
#include <windows.h>

#undef Interface

// Spitfire headers
#include <spitfire/util/queue.h>

// libwin32mm headers
#include <libwin32mm/window.h>

#define WIN32MM_WM_USER_NOTIFY (WM_APP + 472)

namespace win32mm
{
  // ** cNotifyMainThread
  //
  // Runs a function on the main thread
  // NOTE: Generally cRunOnMainThread below should be used instead as it can process events on a queue

  template <class T>
  class cNotifyMainThread
  {
  public:
    cNotifyMainThread();

    void Create(T& t, void (T::*function)());
    void Destroy();

    void Notify();

  private:
    static LRESULT CALLBACK WindowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void OnNotify();

    T* pHandler;
    void (T::*pHandlerFunction)();

    HWND hwndCallback;
  };


  // ** cRunOnMainThread
  //
  // Sends events to the main thread

  template <class D, class T>
  class cRunOnMainThread
  {
  public:
    explicit cRunOnMainThread(D& data);
    ~cRunOnMainThread();

    void Create();
    void Destroy();

    #ifdef BUILD_DEBUG
    bool IsEmpty() { return queue.IsEmpty(); }
    #endif

    void PushEventToMainThread(T* pEvent);

    void ClearEventQueue();

  private:
    void OnNotify();

    D& data;

    cNotifyMainThread<cRunOnMainThread<D, T> > notifyMainThread;

    spitfire::util::cSignalObject soAction;

    spitfire::util::cThreadSafeQueue<T> queue;
  };

  // ** Inlines

  // ** cNotifyMainThread

  template <class T>
  inline cNotifyMainThread<T>::cNotifyMainThread() :
    pHandler(nullptr),
    pHandlerFunction(nullptr),
    hwndCallback(NULL)
  {
  }

  template <class T>
  inline void cNotifyMainThread<T>::Create(T& t, void (T::*function)())
  {
    pHandler = &t;
    pHandlerFunction = function;

    // Register our Window Class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WindowCallback;
    wc.hInstance = GetHInstance();
    wc.lpszClassName = TEXT("cNotifyMainThread_Class");
    const ATOM atom = ::RegisterClassEx(&wc);
    if (atom == 0) std::wcout<<"cNotifyMainThread<T>::Create RegisterClassEx FAILED, error="<<::GetLastError()<<std::endl;

    // Create our window
    hwndCallback = ::CreateWindow(TEXT("cNotifyMainThread_Class"), TEXT("cNotifyMainThread_Title"), WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetHInstance(), NULL);
    //hwndCallback = ::CreateWindowEx(WS_EX_TOOLWINDOW, TEXT("cNotifyMainThread"), TEXT(""), WS_POPUP, 0, 0, 0, 0, NULL, NULL, GetHInstance(), NULL);
    if (hwndCallback == 0) {
      ostringstream_t o;
      o<<::GetLastError();
      const string_t sLastError = o.str();
      std::wcout<<"cNotifyMainThread<T>::Create CreateWindowEx FAILED, error="<<sLastError<<std::endl;
    }

    ASSERT(hwndCallback != NULL);

    ::SetWindowLongPtr(hwndCallback, GWLP_USERDATA, (LONG)this);
  }

  template <class T>
  inline void cNotifyMainThread<T>::Destroy()
  {
    pHandler = nullptr;
    pHandlerFunction = nullptr;

    if (hwndCallback != NULL) {
      ::DestroyWindow(hwndCallback);
      hwndCallback = NULL;
    }
  }

  template <class T>
  inline void cNotifyMainThread<T>::OnNotify()
  {
    ASSERT(pHandler != nullptr);
    ASSERT(pHandlerFunction != nullptr);
    ((*pHandler).*(pHandlerFunction))();
  }

  template <class T>
  inline LRESULT CALLBACK cNotifyMainThread<T>::WindowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    // Check if this is our message
    if (uMsg == WIN32MM_WM_USER_NOTIFY) {
      cNotifyMainThread<T>* pThis = (cNotifyMainThread<T>*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
      ASSERT(pThis != nullptr);
      pThis->OnNotify();
      return FALSE;
    }

    // Call the default window proc
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

  template <class T>
  inline void cNotifyMainThread<T>::Notify()
  {
    ASSERT(hwndCallback != NULL);

    // Notify the main thread
    ::PostMessage(hwndCallback, WIN32MM_WM_USER_NOTIFY, 0, 0);
  }


  // ** cRunOnMainThread

  template <class D, class T>
  inline cRunOnMainThread<D, T>::cRunOnMainThread(D& _data) :
    data(_data),
    soAction(TEXT("cRunOnMainThread<T>::soAction")),
    queue(soAction)
  {
  }

  template <class D, class T>
  inline cRunOnMainThread<D, T>::~cRunOnMainThread()
  {
    ASSERT(queue.IsEmpty());
  }

  template <class D, class T>
  inline void cRunOnMainThread<D, T>::Create()
  {
    notifyMainThread.Create(*this, &cRunOnMainThread<D, T>::OnNotify);
  }

  template <class D, class T>
  inline void cRunOnMainThread<D, T>::Destroy()
  {
    notifyMainThread.Destroy();
  }

  template <class D, class T>
  inline void cRunOnMainThread<D, T>::OnNotify()
  {
    ASSERT(!queue.IsEmpty());

    T* pEvent = queue.RemoveItemFromFront();
    if (pEvent != nullptr) {
      pEvent->EventFunction(data);
      spitfire::SAFE_DELETE(pEvent);
    }
  }

  template <class D, class T>
  inline void cRunOnMainThread<D, T>::PushEventToMainThread(T* pEvent)
  {
    // Add the event to the queue
    queue.AddItemToBack(pEvent);

    // Notify the main thread
    notifyMainThread.Notify();
  }

  template <class D, class T>
  inline void cRunOnMainThread<D, T>::ClearEventQueue()
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
