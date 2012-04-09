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

// Boost headers
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

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
    // Defaults to local time now
    cDateTime::cDateTime()
    {
      SetFromLocalTimeNow();
    }

    cDateTime::cDateTime(int year, int month, int day)
    {
      datetime = boost::posix_time::ptime(boost::gregorian::date(year, month, day), boost::posix_time::hours(0));
    }

    cDateTime::cDateTime(int year, int month, int day, int hour, int minute)
    {
      datetime = boost::posix_time::ptime(boost::gregorian::date(year, month, day), boost::posix_time::hours(hour) + boost::posix_time::minutes(minute));
    }

    cDateTime::cDateTime(int year, int month, int day, int hour, int minute, int second)
    {
      datetime = boost::posix_time::ptime(boost::gregorian::date(year, month, day), boost::posix_time::hours(hour) + boost::posix_time::minutes(minute) + boost::posix_time::seconds(second));
    }

    // Same again, but multiply milliseconds by 1000 get get microseconds
    cDateTime::cDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
    {
      datetime = boost::posix_time::ptime(boost::gregorian::date(year, month, day), boost::posix_time::hours(hour) + boost::posix_time::minutes(minute) + boost::posix_time::seconds(second) + boost::posix_time::microseconds(1000 * millisecond));
    }

    WEEKDAY cDateTime::GetWeekDay() const
    {
      WEEKDAY result = WEEKDAY::MONDAY;
      boost::gregorian::date d = datetime.date();
      boost::gregorian::greg_weekday wd = d.day_of_week();
      switch (wd) {
        case boost::gregorian::Monday: result = WEEKDAY::MONDAY; break;
        case boost::gregorian::Tuesday: result = WEEKDAY::TUESDAY; break;
        case boost::gregorian::Wednesday: result = WEEKDAY::WEDNESDAY; break;
        case boost::gregorian::Thursday: result = WEEKDAY::THURSDAY; break;
        case boost::gregorian::Friday: result = WEEKDAY::FRIDAY; break;
        case boost::gregorian::Saturday: result = WEEKDAY::SATURDAY; break;
        case boost::gregorian::Sunday: result = WEEKDAY::SUNDAY; break;
      };

      return result;
    }

    uint16_t cDateTime::GetMilliSeconds() const
    {
      ASSERT(IsValid());
      // Unfortunately there is not a simple function to get the number of milliseconds, so we get the total
      // number of milliseconds and then subtract the hours, minutes and seconds to get the remainder
      const boost::posix_time::time_duration duration = datetime.time_of_day();
      const long millisecondsTotal = duration.total_milliseconds();
      const long remainder = millisecondsTotal - (((((duration.hours() * 60) + duration.minutes()) * 60) + duration.seconds()) * 1000);
      ASSERT(remainder >= 0);
      ASSERT(remainder < 1000);
      return uint16_t(remainder);
    }

    uint32_t cDateTime::GetMillisecondsSinceMidnight() const
    {
      ASSERT(IsValid());
      return ((((((GetHours() * 60) + GetMinutes()) * 60) + GetSeconds()) * 1000) + GetMilliSeconds());
    }

    // This is the total milliseconds in the year, month, day, hours, minutes, seconds and milliseconds since 0 AD
    uint64_t cDateTime::GetMillisecondsSince0AD() const
    {
      ASSERT(IsValid());
      const boost::posix_time::time_duration duration = datetime.time_of_day();
      return duration.total_milliseconds();
    }

    /*void GetTimeNow()
    {
      using namespace boost::posix_time;
      using namespace boost::gregorian;

      //get the current time from the clock -- one second resolution
      ptime now = second_clock::local_time();

      //Get the date part out of the time
      date today = now.date();
      boost::posix_time::time_duration time = now.time_of_day();

      std::cout << to_simple_string(now) << std::endl;
      std::cout << to_simple_string(today) << std::endl;
      std::cout << to_simple_string(time) << std::endl;
    }*/


    // http://en.wikipedia.org/wiki/ISO_8601
    // Z means UTC zero offset ie. this time is global and to get the local time we then added our local time zone offset
    // 19980717T140855,10Z
    // OR
    // 19980717T140855,10+hhmm
    string_t cDateTime::GetISO8601UTCStringWithTimeZoneOffset(const boost::posix_time::time_duration& offset) const
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
    bool cDateTime::SetFromISO8601UTCString(const string_t& rhs, boost::posix_time::time_duration& offset)
    {
      // Set the offset to zero for the moment
      datetime = boost::posix_time::ptime(boost::gregorian::date(0, boost::gregorian::Jan, 1), boost::posix_time::hours(0));
      offset = boost::posix_time::time_duration(0, 0, 0, 0);

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

      offset = boost::posix_time::time_duration(hours, minutes, 0, 0);
      return true;
    }

    // RFC 3339 Format
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

    boost::posix_time::time_duration cDateTime::GetLocalTimeZoneOffset()
    {
      boost::date_time::c_local_adjustor<boost::posix_time::ptime> local_adj;
      const boost::posix_time::ptime t10(boost::gregorian::date(2002, boost::gregorian::Jan, 1), boost::posix_time::hours(7));
      const boost::posix_time::ptime t11 = local_adj.utc_to_local(t10);

      return t11 - t10;
    }

    bool cDateTime::IsTimeZoneOffsetNonZero(const boost::posix_time::time_duration& offset)
    {
      return offset.total_seconds() != 0;
    }

    void cDateTime::ConvertFromLocalToUTC()
    {
      ASSERT(IsValid());

      //boost::date_time::local_adjustor<boost::posix_time::ptime> local_adj;
      //datetime = local_adj.local_to_utc(datetime);
      ASSERT(false);
    }

    void cDateTime::ConvertFromUTCToLocal()
    {
      ASSERT(IsValid());

      boost::date_time::c_local_adjustor<boost::posix_time::ptime> local_adj;
      datetime = local_adj.utc_to_local(datetime);
    }
  }
}

