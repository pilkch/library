#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <spitfire/util/cString.h>

namespace spitfire
{
  namespace operatingsystem
  {
    enum NOTIFICATION {
      NOTIFICATION_INFORMATION,
      NOTIFICATION_WARNING
    };

    #ifdef __APPLE__

    ... Use Growl

    #elif defined(__GTK__)

    class cNotification
    {
    public:
      cNotification();

      bool IsValid();

      void SetType(NOTIFICATION _type) { type = _type; }

      void ShowNotification(const string_t& sTitle, const string_t& sDescription);

    private:
      static bool bIsGlobalInit;

      NOTIFICATION type;
    };

    #else

    // http://api.kde.org/4.0-api/kdelibs-apidocs/kdeui/html/classKNotification.html

    class cNotification
    {
    public:
      bool IsValid();

      void SetType(NOTIFICATION _type) { type = _type; }

      void ShowNotification(const string_t& sTitle, const string_t& sDescription);

    private:
      NOTIFICATION type;
    };

    #endif
  }
}

#endif // NOTIFICATIONS_H

