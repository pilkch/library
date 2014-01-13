#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <spitfire/util/string.h>

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

    class cNotification
    {
    public:
      friend void NotificationShow(const cNotification& notification, durationms_t timeOutMS);

      explicit cNotification(size_t notificationID);

      void SetInformation() { type = NOTIFICATION_TYPE::INFORMATION; }
      void SetWarning() { type = NOTIFICATION_TYPE::WARNING; }
      void SetTitle(const string_t& sTitle);
      void SetDescription(const string_t& sDescription);
      void SetAction1(size_t idAction, const string_t& sText);
      void SetAction2(size_t idAction, const string_t& sText);
      void SetAction3(size_t idAction, const string_t& sText);

      void SetActionsMusicPlayer(size_t idAction1, size_t idAction2, size_t idAction3);

    protected:
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
  }
}

#endif // NOTIFICATIONS_H
