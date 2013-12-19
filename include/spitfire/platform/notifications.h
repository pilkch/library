#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <spitfire/util/string.h>

#ifdef __GTK__
// libnotifymm headers
#include <libnotifymm.h>
#endif

namespace spitfire
{
  namespace operatingsystem
  {
    class cNotification;

    class cNotificationHandler
    {
    public:
      virtual ~cNotificationHandler() {}

      virtual void OnNotificationClicked(size_t notificationID) = 0;
      virtual void OnNotificationAction(size_t actionID) = 0;
    };

    void NotificationInit(cNotificationHandler& handler);
    void NotificationDestroy();
    void NotificationShow(const cNotification& notification, durationms_t timeOutMS);


    enum class NOTIFICATION_TYPE {
      INFORMATION,
      WARNING
    };

    class cNotification;

    class cNotificationSettings
    {
    public:
      friend class cNotification;

      explicit cNotificationSettings(size_t notificationID);

      void SetInformation() { type = NOTIFICATION_TYPE::INFORMATION; }
      void SetWarning() { type = NOTIFICATION_TYPE::WARNING; }
      void SetTitle(const string_t& sTitle);
      void SetDescription(const string_t& sDescription);
      void SetAction1(size_t idAction, const string_t& sText);
      void SetAction2(size_t idAction, const string_t& sText);
      void SetAction3(size_t idAction, const string_t& sText);

      void SetActionsMusicPlayer(size_t idAction1, size_t idAction2, size_t idAction3);

    protected:
      cNotificationSettings();

      cNotificationSettings(const cNotificationSettings& rhs);
      cNotificationSettings& operator=(const cNotificationSettings& rhs);

      void Assign(const cNotificationSettings& rhs);
      void Clear();

      size_t notificationID;
      NOTIFICATION_TYPE type;
      string_t sTitle;
      string_t sDescription;
      size_t idAction1;
      string_t sActionText1;
      size_t idAction2;
      string_t sActionText2;
      size_t idAction3;
      string_t sActionText3;

      bool bActionsMusicPlayer;
    };

    class cNotification
    {
    public:
      cNotification();
      ~cNotification();

      void Show(const cNotificationSettings& settings, durationms_t timeOutMS);
      void Close();

    private:
      void OnClosed();
      void OnActionClicked(const Glib::ustring& sAction);

      bool bCreated;
      bool bClosed;
      Notify::Notification notification;

      cNotificationSettings settings;

      string_t sActionName1;
      string_t sActionName2;
      string_t sActionName3;
    };
  }
}

#endif // NOTIFICATIONS_H
