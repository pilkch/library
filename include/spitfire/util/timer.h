#ifndef CTIMER_H
#define CTIMER_H

#include <spitfire/spitfire.h>

#ifdef PLATFORM_LINUX_OR_UNIX
// Standard headers
#include <sys/time.h>
#endif

namespace spitfire
{
  namespace util
  {
  /*
  TODO: Use std::high_resolution_clock
  // http://en.cppreference.com/w/cpp/chrono/high_resolution_clock

  #include <iostream>
  #include <chrono>
  #include <thread>

  // "busy sleep" while suggesting that other threads run
  // for a small amount of time
  void little_sleep(std::chrono::microseconds us)
  {
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + us;
    do {
      std::this_thread::yield();
    } while (std::chrono::high_resolution_clock::now() < end);
  }

  int main()
  {
    auto start = std::chrono::high_resolution_clock::now();

    little_sleep(std::chrono::microseconds(100));

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "waited for "<< std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()<< " microseconds\n";
  }
  */

    class cTimer
    {
    public:
      cTimer();

      void InitWithLockedFPS(unsigned int uiHz);
      void InitWithUnspecifiedFPS();

      void Begin();
      void End();

      // Milliseconds per frame
      inline float GetMPF() const { return fMPF; }

      // Frames per second
      inline float GetFPS() const { return fFPS; }

    private:
      int iCount;
      float fTimeMS;

      float lastTimeMS;
      float beginTimeMS;
      float endTimeMS;

      bool bIsLockedFPS;
      float fFPS;
      float fMPF;
      float fUpdateInterval;
      float fUpdateIntervalDivFPS;
    };


    #ifdef __WIN__
    struct timeval {
      long tv_sec;  // Time interval, in seconds
      long tv_usec; // Time interval, in microseconds
    };
    struct timezone {
      int tz_minuteswest; // Minutes W of Greenwich
      int tz_dsttime;     // Type of daylight savings correction
    };
    // http://social.msdn.microsoft.com/Forums/en-US/vcgeneral/thread/430449b3-f6dd-4e18-84de-eebd26a8d668/
    int gettimeofday(struct timeval* tv, struct timezone* tz);
    #endif

    inline sampletime_t GetTimeMS()
    {
      timeval tvNow;
      gettimeofday(&tvNow, nullptr);
      return (uint64_t(tvNow.tv_sec) * 1000) + (uint64_t(tvNow.tv_usec) / 1000);
    }
  }
}

#endif // CTIMER_H
