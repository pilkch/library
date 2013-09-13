#ifndef CDATETIME_H
#define CDATETIME_H

// Boost headers
#include <boost/date_time.hpp>

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

    class cDateTime
    {
    public:
      cDateTime(); // Defaults to local time now
      cDateTime(int year, int month, int day);
      cDateTime(int year, int month, int day, int hour, int minute);
      cDateTime(int year, int month, int day, int hour, int minute, int second);
      cDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);

      // Is special doesn't compile on gcc for some reason?  Complains about something deep down in a template function
      bool IsValid() const { return !datetime.is_not_a_date_time() && !datetime.date().is_special() && !datetime.time_of_day().is_special();  }

      bool IsUnixEpoch() const { return ((GetYear() == 1970) && (GetMonth() == 1) && (GetDay() == 1) && (GetHours() == 0) && (GetMinutes() == 0) && (GetSeconds() == 0) && (GetMilliSeconds() == 0)); }

      WEEKDAY GetWeekDay() const; // Returns the day of the week of this date

      uint16_t GetYear() const { ASSERT(IsValid()); return datetime.date().year(); }
      uint16_t GetMonth() const { ASSERT(IsValid()); return datetime.date().month(); }
      uint16_t GetDay() const { ASSERT(IsValid()); return datetime.date().day(); }
      uint16_t GetHours() const { ASSERT(IsValid()); return datetime.time_of_day().hours(); }
      uint16_t GetMinutes() const { ASSERT(IsValid()); return datetime.time_of_day().minutes(); }
      uint16_t GetSeconds() const { ASSERT(IsValid()); return datetime.time_of_day().seconds(); }
      uint16_t GetMilliSeconds() const;

      uint32_t GetMillisecondsSinceMidnight() const;
      // This is the total milliseconds in the year, month, day, hours, minutes, seconds and milliseconds since 0 AD
      uint64_t GetMillisecondsSince0AD() const;

      void AddDays(int days);

      time_t GetTimeT() const;
      void SetFromTimeT(time_t time);

      string_t GetISO8601UTCStringWithTimeZoneOffset(const boost::posix_time::time_duration& offset) const;
      string_t GetISO8601UTCString() const;
      bool SetFromISO8601UTCString(const string_t& rhs, boost::posix_time::time_duration& offset); // Returns true and sets offset if there is one


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

      void ConvertFromLocalToUTC();
      void ConvertFromUTCToLocal();

      bool operator==(const cDateTime& rhs) const;
      bool operator<(const cDateTime& rhs) const;
      bool operator>(const cDateTime& rhs) const;

    private:
      static boost::posix_time::ptime GetLocalTimeNow() { return boost::posix_time::microsec_clock::local_time(); }
      static boost::posix_time::ptime GetUniversalTime() { return boost::posix_time::microsec_clock::universal_time(); }
      static boost::posix_time::time_duration GetLocalTimeZoneOffset();
      static bool IsTimeZoneOffsetNonZero(const boost::posix_time::time_duration& offset);

      boost::posix_time::ptime datetime;
    };
  }
}

#endif // CDATETIME_H
