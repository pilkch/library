#ifndef CDATETIME_H
#define CDATETIME_H

// http://www.boost.org/doc/libs/1_37_0/doc/html/date_time.html

namespace spitfire
{
  namespace util
  {
    const int cSecondsInADay = 86400;

    class cDateTime
    {
    public:
      cDateTime(); // Defaults to local time now
      cDateTime(int year, int month, int day);
      cDateTime(int year, int month, int day, int hour, int minute);
      cDateTime(int year, int month, int day, int hour, int minute, int second);
      cDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);

      // Is special doesn't compile on gcc for some reason?  Complains about something deep down in a template function
      bool IsValid() const { return !datetime.is_not_a_date_time(); /*(!datetime.date().is_special()) && (!datetime.time_of_day().is_special());*/  }

      uint16_t GetYear() const { ASSERT(IsValid()); return datetime.date().year(); }
      uint8_t GetMonth() const { ASSERT(IsValid()); return datetime.date().month(); }
      uint8_t GetDay() const { ASSERT(IsValid()); return datetime.date().day(); }
      uint8_t GetHours() const { ASSERT(IsValid()); return datetime.time_of_day().hours(); }
      uint8_t GetMinutes() const { ASSERT(IsValid()); return datetime.time_of_day().minutes(); }
      uint8_t GetSeconds() const { ASSERT(IsValid()); return datetime.time_of_day().seconds(); }
      uint16_t GetMilliSeconds() const;

      // This is the total milliseconds in the year, month, day, hours, minutes, seconds and milliseconds since 0 AD
      uint32_t GetMillisecondsSinceMidnight() const;

      string_t GetISO8601UTCStringWithTimeZoneOffset(const boost::posix_time::time_duration& offset) const;
      string_t GetISO8601UTCString() const;
      bool SetFromISO8601UTCString(const string_t& rhs, boost::posix_time::time_duration& offset); // Returns true and sets offset if there is one

      void SetFromLocalTimeNow() { datetime = GetLocalTimeNow(); }
      void SetFromUniversalTimeNow() { datetime = GetUniversalTime(); }

      void ConvertFromLocalToUTC();
      void ConvertFromUTCToLocal();

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
