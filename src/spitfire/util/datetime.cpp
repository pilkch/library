#include <cassert>
#include <cctype> // toupper/tolower
#include <cmath>

#include <string>
#include <vector>
#include <list>
#include <algorithm>

// Reading and writing text files
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>

#include <spitfire/util/string.h>
#include <spitfire/util/log.h>
#include <spitfire/util/datetime.h>

namespace spitfire
{
  namespace util
  {
    string_t WeekDayToLongString(WEEKDAY weekDay)
    {
      if (weekDay == WEEKDAY::MONDAY) return TEXT("Monday");
      else if (weekDay == WEEKDAY::TUESDAY) return TEXT("Tuesday");
      else if (weekDay == WEEKDAY::WEDNESDAY) return TEXT("Wednesday");
      else if (weekDay == WEEKDAY::THURSDAY) return TEXT("Thursday");
      else if (weekDay == WEEKDAY::FRIDAY) return TEXT("Friday");
      else if (weekDay == WEEKDAY::SATURDAY) return TEXT("Saturday");

      return TEXT("Sunday");
    }

    string_t WeekDayToShortString(WEEKDAY weekDay)
    {
      if (weekDay == WEEKDAY::MONDAY) return TEXT("Mon");
      else if (weekDay == WEEKDAY::TUESDAY) return TEXT("Tue");
      else if (weekDay == WEEKDAY::WEDNESDAY) return TEXT("Wed");
      else if (weekDay == WEEKDAY::THURSDAY) return TEXT("Thu");
      else if (weekDay == WEEKDAY::FRIDAY) return TEXT("Fri");
      else if (weekDay == WEEKDAY::SATURDAY) return TEXT("Sat");

      return TEXT("Sun");
    }

    WEEKDAY StringToWeekDay(string_t sWeekDay)
    {
      WEEKDAY weekDay = WEEKDAY::SUNDAY;

      if ((sWeekDay == TEXT("Monday")) || (sWeekDay == TEXT("Mon"))) weekDay = WEEKDAY::MONDAY;
      else if ((sWeekDay == TEXT("Tuesday")) || (sWeekDay == TEXT("Tue"))) weekDay = WEEKDAY::TUESDAY;
      else if ((sWeekDay == TEXT("Wednesday")) || (sWeekDay == TEXT("Wed"))) weekDay = WEEKDAY::WEDNESDAY;
      else if ((sWeekDay == TEXT("Thursday")) || (sWeekDay == TEXT("Thu"))) weekDay = WEEKDAY::THURSDAY;
      else if ((sWeekDay == TEXT("Friday")) || (sWeekDay == TEXT("Fri"))) weekDay = WEEKDAY::FRIDAY;
      else if ((sWeekDay == TEXT("Saturday")) || (sWeekDay == TEXT("Sat"))) weekDay = WEEKDAY::SATURDAY;

      return weekDay;
    }

    string_t MonthToLongString(int month)
    {
      const string_t months[12] = {
        TEXT("January"),
        TEXT("February"),
        TEXT("March"),
        TEXT("April"),
        TEXT("May"),
        TEXT("June"),
        TEXT("July"),
        TEXT("August"),
        TEXT("September"),
        TEXT("October"),
        TEXT("November"),
        TEXT("December"),
      };

      ASSERT((month >= 1) && (month <= 12));
      return months[month];
    }

    string_t MonthToShortString(int month)
    {
      const string_t months[12] = {
        TEXT("Jan"),
        TEXT("Feb"),
        TEXT("Mar"),
        TEXT("Apr"),
        TEXT("May"),
        TEXT("Jun"),
        TEXT("Jul"),
        TEXT("Aug"),
        TEXT("Sep"),
        TEXT("Oct"),
        TEXT("Nov"),
        TEXT("Dec"),
      };

      ASSERT((month >= 1) && (month <= 12));
      return months[month - 1];
    }

    int StringToMonth(const string_t& sMonth)
    {
      int month = 12;

      if ((sMonth == TEXT("January")) || (sMonth == TEXT("Jan"))) month = 1;
      else if ((sMonth == TEXT("February")) || (sMonth == TEXT("Feb"))) month = 2;
      else if ((sMonth == TEXT("March")) || (sMonth == TEXT("Mar"))) month = 3;
      else if ((sMonth == TEXT("April")) || (sMonth == TEXT("Apr"))) month = 4;
      else if ((sMonth == TEXT("May")) || (sMonth == TEXT("May"))) month = 5;
      else if ((sMonth == TEXT("June")) || (sMonth == TEXT("Jun"))) month = 6;
      else if ((sMonth == TEXT("July")) || (sMonth == TEXT("Jul"))) month = 7;
      else if ((sMonth == TEXT("August")) || (sMonth == TEXT("Aug"))) month = 8;
      else if ((sMonth == TEXT("September")) || (sMonth == TEXT("Sep"))) month = 9;
      else if ((sMonth == TEXT("October")) || (sMonth == TEXT("Oct"))) month = 10;
      else if ((sMonth == TEXT("November")) || (sMonth == TEXT("Nov"))) month = 11;
      else if ((sMonth == TEXT("December")) || (sMonth == TEXT("Dec"))) month = 12;

      return month;
    }


