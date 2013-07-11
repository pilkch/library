// Standard headers
#include <iostream>

// Spitfire headers
#include <spitfire/util/thread.h>

// libgtkmm headers
#include <libgtkmm/dispatcher.h>

namespace gtkmm
{
  #ifdef BUILD_SUPPORT_UNITTESTS
  class cTest
  {
  public:
    void Test()
    {
      notify.Create(*this, &cTest::OnNotification);
      notify.Notify();
    }

  private:
    void OnNotification()
    {
      std::cout<<"cTest::OnNotification"<<std::endl;
      assert(spitfire::util::IsMainThread());
    }

    cGtkmmNotifyMainThread notify;
  };
  #endif // BUILD_SUPPORT_UNITTESTS
}
