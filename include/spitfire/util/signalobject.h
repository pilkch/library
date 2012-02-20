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

      bool IsSignalled();

      void Signal();
      void Reset();

      void WaitForever();
      bool WaitTimeoutMS(uint32_t uTimeOutMS);

    private:
      std::string sName;
      std::mutex mutex;
      std::condition_variable condition;
      volatile bool bIsSignalled;
    };

    inline cSignalObject::cSignalObject(const std::string& _sName) :
      sName(_sName),
      bIsSignalled(false)
    {
    }

    inline bool cSignalObject::IsSignalled()
    {
      std::unique_lock<std::mutex> lock(mutex);

      return bIsSignalled;
    }

    inline void cSignalObject::Signal()
    {
      std::unique_lock<std::mutex> lock(mutex);
      bIsSignalled = true;
      condition.notify_one();
    }

    inline void cSignalObject::Reset()
    {
      std::unique_lock<std::mutex> lock(mutex);
      bIsSignalled = false;
    }

    inline void cSignalObject::WaitForever()
    {
      std::unique_lock<std::mutex> lock(mutex);
      condition.wait(lock);
    }

    inline bool cSignalObject::WaitTimeoutMS(uint32_t uTimeOutMS)
    {
      const auto endTime = std::chrono::system_clock::now() + std::chrono::milliseconds(uTimeOutMS);
      std::unique_lock<std::mutex> lock(mutex);
      return condition.wait_until(lock, endTime, [&]() { return cSignalObject::bIsSignalled; });
    }
  }
}

#endif // SPITFIRE_SIGNALOBJECT_H
