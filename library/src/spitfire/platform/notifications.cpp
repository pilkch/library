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
#endif

#ifdef __GTK__
// libnotifymm headers
// NOTE: This is a non-standard, modified location
#include <libnotifymm/libnotifymm.h>
#endif

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/lang.h>
#include <spitfire/util/log.h>

#include <spitfire/platform/notifications.h>

namespace spitfire
{
  namespace operatingsystem
  {
    const size_t NOTIFICATIONS_TIMEOUT_MS = 5000;

    #ifdef __APPLE__

    ... Use Growl

    #elif defined(__GTK__)

    bool cNotification::bIsGlobalInit = false;

    cNotification::cNotification() :
      type(NOTIFICATION::NOTIFICATION_INFORMATION)
    {
      if (!bIsGlobalInit) {
        Notify::init(SPITFIRE_APPLICATION_NAME);
        bIsGlobalInit = true;
      }
    }

    bool cNotification::IsValid()
    {
      return (bIsGlobalInit && Notify::is_initted());
    }

    void cNotification::ShowNotification(const string_t& sTitle, const string_t& sDescription)
    {
      ASSERT(IsValid());

      //Glib::RefPtr<Gtk::StatusIcon> status_icon =
      //Gtk::StatusIcon::create(Gtk::Stock::INFO);
      //Notify::Notification n("StatusIcon", "Testing StatusIcon mode with low urgency", "gtk-info", status_icon);

      Notify::Notification notification(string::ToUTF8(sTitle).c_str(), string::ToUTF8(sDescription).c_str());

      // There is also Notify::URGENCY_LOW
      if (type == NOTIFICATION::NOTIFICATION_WARNING) notification.set_urgency(Notify::URGENCY_CRITICAL);

      notification.set_timeout(NOTIFICATIONS_TIMEOUT_MS);

#ifdef GLIBMM_EXCEPTIONS_ENABLED
      if (!notification.show()) {
          std::cerr<<"cNotification::ShowNotification Failed to send notification"<<std::endl;
      }
#else
      std::auto_ptr<Glib::Error> error;
      if (!notification.show(error)) {
          std::cerr<<"cNotification::ShowNotification Failed to send notification error=\""<<error->what()<<"\""<<std::endl;
      }
#endif //GLIBMM_EXCEPTIONS_ENABLED
    }

    #else

    // http://api.kde.org/4.0-api/kdelibs-apidocs/kdeui/html/classKNotification.html

    void cNotification::ShowNotification(const string_t& sTitle, const string_t& sDescription)
    {
      string_t sType = TEXT("Information");

      if (type == NOTIFICATION::NOTIFICATION_WARNING) sType = TEXT("Warning");

      KNotification* pNotification = new KNotification(sType);
      pNotification->setTitle(sTitle);
      pNotification->setText(sDescription);
      pNotification->setPixmap(contact->pixmap());
      pNotification->setActions(QStringList(i18n("Open chat")));

      foreach(const QString& group, contact->groups()) {
          pNotification->addContext("group", group);
      }

      connect(pNotification, SIGNAL(activated(unsigned int)), contact, SLOT(slotOpenChat()));

      pNotification->sendEvent();

      // TODO: Is there a memory leak here, not releasing pNotification?
      #warning "Is there a memory leak here, not releasing pNotification?"

      #error "KDE HAS NOT BEEN TESTED"
    }

    #endif
  }
}

#ifdef BUILD_DEBUG
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/unittest.h>

class cNotificationUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cNotificationUnitTest() :
    cUnitTestBase(TEXT("cNotificationUnitTest"))
  {
  }

  void Test()
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
};

cNotificationUnitTest gNotificationUnitTest;
#endif // BUILD_DEBUG

