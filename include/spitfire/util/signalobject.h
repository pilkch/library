#ifndef SPITFIRE_SIGNALOBJECT_H
#define SPITFIRE_SIGNALOBJECT_H

// Standard headers
#include <mutex>

// Boost headers
#include <boost/date_time/posix_time/posix_time.hpp>

// Spitfire headers
#include <spitfire/spitfire.h>

namespace spitfire
{
  namespace util
  {
    // We should definitely use the boost signals2 library when it is ready, this is inefficient, not pretty code!

    class cSignalObject
    {
    public:
      explicit cSignalObject(const std::string& sName);

      const std::string& GetName() const { return sName; }

      bool IsSignalled();

      void Signal();
      void Reset();

      void WaitSignalForever();

      bool WaitSignalTimeOutMS(uint32_t uTimeOutMS);

    private:
      cMutex mutex;
      const std::string sName;
      bool bIsSignalled; // NOTE: With a real signalobject we should not have to use this
    };

    inline cSignalObject::cSignalObject(const std::string& _sName) :
      mutex(_sName + "_mutex"),
      sName(_sName),
      bIsSignalled(false)
    {
    }

    inline bool cSignalObject::IsSignalled()
    {
      cLockObject lock(mutex);

      return bIsSignalled;
    }

    inline void cSignalObject::Signal()
    {
      cLockObject lock(mutex);

      bIsSignalled = true;
    }

    inline void cSignalObject::Reset()
    {
      cLockObject lock(mutex);

      bIsSignalled = false;
    }

    inline void cSignalObject::WaitSignalForever()
    {
      WaitSignalTimeOutMS(0xFFFFFFFF);
    }

    // This is our ghetto signal waiting mechanism, it is probably vulnerable to deadlocks and all sorts of other nasty stuff
    inline bool cSignalObject::WaitSignalTimeOutMS(uint32_t uTimeOutMS)
    {
      // Early exit if we are already signalled
      if (IsSignalled()) return true;

      const std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

      uint64_t uTimePassed = 0;
      while (uTimePassed <= uTimeOutMS) {
        if (IsSignalled()) return true;

        SleepThisThreadMS(50);
        YieldThisThread();

        const std::chrono::system_clock::duration timePassed = (std::chrono::system_clock::now() - start);

        uTimePassed = timePassed.count();
      }

      return false;
    }
  }
}

#endif // SPITFIRE_SIGNALOBJECT_H