    // Defaults to local time now
    cDateTime::cDateTime()
    {
      SetFromLocalTimeNow();
    }

    cDateTime::cDateTime(int year, int month, int day) :
      cDateTime(year, month, day, 0, 0)
    {
    }

    cDateTime::cDateTime(int year, int month, int day, int hours, int minutes) :
      cDateTime(year, month, day, hours, minutes, 0)
    {
    }

    cDateTime::cDateTime(int year, int month, int day, int hours, int minutes, int seconds)
    {
      struct std::tm t;
      t.tm_sec = seconds;        // second of minute (0 .. 59 and 60 for leap seconds)
      t.tm_min = minutes;        // minute of hour (0 .. 59)
      t.tm_hour = hours;      // hour of day (0 .. 23)
      t.tm_mday = day;       // day of month (0 .. 31)
      t.tm_mon = month - 1;      // month of year (0 .. 11)
      t.tm_year = year - 1900; // year since 1900
      t.tm_isdst = -1;       // determine whether daylight saving time
      const std::time_t time = std::mktime(&t);
      ASSERT(time != -1);

      SetFromTimeT(time);
    }

    void cDateTime::AddDays(int days)
    {
      datetime += std::chrono::hours(days * 24);
    }

    WEEKDAY cDateTime::GetWeekDay() const
    {
      time_t time = std::chrono::system_clock::to_time_t(datetime);
      std::chrono::system_clock::time_point currentTimeRounded = std::chrono::system_clock::from_time_t(time);
      if (currentTimeRounded > datetime) {
        time--;
        currentTimeRounded -= std::chrono::seconds(1);
      }
      tm values;
      #ifdef __WIN__
      localtime_s(&values, &time);
      #else
      localtime_r(&time, &values);
      #endif

      WEEKDAY result = WEEKDAY::SUNDAY;

      switch (values.tm_wday) {
        case 0: result = WEEKDAY::SUNDAY; break;
        case 1: result = WEEKDAY::MONDAY; break;
        case 2: result = WEEKDAY::TUESDAY; break;
        case 3: result = WEEKDAY::WEDNESDAY; break;
        case 4: result = WEEKDAY::THURSDAY; break;
        case 5: result = WEEKDAY::FRIDAY; break;
        case 6: result = WEEKDAY::SATURDAY; break;
      };

      return result;
    }

    bool cDateTime::GetDateTimeFields(cDateTimeFields& fields) const
    {
      time_t time = std::chrono::system_clock::to_time_t(datetime);
      std::chrono::system_clock::time_point currentTimeRounded = std::chrono::system_clock::from_time_t(time);
      if (currentTimeRounded > datetime) {
        time--;
        currentTimeRounded -= std::chrono::seconds(1);
      }
      tm values;
      #ifdef __WIN__
      localtime_s(&values, &time);
      #else
      localtime_r(&time, &values);
      #endif

      fields.year = values.tm_year + 1900;
      fields.month = values.tm_mon;
      fields.day = values.tm_mday;
      fields.hours = values.tm_hour;
      fields.minutes = values.tm_min;
      fields.seconds = values.tm_sec;
      fields.milliseconds = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(datetime - currentTimeRounded).count();

      return true;
    }

    time_t cDateTime::GetTimeT() const
    {
      return std::chrono::system_clock::to_time_t(datetime);
    }

    void cDateTime::SetFromTimeT(time_t time)
    {
      datetime = std::chrono::system_clock::from_time_t(time);
    }

    // Stolen from http://stackoverflow.com/a/9088549/1074390
    // Get the UTC timezone offset (e.g. -8 hours for PST)
    int get_utc_offset()
    {
      // Get the local time for Jan 2, 1900 00:00 UTC
      const time_t zero = 24 * 60 * 60L;
      struct tm tm_utc;
      #ifdef __WIN__
      localtime_s(&tm_utc, &zero);
      #else
      localtime_r(&zero, &tm_utc);
      #endif
      int gmtime_hours = tm_utc.tm_hour;

      // If the local time is the "day before" the UTC, subtract 24 hours from the hours to get the UTC offset
      if (tm_utc.tm_mday < 2) gmtime_hours -= 24;

      return gmtime_hours;
    }

