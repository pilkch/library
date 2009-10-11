#ifndef CTIMER_H
#define CTIMER_H

namespace spitfire
{
  namespace util
  {
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

    inline sampletime_t GetTimeMS()
    {
      // Return the milliseconds since we started
      return SDL_GetTicks();
    }
  }
}

#endif // CTIMER_H
