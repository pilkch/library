#ifndef SPITFIRE_NOTIFYEVENT_H
#define SPITFIRE_NOTIFYEVENT_H

// Spitfire headers
#include <spitfire/spitfire.h>

namespace spitfire
{
  namespace util
  {
    // ** cNotifyEvent
    // This class is used in conjunction with the native messaging system such as GTK or Win32
    // to send messages to the main thread from a second thread.

    template <class T>
    class cNotifyEvent
    {
    public:
      explicit cNotifyEvent(T& target);
      virtual ~cNotifyEvent() {}

      void EventFunction() { _EventFunction(); }

    protected:
      T& target;

    private:
      virtual void _EventFunction() = 0;
    };

    // Inlines

    template <class T>
    cNotifyEvent<T>::cNotifyEvent(T& _target) :
      target(_target)
    {
    }
  }
}

#endif // !SPITFIRE_NOTIFYEVENT_H
