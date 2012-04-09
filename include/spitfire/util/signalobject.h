#ifndef SPITFIRE_SIGNALOBJECT_H
#define SPITFIRE_SIGNALOBJECT_H

// Standard headers
#include <condition_variable>
#include <mutex>

// Boost headers
#include <boost/bind/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread_time.hpp>

// Spitfire headers
#include <spitfire/spitfire.h>

namespace spitfire
{
  namespace util
  {
    class cSignalObject
    {
    public:
      explicit cSignalObject(const std::string& sName);

      bool IsSignalled() const;

      void Signal();    // Signals all waiting threads
      void SignalOne(); // Signals only one waiting thread
      void Reset();

      void WaitForever();
      bool WaitTimeoutMS(uint32_t uTimeOutMS);

    private:
      std::string sName;
      mutable std::mutex mutex;
      std::condition_variable condition;
      volatile bool bIsSignalled;
    };

    inline cSignalObject::cSignalObject(const std::string& _sName) :
      sName(_sName),
      bIsSignalled(false)
    {
    }

    inline bool cSignalObject::IsSignalled() const
    {
      std::unique_lock<std::mutex> lock(mutex);

      return bIsSignalled;
    }

    inline void cSignalObject::Signal()
    {
      {
        std::unique_lock<std::mutex> lock(mutex);
        bIsSignalled = true;
      }

      // Mutex must be unlocked when condition variable is notified
      condition.notify_all();
    }

    inline void cSignalObject::SignalOne()
    {
      {
        std::unique_lock<std::mutex> lock(mutex);
        bIsSignalled = true;
      }

      // Mutex must be unlocked when condition variable is notified
      condition.notify_one();
    }

    inline void cSignalObject::Reset()
    {
      std::unique_lock<std::mutex> lock(mutex);
      bIsSignalled = false;
    }

    inline void cSignalObject::WaitForever()
    {
      // In a while loop because pthreads are susceptible to spurious wakes
      // http://stackoverflow.com/questions/6877032/boostcondition-variable-timed-wait-return-immediately
      while (true) {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock);
        if (bIsSignalled) {
          bIsSignalled = false;
          break;
        }
      }
    }

    inline bool cSignalObject::WaitTimeoutMS(uint32_t uTimeOutMS)
    {
      const auto endTime = std::chrono::system_clock::now() + std::chrono::milliseconds(uTimeOutMS);
      bool bResult = false;
      {
        std::unique_lock<std::mutex> lock(mutex);
        bResult = condition.wait_until(lock, endTime, [&]() { return cSignalObject::bIsSignalled; });
        bIsSignalled = false;
      }
      return bResult;
    }
  }
}

#endif // SPITFIRE_SIGNALOBJECT_H
