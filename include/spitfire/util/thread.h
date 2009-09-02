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
  }
}

#endif // CTHREAD_H
