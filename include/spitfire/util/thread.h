#ifndef CTHREAD_H
#define CTHREAD_H

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
      friend class cSignalObject;

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



    // We should definitely use the boost signals2 library when it is ready, this is inefficient, not pretty code!

    class cSignalObject
    {
    public:
      explicit cSignalObject();

      bool IsSignalled();

      void Signal();
      void Reset();

      void WaitSignalForever();

      bool WaitSignalTimeOutMS(uint32_t uTimeOutMS);

    private:
      cMutex mutex;
      bool bIsSignalled; // NOTE: With a real signalobject we should not have to use this
    };

    inline cSignalObject::cSignalObject() :
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

      const boost::posix_time::ptime start = boost::posix_time::microsec_clock::local_time();

      uint64_t uTimePassed = 0;
      while (uTimePassed <= uTimeOutMS) {
        if (IsSignalled()) return true;

        SleepThisThreadMS(50);
        YieldThisThread();

        boost::posix_time::time_duration timePassed = boost::posix_time::microsec_clock::local_time() - start;

        uTimePassed = timePassed.total_milliseconds();
      }

      return false;
    }
  }
}

#endif // CTHREAD_H