    // Stolen from http://stackoverflow.com/a/9088549/1074390
    // The UTC analogue of mktime (much like timegm on some systems)
    time_t tm_to_time_t_utc(struct tm& tm_utc)
    {
      // Get the epoch time relative to the local time zone, and then add the appropriate number of seconds to make it UTC
      return std::mktime(&tm_utc) + get_utc_offset() * 3600;
    }

    std::chrono::system_clock::time_point cDateTime::GetUniversalTime()
    {
      const time_t now = time(NULL);
      tm tm_now_utc;
      #ifdef __WIN__
      gmtime_s(&tm_now_utc, &now);
      #else
      gmtime_r(&now, &tm_now_utc);
      #endif

      const time_t now_utc = tm_to_time_t_utc(tm_now_utc);

      return std::chrono::system_clock::from_time_t(now_utc);
    }

    // http://en.wikipedia.org/wiki/ISO_8601
    // Z means UTC zero offset ie. this time is global and to get the local time we then added our local time zone offset
    // 19980717T140855,10Z
    // OR
    // 19980717T140855,10+hhmm
    /*string_t cDateTime::GetISO8601UTCStringWithTimeZoneOffset(const std::chrono::system_clock::duration& offset) const
    {
      ASSERT(IsValid());

      ostringstream_t o;

      // "YYYYMMDDTHHMMSS,fffffffff"
      o<<spitfire::string::ToString_t(to_iso_string(datetime));

      // "Z"
      // OR
      // "+hhmm"
      // OR
      // "-hhmm"
      if (IsTimeZoneOffsetNonZero(offset)) {
        if (offset.is_negative()) o<<TEXT("-");
        else o<<TEXT("+");

        std::ostringstream o;
        o<<std::setw(2)<<std::setfill('0')<<offset.hours();
        o<<std::setw(2)<<std::setfill('0')<<offset.minutes();
      } else o<<TEXT("Z");

      return o.str();
    }

    // http://en.wikipedia.org/wiki/ISO_8601
    // Z means UTC zero offset ie. this time is global and to get the local time we then added our local time zone offset
    // 19980717T140855,10Z
    string_t cDateTime::GetISO8601UTCString() const
    {
      ASSERT(IsValid());

      ostringstream_t o;

      // "YYYYMMDDTHHMMSS,fffffffff"
      o<<spitfire::string::ToString_t(to_iso_string(datetime));

      // "Z"
      o<<TEXT("Z");

      return o.str();
    }

    // http://en.wikipedia.org/wiki/ISO_8601
    // Z means UTC zero offset ie. this time is global and to get the local time we then added our time zone offset
    // This function can cope with either of these formats
    // 19980717T140855,10Z
    // OR
    // 19980717T140855,10+hhmm
    bool cDateTime::SetFromISO8601UTCString(const string_t& rhs, std::chrono::system_clock::duration& offset)
    {
      // Set the offset to zero for the moment
      datetime = std::chrono::system_clock::time_point(boost::gregorian::date(0, boost::gregorian::Jan, 1), boost::posix_time::hours(0));
      offset = std::chrono::system_clock::duration(0, 0, 0, 0);

      // Find either a Z, + or -
      size_t zOrPlusOrMinus = rhs.find(TEXT("Z"));
      if (zOrPlusOrMinus == string_t::npos) zOrPlusOrMinus = rhs.find(TEXT("+"));
      if (zOrPlusOrMinus == string_t::npos) zOrPlusOrMinus = rhs.find(TEXT("-"));

      // If we haven't found one then we don't have a valid ISO8601 string, return
      if (zOrPlusOrMinus == string_t::npos) return false;

      // Split the string into "19980717T140855,10" character "hhmm"
      const string_t before = rhs.substr(0, zOrPlusOrMinus);
      const char_t character = rhs[zOrPlusOrMinus];
      const string_t after = rhs.substr(zOrPlusOrMinus + 1);

      // 19980717T140855,10
      datetime = boost::posix_time::from_iso_string(spitfire::string::ToUTF8(before));

      // We have a Z character so this is non timezone UTC time, return false to indicate that we don't have a timezone
      if (character == TEXT('Z')) return false;

      // +/-hhmm
      int hours = spitfire::string::ToUnsignedInt(after.substr(0, 1));
      if (character == TEXT('-')) hours = -hours;

      int minutes = spitfire::string::ToUnsignedInt(after.substr(2, 3));

      offset = std::chrono::system_clock::duration(hours, minutes, 0, 0);
      return true;
    }

    bool cDateTime::SetFromISO8601UTCString(const string_t& rhs)
    {
      std::chrono::system_clock::duration offset;
      return SetFromISO8601UTCString(rhs, offset);
    }*/


