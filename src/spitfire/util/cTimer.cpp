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
      lastTime(0),
      beginTime(0),
      endTime(0),

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

      lastTime = GetTime();
      beginTime = lastTime;
      endTime = lastTime;
    }

    void cTimer::InitWithUnspecifiedFPS()
    {
      iCount = 0;

      bIsLockedFPS = false;
      fFPS = 0.0f;
      fMPF = 0.0f;
      fUpdateInterval = 0.0f;
      fUpdateIntervalDivFPS = 0.0f;

      lastTime = GetTime();
      beginTime = lastTime;
      endTime = lastTime;
    }


    void cTimer::Begin(sampletime_t currentTime)
    {
      beginTime = currentTime;
    }

    void cTimer::End(sampletime_t currentTime)
    {
      endTime = currentTime;


      iCount++;
      fTimeMS += float(endTime - beginTime);

      if (bIsLockedFPS) {
        // Update only every interval
        if ((currentTime - lastTime) > fUpdateInterval) {
          fFPS = (iCount * ((currentTime - lastTime) * fUpdateIntervalDivFPS));
          fMPF = endTime - beginTime;
          lastTime = currentTime;
          iCount = 0;
        }
      } else {
        // Update every single call
        fFPS = 1000.0f / (currentTime - lastTime);
        fMPF = endTime - beginTime;
        lastTime = currentTime;
        iCount = 0;
      }
    }
  }
}
