// Standard headers
#include <cmath>
#include <chrono>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/timer.h>
#include <spitfire/util/thread.h>

namespace spitfire
{
  namespace util
  {
    bool bTimeInitCalled = false;
    std::chrono::system_clock::time_point timeInitCalled;

    void TimeInit()
    {
      ASSERT(IsMainThread());
      timeInitCalled = std::chrono::system_clock::now();
      bTimeInitCalled = true;
    }

    durationms_t GetTimeMS()
    {
      ASSERT(bTimeInitCalled);
      std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
      return std::chrono::duration_cast<std::chrono::milliseconds>(now - timeInitCalled).count();
    }


    const float fOneOverOneThousand = 1.0f / 1000.0f;

    cFPSTimer::cFPSTimer() :
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

    void cFPSTimer::InitWithLockedFPS(unsigned int uiHz)
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

    void cFPSTimer::InitWithUnspecifiedFPS()
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


    void cFPSTimer::Begin()
    {
      beginTimeMS = float(GetTimeMS());
    }

    void cFPSTimer::End()
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
  }
}