    // RFC 1123 Format
    // Wed, 05 Jun 2013 13:08:10 GMT
    string_t cDateTime::GetRFC1123Format() const
    {
      ASSERT(IsValid());

      cDateTimeFields fields;
      GetDateTimeFields(fields);

      ostringstream_t o;

      o<<std::setfill(TEXT('0'));

      o<<WeekDayToShortString(GetWeekDay());
      o<<TEXT(", ");
      o<<std::setw(2)<<std::setfill(TEXT('0'))<<int(fields.day);
      o<<TEXT(" ");
      o<<MonthToShortString(fields.month);
      o<<TEXT(" ");
      o<<std::setw(4)<<std::setfill(TEXT('0'))<<int(fields.year);
      o<<TEXT(" ");
      o<<std::setw(2)<<std::setfill(TEXT('0'))<<int(fields.hours);
      o<<TEXT(':');
      o<<std::setw(2)<<std::setfill(TEXT('0'))<<int(fields.minutes);
      o<<TEXT(':');
      o<<std::setw(2)<<std::setfill(TEXT('0'))<<int(fields.seconds);
      o<<TEXT(" GMT");

      return o.str();
    }

    // RFC 1123 Format (Friendly variation)
    // Wed, 05 Jun 2013 13:08:10
    string_t cDateTime::GetRFC1123FormatNoTimeZone() const
    {
      ASSERT(IsValid());

      cDateTimeFields fields;
      GetDateTimeFields(fields);

      ostringstream_t o;

      o<<std::setfill(TEXT('0'));

      o<<WeekDayToShortString(GetWeekDay());
      o<<TEXT(", ");
      o<<std::setw(2)<<std::setfill(TEXT('0'))<<int(fields.day);
      o<<TEXT(" ");
      o<<MonthToShortString(fields.month);
      o<<TEXT(" ");
      o<<std::setw(4)<<std::setfill(TEXT('0'))<<int(fields.year);
      o<<TEXT(" ");
      o<<std::setw(2)<<std::setfill(TEXT('0'))<<int(fields.hours);
      o<<TEXT(':');
      o<<std::setw(2)<<std::setfill(TEXT('0'))<<int(fields.minutes);
      o<<TEXT(':');
      o<<std::setw(2)<<std::setfill(TEXT('0'))<<int(fields.seconds);

      return o.str();
    }

    /*// RFC 3339 Format
    // YYYY-MM-DDThh:mm:ss
    // 2006-03-20T17:53:38
    string_t cDateTime::GetRFC3339String() const
    {
      ASSERT(IsValid());

      ostringstream_t o;

      o<<std::setfill(TEXT('0'));

      o<<std::setw(4)<<GetYear();
      o<<std::setw(2)<<GetMonth();
      o<<std::setw(2)<<GetDay();
      o<<TEXT('T');
      o<<std::setw(2)<<GetHours();
      o<<TEXT(':');
      o<<std::setw(2)<<GetMinutes();
      o<<TEXT(':');
      o<<std::setw(2)<<GetSeconds();

      return o.str();
    }

    // YYYYMMDD
    string_t cDateTime::GetDateYYYYMMDD() const
    {
      ASSERT(IsValid());

      ostringstream_t o;

      o<<std::setfill(TEXT('0'));

      o<<std::setw(4)<<GetYear();
      o<<std::setw(2)<<GetMonth();
      o<<std::setw(2)<<GetDay();

      return o.str();
    }*/

    bool cDateTime::operator==(const cDateTime& rhs) const
    {
      return (datetime == rhs.datetime);
    }

    bool cDateTime::operator<(const cDateTime& rhs) const
    {
      return (datetime < rhs.datetime);
    }

    bool cDateTime::operator>(const cDateTime& rhs) const
    {
      return (datetime > rhs.datetime);
    }

    cDateTime cDateTime::operator+(const std::chrono::system_clock::duration& duration) const
    {
      cDateTime copy(*this);
      copy.datetime += duration;
      return copy;
    }

    cDateTime cDateTime::operator-(const std::chrono::system_clock::duration& duration) const
    {
      cDateTime copy(*this);
      copy.datetime -= duration;
      return copy;
    }

    std::chrono::system_clock::duration cDateTime::operator-(const cDateTime& rhs) const
    {
      return datetime - rhs.datetime;
    }
  }
}

