#ifndef CTHREAD_H
#define CTHREAD_H

// Standard headers
#include <thread>
#include <mutex>

// Boost headers
#include <boost/date_time/posix_time/posix_time.hpp>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/mutex.h>
#include <spitfire/util/signalobject.h>

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
  }
}

#endif // CTHREAD_H
