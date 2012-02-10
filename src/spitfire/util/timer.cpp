#include <cmath>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/timer.h>

namespace spitfire
{
  namespace util
  {
    const float fOneOverOneThousand = 1.0f / 1000.0f;

    cTimer::cTimer() :
      iCount(0),
      lastTimeMS(0.0f),
      beginTimeMS(0.0f),
      endTimeMS(0.0f),

      bIsLockedFPS(true),
      fFPS(0.0f),
      fMPF(0.0f),
      fUpdateInterval(0.0f),
      fUpdateIntervalDivFPS(0.0f)
    {
    }

    void cTimer::InitWithLockedFPS(unsigned int uiHz)
    {
      iCount = 0;

      fFPS = float_t(uiHz);
      fMPF = 1000.0f / fFPS;
      fUpdateInterval = 100.0f;
      fUpdateIntervalDivFPS = fUpdateInterval / 1000.0f;

      lastTimeMS = GetTimeMS();
      beginTimeMS = lastTimeMS;
      endTimeMS = lastTimeMS;
    }

    void cTimer::InitWithUnspecifiedFPS()
    {
      iCount = 0;

      bIsLockedFPS = false;
      fFPS = 0.0f;
      fMPF = 0.0f;
      fUpdateInterval = 0.0f;
      fUpdateIntervalDivFPS = 0.0f;

      lastTimeMS = GetTimeMS();
      beginTimeMS = lastTimeMS;
      endTimeMS = lastTimeMS;
    }


    void cTimer::Begin()
    {
      beginTimeMS = float(GetTimeMS());
    }

    void cTimer::End()
    {
      const float currentTime = float(GetTimeMS());

      endTimeMS = currentTime;


      iCount++;
      fTimeMS += endTimeMS - beginTimeMS;

      if (bIsLockedFPS) {
        // Update only every interval
        if ((currentTime - lastTimeMS) > fUpdateInterval) {
          fFPS = (iCount * ((currentTime - lastTimeMS) * fUpdateIntervalDivFPS));
          fMPF = endTimeMS - beginTimeMS;
          lastTimeMS = currentTime;
          iCount = 0;
        }
      } else {
        // Update every single call
        fFPS = 1000.0f / (currentTime - lastTimeMS);
        fMPF = endTimeMS - beginTimeMS;
        lastTimeMS = currentTime;
        iCount = 0;
      }
    }


    #ifdef __WIN__
    // http://social.msdn.microsoft.com/Forums/en-US/vcgeneral/thread/430449b3-f6dd-4e18-84de-eebd26a8d668/
    int gettimeofday(struct timeval* tv, struct timezone* tz)
    {
      FILETIME ft;
      unsigned int64_t tmpres = 0;

      if (tv != nullptr) {
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        // Convert file time to unix epoch
        tmpres /= 10;  // Convert to microseconds
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = long(tmpres / 1000000UL);
        tv->tv_usec = long(tmpres % 1000000UL);
      }

      if (tz != nullptr) {
        static bool bHasSetTimeZone = false;
        if (!bHasSetTimeZone) {
          _tzset();
          bHasSetTimeZone = true;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
      }

      return 0;
    }
    #endif
  }
}
