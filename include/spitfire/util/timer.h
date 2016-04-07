#ifndef CTIMER_H
#define CTIMER_H

#include <spitfire/spitfire.h>

namespace spitfire
{
  namespace util
  {
    // Set up data for time functions
    void TimeInit();

    // Get the time since epoch in milliseconds
    durationms_t GetTimeMS();

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
    std::cout << "waited for "<< std::chrono::duration_cast<std::chrono::duration<int, std::micro>>(elapsed).count()<< " microseconds\n";
  }
  */

    // ** cTimeOut
    //
    // For use in a loop for example to work out 

    class cTimeOut {
    public:
      explicit cTimeOut(durationms_t uTimeOutMS);

      bool IsExpired() const;

      durationms_t GetRemainingMS() const;

    private:
      const durationms_t uEndTimeMS;
    };


    // ** cTimer
    //
    // For calculating the frames per second and timing how long each frame takes to render
    // TODO: Move this into breathe
    class cFPSTimer
    {
    public:
      cFPSTimer();

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


    // ** cTimeOut

    inline cTimeOut::cTimeOut(durationms_t uTimeOutMS) :
      uEndTimeMS(GetTimeMS() + uTimeOutMS)
    {
    }

    inline bool cTimeOut::IsExpired() const
    {
      return (GetTimeMS() > uEndTimeMS);
    }

    inline durationms_t cTimeOut::GetRemainingMS() const
    {
      const int64_t iRemainingMS = int64_t(uEndTimeMS) - int64_t(GetTimeMS());
      return (iRemainingMS >= 0) ? iRemainingMS : 0;
    }
  }
}

#endif // CTIMER_H
