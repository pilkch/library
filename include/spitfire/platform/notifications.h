#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <spitfire/util/cString.h>

namespace spitfire
{
  namespace operatingsystem
  {
    class enum NOTIFICATION {
      NOTIFICATION_INFORMATION,
      NOTIFICATION_WARNING
    };

    #ifdef __GTK__

    class cNotification
    {
    public:
      cNotification();

      bool IsValid();

      void SetType(NOTIFICATION _type) { type = _type; }
      void SetTitle(const string_t& sTitle);
      void SetDescription(const string_t& sDescription);
      void SetIcon(...);

      void Show();

    private:
      NOTIFICATION type;
    };

    #elif defined(__KDE__)

    // http://api.kde.org/4.0-api/kdelibs-apidocs/kdeui/html/classKNotification.html

    class cNotification
    {
    public:
      cNotification();

      bool IsValid();

      void SetType(NOTIFICATION _type) { type = _type; }
      void SetTitle(const string_t& sTitle);
      void SetDescription(const string_t& sDescription);
      void SetIcon(...);

      void Show();

    private:
      static bool bIsGlobalInit;

      NOTIFICATION type;
    };

    #elif defined(__APPLE__)

    // Use Growl

    class cNotification
    {
    public:
      bool IsValid();

      void SetType(NOTIFICATION _type) { type = _type; }
      void SetTitle(const string_t& sTitle);
      void SetDescription(const string_t& sDescription);
      void SetIcon(...);

      void Show();

    private:
      NOTIFICATION type;
    };

    #else

    class cNotification
    {
    public:
      cNotification();

      bool IsValid();

      void SetType(NOTIFICATION _type) { type = _type; }
      void SetTitle(const string_t& sTitle);
      void SetDescription(const string_t& sDescription);
      void SetIcon(...);

      void Show();

    private:
      NOTIFICATION type;
    };

    #endif
  }
}

#endif // NOTIFICATIONS_H
