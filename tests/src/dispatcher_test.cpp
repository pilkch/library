// Standard headers
#include <iostream>

// Spitfire headers
#include <spitfire/util/thread.h>

// libgtkmm headers
#include <libgtkmm/dispatcher.h>

class cTestDispatcher
{
public:
  cTestDispatcher() : notified_on_main_thread(false), notified_on_another_thread(false) {}

  void Test()
  {
    notify.Create(*this, &cTest::OnNotification);
    notify.Notify();
  }

  bool notified_on_main_thread;
  bool notified_on_another_thread;

private:
  void OnNotification()
  {
    std::cout<<"cTest::OnNotification"<<std::endl;
    if (spitfire::util::IsMainThread()) {
      notified_on_main_thread = true;
    } else {
      notified_on_another_thread = true;
    }
  }

  cGtkmmNotifyMainThread notify;
};

TEST(Gtkmm, TestDispatcher)
{
  cTestDispatcher dispatcher;
  dispatcher.Test();

  ASSERT_TRUE(dispatcher.notified_on_main_thread);
  ASSERT_FALSE(dispatcher.notified_on_another_thread);
}
