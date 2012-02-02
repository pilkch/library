#ifndef CTHREAD_H
#define CTHREAD_H

// Standard headers
#include <thread>
#include <mutex>

// Boost headers
#include <boost/date_time/posix_time/posix_time.hpp>

// Spitfire headers
#include <spitfire/spitfire.h>

namespace spitfire
{
  namespace util
  {
    class cLockObject;
    class cSignalObject;

    void SetMainThread();
    bool IsMainThread();

    inline void SleepThisThreadMS(uint32_t milliseconds)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    inline void YieldThisThread()
    {
      std::this_thread::yield();
    }

    class cThread
    {
    public:
      explicit cThread(const std::string& sName);
      virtual ~cThread();

      const std::string& GetName() const { return sName; }

      void Run();
      void WaitUntilFinished(); // Safe, blocks forever until thread is done
      void StopNow(); // Unsafe, kills thread instantly and returns

      bool IsRunning() const;
      bool IsFinished() const;

    private:
      static int RunThreadFunction(void* pThis);
      virtual void ThreadFunction() = 0;

      const std::string sName;
      std::thread* pThread;

      NO_COPY(cThread);
    };

    class cMutex
    {
    public:
      friend class cLockObject;
      friend class cSignalObject;

      explicit cMutex(const std::string& sName);

      const std::string& GetName() const { return sName; }

    private:
      const std::string sName;
      std::mutex mutex;

      NO_COPY(cMutex);
    };

    class cLockObject
    {
    public:
      explicit cLockObject(cMutex& mutex);

    private:
      std::lock_guard<std::mutex> lock;

      NO_COPY(cLockObject);
    };


    // *** cThread

    inline cThread::cThread(const std::string& _sName) :
      sName(_sName),
      pThread(nullptr)
    {
    }

    inline cThread::~cThread()
    {
      if (IsRunning()) WaitUntilFinished();
    }

    inline bool cThread::IsRunning() const
    {
      return (pThread != nullptr);
    }

    inline bool cThread::IsFinished() const
    {
      return (pThread == nullptr);
    }

    inline void cThread::WaitUntilFinished()
    {
      ASSERT(pThread != nullptr);
      pThread->join();
      SAFE_DELETE(pThread);
    }

    inline void cThread::StopNow()
    {
      WaitUntilFinished();
    }

    // Not the most elegant method, but it works
    inline int cThread::RunThreadFunction(void* pData)
    {
      ASSERT(pData != nullptr);
      cThread* pThis = static_cast<cThread*>(pData);
      ASSERT(pThis != nullptr);
      pThis->ThreadFunction();

      // We don't really do this correctly, we just return 0 for every thread
      return 0;
    }


    // *** cMutex

    inline cMutex::cMutex(const std::string& _sName) :
      sName(_sName)
    {
    }

    // *** cLockObject

    inline cLockObject::cLockObject(cMutex& _mutex) :
      lock(_mutex.mutex)
    {
    }



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

#endif // CTHREAD_H
