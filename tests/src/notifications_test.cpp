// Standard headers
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <memory> // For auto_ptr
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#ifdef __LINUX__
#include <errno.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#endif

#ifdef __WIN__
#include <windows.h>
#elif defined(__GTK__)
// libnotifymm headers
// NOTE: This is a non-standard, modified location
//#include <libnotifymm/libnotifymm.h>

// libnotify headers
#include <libnotify/notify.h>

// gtkmm headers
#include <gtkmm.h>
#endif

#include <gtest/gtest.h>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/platform/notifications.h>
#include <spitfire/util/string.h>
#include <spitfire/util/lang.h>
#include <spitfire/util/log.h>

TEST(SpitfirePlatform, TestNotifications)
{
  {
    spitfire::operatingsystem::cNotification notification;

    if (notification.IsValid()) {
      notification.SetType(spitfire::operatingsystem::NOTIFICATION::NOTIFICATION_INFORMATION);

      notification.ShowNotification(TEXT("Title for Information"), TEXT("Description for Information"));
    }
  }
  {
    spitfire::operatingsystem::cNotification notification;

    if (notification.IsValid()) {
      notification.SetType(spitfire::operatingsystem::NOTIFICATION::NOTIFICATION_WARNING);

      notification.ShowNotification(TEXT("Title for Warning"), TEXT("Description for Warning"));
    }
  }
}
