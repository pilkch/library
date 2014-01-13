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

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>
#include <spitfire/util/lang.h>
#include <spitfire/util/log.h>

#include <spitfire/platform/notifications.h>

namespace spitfire
{
  namespace operatingsystem
  {
    const size_t NOTIFICATIONS_TIMEOUT_MS = 5000;

    // ** cNotification

    cNotification::cNotification(size_t _notificationID) :
      notificationID(_notificationID),
      type(NOTIFICATION_TYPE::INFORMATION),
      idAction1(0),
      idAction2(0),
      idAction3(0),
      bActionsMusicPlayer(false)
    {
    }

    void cNotification::SetTitle(const string_t& _sTitle)
    {
      sTitle = _sTitle;
    }

    void cNotification::SetDescription(const string_t& _sDescription)
    {
      sDescription = _sDescription;
    }

    void cNotification::SetAction1(size_t id, const string_t& sText)
    {
      idAction1 = id;
      sActionText1 = sText;
    }

    void cNotification::SetAction2(size_t id, const string_t& sText)
    {
      idAction2 = id;
      sActionText2 = sText;
    }

    void cNotification::SetAction3(size_t id, const string_t& sText)
    {
      idAction3 = id;
      sActionText3 = sText;
    }

    void cNotification::SetActionsMusicPlayer(size_t _idAction1, size_t _idAction2, size_t _idAction3)
    {
      idAction1 = _idAction1;
      idAction2 = _idAction2;
      idAction3 = _idAction3;
      bActionsMusicPlayer = true;
    }


    #ifdef __APPLE__

    ... Use Growl

    #elif defined(__GTK__)

    bool bIsGlobalInit = false;

    /*
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
    }*/

    cNotificationHandler* pHandler = nullptr;

    void NotificationInit(cNotificationHandler& handler)
    {
      notify_init(SPITFIRE_APPLICATION_NAME);

      ASSERT(notify_is_initted());

      bIsGlobalInit = true;
      pHandler = &handler;
    }

    void NotificationDestroy()
    {
      notify_uninit();

      bIsGlobalInit = false;
      pHandler = nullptr;
    }

    void NotificationActionCallback(NotifyNotification* pNotification, char* szAction, gpointer pUserData)
    {
      LOG<<"NotificationActionCallback"<<std::endl;

      ASSERT(pUserData != nullptr);

      const size_t id = size_t(pUserData);
      LOG<<"NotificationActionCallback id="<<id<<std::endl;
      if (pHandler != nullptr) {
        LOG<<"NotificationActionCallback Calling notification handler"<<std::endl;
        pHandler->OnNotificationAction(id);
      }

      g_object_unref(pNotification);
    }

    // http://cgit.collabora.com/git/user/jonathon/empathy/plain/src/empathy-misc.h
    const gint NOTIFICATION_CLOSED_INVALID = 0;
    const gint NOTIFICATION_CLOSED_EXPIRED = 1;
    const gint NOTIFICATION_CLOSED_DISMISSED = 2;
    const gint NOTIFICATION_CLOSED_PROGRAMMATICALY = 3;
    const gint NOTIFICATION_CLOSED_RESERVED = 4;

    void NotificationClosedCallback(NotifyNotification* pNotification, size_t notificationID)
    {
      if (pHandler != nullptr) {
        // Notify the handler if the notification was closed by clicking on the window area
        const gint reason = notify_notification_get_closed_reason(pNotification);
        if (reason == NOTIFICATION_CLOSED_DISMISSED) pHandler->OnNotificationClicked(notificationID);
      }
      g_object_unref(pNotification);
    }

    void NotificationShow(const cNotification& notification, durationms_t timeOutMS)
    {
      // https://developer.gnome.org/libnotify/unstable/NotifyNotification.html
      // https://git.gnome.org/browse/rhythmbox/tree/plugins/notification/rb-notification-plugin.c#n125

      ASSERT(bIsGlobalInit);

      NotifyNotification* pNotification = notify_notification_new(notification.sTitle.c_str(), notification.sDescription.c_str(), nullptr);

      // Tell the desktop which category our notification is for
      if (notification.bActionsMusicPlayer) notify_notification_set_category(pNotification, "x-gnome.music");

      // Tell the desktop which desktop entry this notification is for
      notify_notification_set_hint(pNotification, "desktop-entry", g_variant_new_string(SPITFIRE_APPLICATION_NAME_LWR));

      // Set the timeout of the notification
      notify_notification_set_timeout(pNotification, timeOutMS);

      // Set the urgency level of the notification
      NotifyUrgency urgency = notification.type == NOTIFICATION_TYPE::WARNING ? NOTIFY_URGENCY_CRITICAL : NOTIFY_URGENCY_NORMAL;
      notify_notification_set_urgency(pNotification, urgency);

      // Handle the closed signal
      g_signal_connect(pNotification, "closed", G_CALLBACK(NotificationClosedCallback), (void*)notification.notificationID);

      // Set the image
      Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file("data/application_32x32.png");
      notify_notification_set_image_from_pixbuf(pNotification, image->gobj());

      // Set the actions for this notification
      if (notification.bActionsMusicPlayer) {
        notify_notification_add_action(pNotification, "media-skip-backward", "Previous", NotificationActionCallback, (void*)notification.idAction1, nullptr);
        notify_notification_add_action(pNotification, "media-playback-pause", "Pause", NotificationActionCallback, (void*)notification.idAction2, nullptr);
        notify_notification_add_action(pNotification, "media-skip-forward", "Next", NotificationActionCallback, (void*)notification.idAction3, nullptr);

        notify_notification_set_hint(pNotification, "action-icons", g_variant_new_boolean(TRUE));
      } else if (notification.idAction1 != 0) {
        ASSERT(pHandler != nullptr); // There is no point in having an action if there is no handler to catch it

        notify_notification_add_action(pNotification, notification.sActionText1.c_str(), notification.sActionText1.c_str(), NotificationActionCallback, (void*)notification.idAction1, nullptr);

        if (notification.idAction2 != 0) {
          notify_notification_add_action(pNotification, notification.sActionText2.c_str(), notification.sActionText2.c_str(), NotificationActionCallback, (void*)notification.idAction2, nullptr);

          if (notification.idAction3 != 0) {
            notify_notification_add_action(pNotification, notification.sActionText3.c_str(), notification.sActionText3.c_str(), NotificationActionCallback, (void*)notification.idAction3, nullptr);
          }
        }
      }

      GError* error = nullptr;
      notify_notification_show(pNotification, &error);

      //Glib::RefPtr<Gtk::StatusIcon> status_icon =
      //Gtk::StatusIcon::create(Gtk::Stock::INFO);
      //Notify::Notification n("StatusIcon", "Testing StatusIcon mode with low urgency", "gtk-info", status_icon);

      /*Notify::Notification notification(string::ToUTF8(sTitle).c_str(), string::ToUTF8(sDescription).c_str());

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
#endif //GLIBMM_EXCEPTIONS_ENABLED*/
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

#ifdef BUILD_SPITFIRE_UNITTEST
#include <spitfire/util/string.h>
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
#endif // BUILD_SPITFIRE_UNITTEST

