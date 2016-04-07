#ifndef CDATETIME_H
#define CDATETIME_H

// Standard headers
#include <chrono>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/string.h>

// http://www.boost.org/doc/libs/1_37_0/doc/html/date_time.html

namespace spitfire
{
  namespace util
  {
    const int cSecondsInADay = 86400;

    enum class WEEKDAY {
      MONDAY,
      TUESDAY,
      WEDNESDAY,
      THURSDAY,
      FRIDAY,
      SATURDAY,
      SUNDAY
    };

    string_t WeekDayToLongString(WEEKDAY weekDay);
    string_t WeekDayToShortString(WEEKDAY weekDay);
    WEEKDAY StringToWeekDay(const string_t& sWeekDay);

    string_t MonthToLongString(int month);
    string_t MonthToShortString(int month);
    int StringToMonth(const string_t& sMonth);

    struct cDateTimeFields {
      int year;
      int month;
      int day;
      int hours;
      int minutes;
      int seconds;
      int milliseconds;
    };

    class cDateTime
    {
    public:
      cDateTime(); // Defaults to local time now
      cDateTime(int year, int month, int day);
      cDateTime(int year, int month, int day, int hours, int minutes);
      cDateTime(int year, int month, int day, int hours, int minutes, int seconds);
      cDateTime(int year, int month, int day, int hours, int minutes, int seconds, int milliseconds);

      // Is special doesn't compile on gcc for some reason?  Complains about something deep down in a template function
      bool IsValid() const { return datetime != std::chrono::system_clock::time_point::time_point();  }

      bool IsUnixEpoch() const { return !IsValid(); }

      WEEKDAY GetWeekDay() const; // Returns the day of the week of this date

      bool GetDateTimeFields(cDateTimeFields& fields) const;

      void AddDays(int days);

      time_t GetTimeT() const;
      void SetFromTimeT(time_t time);


      // ISO 8601 UTC Format with time zone offset
      // 2013-12-19T00:16:34+00:00
      string_t GetISO8601UTCStringWithTimeZoneOffset(const std::chrono::system_clock::duration& offset) const;

      // ISO 8601 UTC format
      // 2013-12-19T00:16Z
      string_t GetISO8601UTCString() const;
      bool SetFromISO8601UTCString(const string_t& rhs, std::chrono::system_clock::duration& offset); // Returns true and sets offset if there is one
      bool SetFromISO8601UTCString(const string_t& rhs);


      // RFC 1123 Format
      // Must be in GMT time zone
      // Wed, 05 Jun 2013 13:08:10 GMT
      string_t GetRFC1123Format() const;
      bool SetFromRFC1123Format(const string_t& rhs);

      // RFC 3339 Format
      // YYYY-MM-DDThh:mm:ss
      string_t GetRFC3339String() const;
      bool SetFromRFC3339String(const string_t& rhs);

      // YYYYMMDD
      string_t GetDateYYYYMMDD() const;
      bool SetFromDateYYYYMMDD(const string_t& rhs);

      void SetFromUnixEpoch() { *this = cDateTime(1970, 1, 1); }
      void SetFromLocalTimeNow() { datetime = GetLocalTimeNow(); }
      void SetFromUniversalTimeNow() { datetime = GetUniversalTime(); }
      
      bool operator==(const cDateTime& rhs) const;
      bool operator<(const cDateTime& rhs) const;
      bool operator>(const cDateTime& rhs) const;

      cDateTime operator+(const std::chrono::system_clock::duration& duration) const;
      cDateTime operator-(const std::chrono::system_clock::duration& duration) const;

      // Get the difference between two dates
      std::chrono::system_clock::duration operator-(const cDateTime& rhs) const;

    private:
      static std::chrono::system_clock::time_point GetLocalTimeNow() { return std::chrono::system_clock::now(); }
      static std::chrono::system_clock::time_point GetUniversalTime();

      std::chrono::system_clock::time_point datetime;
    };
  }
}

#endif // CDATETIME_H
