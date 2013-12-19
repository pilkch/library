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
#include <libnotifymm.h>
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

    cNotificationSettings::cNotificationSettings(size_t _notificationID)
    {
      Clear();

      notificationID = _notificationID;
    }

    cNotificationSettings::cNotificationSettings()
    {
      Clear();
    }

    cNotificationSettings::cNotificationSettings(const cNotificationSettings& rhs)
    {
      Assign(rhs);
    }

    cNotificationSettings& cNotificationSettings::operator=(const cNotificationSettings& rhs)
    {
      Assign(rhs);
      return *this;
    }

    void cNotificationSettings::Assign(const cNotificationSettings& rhs)
    {
      ASSERT(this != &rhs);
      notificationID = rhs.notificationID;
      type = rhs.type;
      sTitle = rhs.sTitle;
      sDescription = rhs.sDescription;
      idAction1 = rhs.idAction1;
      sActionText1 = rhs.sActionText1;
      idAction2 = rhs.idAction2;
      sActionText2 = rhs.sActionText2;
      idAction3 = rhs.idAction3;
      sActionText3 = rhs.sActionText3;
      bActionsMusicPlayer = rhs.bActionsMusicPlayer;
    }

    void cNotificationSettings::Clear()
    {
      notificationID = 0;
      type = NOTIFICATION_TYPE::INFORMATION;
      sTitle.clear();
      sDescription.clear();
      idAction1 = 0;
      sActionText1.clear();
      idAction2 = 0;
      sActionText2.clear();
      idAction3 = 0;
      sActionText3.clear();
      bActionsMusicPlayer = false;
    }

    void cNotificationSettings::SetTitle(const string_t& _sTitle)
    {
      sTitle = _sTitle;
    }

    void cNotificationSettings::SetDescription(const string_t& _sDescription)
    {
      sDescription = _sDescription;
    }

    void cNotificationSettings::SetAction1(size_t id, const string_t& sText)
    {
      idAction1 = id;
      sActionText1 = sText;
    }

    void cNotificationSettings::SetAction2(size_t id, const string_t& sText)
    {
      idAction2 = id;
      sActionText2 = sText;
    }

    void cNotificationSettings::SetAction3(size_t id, const string_t& sText)
    {
      idAction3 = id;
      sActionText3 = sText;
    }

    void cNotificationSettings::SetActionsMusicPlayer(size_t _idAction1, size_t _idAction2, size_t _idAction3)
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

    cNotificationHandler* pHandler = nullptr;

    void NotificationInit(cNotificationHandler& handler)
    {
      LOG<<"NotificationInit"<<std::endl;

      Notify::init(SPITFIRE_APPLICATION_NAME);

      ASSERT(Notify::is_initted());

      bIsGlobalInit = true;
      pHandler = &handler;
    }

    void NotificationDestroy()
    {
      Notify::uninit();

      bIsGlobalInit = false;
      pHandler = nullptr;
    }


    // ** cNotification

    cNotification::cNotification() :
      bCreated(false),
      bClosed(true),
      notification("Caption", "Description")
    {
    }

    cNotification::~cNotification()
    {
      Close();
    }

    void cNotification::Show(const cNotificationSettings& _constSettings, durationms_t timeOutMS)
    {
      LOG<<"cNotification::Show"<<std::endl;

      cNotificationSettings _settings = _constSettings;

      // https://developer.gnome.org/libnotify/unstable/NotifyNotification.html
      // https://git.gnome.org/browse/rhythmbox/tree/plugins/notification/rb-notification-plugin.c#n125

      ASSERT(bIsGlobalInit);

      if (!bCreated) {
        LOG<<"cNotification::Show Installing signal_closed handler"<<std::endl;

        // Handle the closed signal
        notification.signal_closed().connect(sigc::mem_fun(*this, &cNotification::OnClosed));

        bCreated = true;
      }

      if (_settings.bActionsMusicPlayer != settings.bActionsMusicPlayer) {
        // Clear any existing hints
        notification.clear_hints();

        // Tell the desktop which desktop entry this notification is for
        notification.set_hint("desktop-entry", Glib::ustring(SPITFIRE_APPLICATION_NAME_LWR));

        if (_settings.bActionsMusicPlayer) {
          // Tell the desktop that this notification is persistent
          // NOTE: This means that the notification can be dismissed but never "closed" so the signal_closed notification will never get called
          notification.set_hint("resident", int(TRUE));

          // Tell the desktop that we want to show items for our action buttons
          notification.set_hint("action-icons", int(TRUE));

          // Tell the desktop which category our notification is for
          notification.set_category("x-gnome.music");
        }
      }

      // Update the text on our notification
      notification.update(_settings.sTitle.c_str(), _settings.sDescription.c_str(), "");

      // Set the timeout of the notification
      notification.set_timeout(timeOutMS);

      // Set the urgency level of the notification
      // NOTE: There is also Notify::URGENCY_LOW
      Notify::Urgency urgency = (_settings.type == NOTIFICATION_TYPE::WARNING) ? Notify::URGENCY_CRITICAL : Notify::URGENCY_NORMAL;
      notification.set_urgency(urgency);


      // Set the image
      Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file("data/application_32x32.png");
      notification.set_icon_from_pixbuf(image);

      // Set the actions for this notification
      if (_settings.bActionsMusicPlayer) {
        sActionName1 = "media-skip-backward";
        _settings.sActionText1 = "Previous";
        sActionName2 = "media-playback-pause";
        _settings.sActionText2 = "Pause";
        sActionName3 = "media-skip-forward";
        _settings.sActionText3 = "Next";
      } else {
        sActionName1 = _settings.sActionText1;
        sActionName2 = _settings.sActionText2;
        sActionName3 = _settings.sActionText3;
      }

      if (
        (_settings.idAction1 != settings.idAction1) ||
        (_settings.idAction2 != settings.idAction2) ||
        (_settings.idAction3 != settings.idAction3) ||
        (_settings.sActionText1 != settings.sActionText1) ||
        (_settings.sActionText2 != settings.sActionText2) ||
        (_settings.sActionText3 != settings.sActionText3)
      ) {
        // Clear any existing actions
        //notification.clear_actions();

        if (_settings.idAction1 != 0) {
          ASSERT(pHandler != nullptr); // There is no point in having an action if there is no handler to catch it

          notification.add_action(sActionName1.c_str(), _settings.sActionText1.c_str(), sigc::mem_fun(*this, &cNotification::OnActionClicked));

          if (_settings.idAction2 != 0) {
            notification.add_action(sActionName2.c_str(), _settings.sActionText2.c_str(), sigc::mem_fun(*this, &cNotification::OnActionClicked));

            if (_settings.idAction3 != 0) {
              notification.add_action(sActionName3.c_str(), _settings.sActionText3.c_str(), sigc::mem_fun(*this, &cNotification::OnActionClicked));
            }
          }
        }
      }

      settings = _settings;

      notification.show();

      bClosed = false;
    }

    void cNotification::Close()
    {
      if (!bClosed) {
        notification.close();

        bClosed = true;
      }
    }

    void cNotification::OnClosed()
    {
      LOG<<"cNotification::OnClosed"<<std::endl;

      if (pHandler != nullptr) {
        // Notify the handler if the notification was closed by clicking on the window area
        pHandler->OnNotificationClicked(settings.notificationID);
      }

      bClosed = true;
    }

    void cNotification::OnActionClicked(const Glib::ustring& sAction)
    {
      LOG<<"cNotification::OnActionClicked \""<<sAction.c_str()<<"\""<<std::endl;

      if (pHandler != nullptr) {
        if (sAction == sActionName1) pHandler->OnNotificationAction(settings.idAction1);
        else if (sAction == sActionName2) pHandler->OnNotificationAction(settings.idAction2);
        else if (sAction == sActionName3) pHandler->OnNotificationAction(settings.idAction3);
      }
    }

    #else

    // http://api.kde.org/4.0-api/kdelibs-apidocs/kdeui/html/classKNotification.html

    void cNotification::Show(const cNotificationSettings& settings, durationms_t timeOutMS)
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

