#ifndef CTHREAD_H
#define CTHREAD_H

#error "This is now generally deprecated in favour of Spitfire threads which use Boost"

namespace breathe
{
  namespace util
  {
    class cLockObject;

    inline void SleepThisThread(uint32_t milliseconds)
    {
      SDL_Delay(milliseconds);
    }

    inline void YieldThisThread()
    {
#ifdef __WIN__
      ::Sleep(0);
      SDL_Delay(0);
      SDL_Delay(1);
#else
      sched_yield();
#endif
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

      SDL_Thread* thread;

      NO_COPY(cThread);
    };

    class cMutex
    {
    public:
      friend class cLockObject;

      cMutex();
      ~cMutex();

    private:
      SDL_mutex* mutex;

      NO_COPY(cMutex);
    };

    class cLockObject
    {
    public:
      explicit cLockObject(cMutex& mutex);
      ~cLockObject();

    private:
      cMutex& mutex;

      NO_COPY(cLockObject);
    };


    // *** cThread

    inline cThread::cThread() :
      thread(nullptr)
    {
    }

    inline cThread::~cThread()
    {
      if (IsRunning()) WaitUntilFinished();
    }

    inline bool cThread::IsRunning() const
    {
      return (thread != nullptr);
    }

    inline bool cThread::IsFinished() const
    {
      return (thread != nullptr);
    }

    inline void cThread::Run()
    {
      thread = SDL_CreateThread(RunThreadFunction, this);
    }

    inline void cThread::WaitUntilFinished()
    {
      LOG<<"cThread::WaitUntilFinished"<<std::endl;
      ASSERT(thread != nullptr);
      SDL_WaitThread(thread, NULL);
      thread = nullptr;
      LOG<<"cThread::WaitUntilFinished returning"<<std::endl;
    }

    inline void cThread::StopNow()
    {
      LOG<<"cThread::StopNow"<<std::endl;
      ASSERT(thread != nullptr);
      SDL_KillThread(thread);
      thread = nullptr;
      LOG<<"cThread::StopNow returning"<<std::endl;
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

    inline cMutex::cMutex()
    {
      mutex = SDL_CreateMutex();
      ASSERT(mutex != nullptr);
    }

    inline cMutex::~cMutex()
    {
      ASSERT(mutex != nullptr);
      SDL_DestroyMutex(mutex);
    }


    // *** cLockObject

    inline cLockObject::cLockObject(cMutex& _mutex) :
      mutex(_mutex)
    {
      ASSERT(mutex.mutex != nullptr);
      SDL_mutexP(mutex.mutex);
    }

    inline cLockObject::~cLockObject()
    {
      ASSERT(mutex.mutex != nullptr);
      SDL_mutexV(mutex.mutex);
    }
  }
}

#endif // CTHREAD_H
