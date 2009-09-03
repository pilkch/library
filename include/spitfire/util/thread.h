#ifndef CTHREAD_H
#define CTHREAD_H

namespace spitfire
{
  namespace util
  {
    class cLockObject;

    void SetMainThread();
    bool IsMainThread();

    inline void SleepThisThread(uint32_t milliseconds)
    {
      boost::this_thread::sleep(boost::posix_time::milliseconds(milliseconds));
    }

    inline void YieldThisThread()
    {
      boost::this_thread::yield();
    }

    class cThread
    {
    public:
      cThread();
      virtual ~cThread();

      void Run();
      void WaitUntilFinished(); // Safe, blocks forever until thread is done
      void StopNow(); // Unsafe, kills thread instantly and returns

      bool IsRunning() const;
      bool IsFinished() const;

    private:
      static int RunThreadFunction(void* pThis);
      virtual void ThreadFunction() = 0;

      boost::thread* pThread;

      NO_COPY(cThread);
    };

    class cMutex
    {
    public:
      friend class cLockObject;

      cMutex() {}
      ~cMutex() {}

    private:
      boost::mutex mutex;

      NO_COPY(cMutex);
    };

    class cLockObject
    {
    public:
      explicit cLockObject(cMutex& mutex);

    private:
      boost::mutex::scoped_lock lock;

      NO_COPY(cLockObject);
    };


    // *** cThread

    inline cThread::cThread() :
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
      return (pThread != nullptr);
    }

    inline void cThread::WaitUntilFinished()
    {
      ASSERT(pThread != nullptr);
      pThread->join();
      SAFE_DELETE(pThread);
    }

    inline void cThread::StopNow()
    {
      ASSERT(pThread != nullptr);
      pThread->interrupt();
      pThread->join();
      SAFE_DELETE(pThread);
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


    // *** cLockObject

    inline cLockObject::cLockObject(cMutex& _mutex) :
      lock(_mutex.mutex)
    {
    }



#if 0
    // Do we even need signal objects?  Can't we simulate them with a mutex, a lock and a bit of data (bool bIsSignalled)?

    // http://www.boost.org/doc/libs/1_40_0/doc/html/thread/synchronization.html#thread.synchronization.condvar_ref

    class cSignalObject
    {
    public:
      explicit cSignalObject(cMutex& mutex);

      bool IsSignalled();

      void Signal();
      void Reset();

      void WaitSignalForever();

      bool WaitSignalTimeOut(uint32_t uTimeOutMS);

    private:
      cMutex& mutex;

      boost::condition_variable condition;
    };

    cSignalObject::cSignalObject(cMutex& _mutex) :
      mutex(_mutex)
    {
    }

    bool cSignalObject::IsSignalled()
    {
      return WaitSignalTimeOut(0);
    }

    void cSignalObject::Signal()
    {
      condition.notify_all();
    }

    void cSignalObject::WaitSignalForever()
    {
        boost::unique_lock<boost::mutex> lock(mut);

        cond.wait(lock);
    }

    void cSignalObject::WaitSignalTimeOut(uint32_t uTimeOutMS)
    {
      boost::unique_lock<boost::mutex> lock(mut);

      cond.timed_wait(lock, boost::duration_type const& rel_time);
    }
#endif
  }
}

#endif // CTHREAD_H
