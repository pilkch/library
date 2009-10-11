#include <cmath>

#include <SDL/SDL.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cTimer.h>

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
  }
}
