#ifndef CTHREAD_H
#define CTHREAD_H

namespace breathe
{
  namespace util
  {
    class cLockObject;

    inline void PauseThisThread(uint32_t milliseconds)
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
      void Wait();
      void StopNow();

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
      void Lock();
      void Unlock();

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
      thread(NULL)
    {
    }

    inline cThread::~cThread()
    {
      if (IsRunning()) StopNow();
    }

    inline bool cThread::IsRunning() const
    {
      return thread != NULL;
    }

    inline bool cThread::IsFinished() const
    {
      return thread != NULL;
    }

    inline void cThread::Run()
    {
      thread = SDL_CreateThread(RunThreadFunction, this);
    }

    inline void cThread::Wait()
    {
      ASSERT(thread != nullptr);
      SDL_WaitThread(thread, NULL);
      thread = NULL;
    }

    inline void cThread::StopNow()
    {
      ASSERT(thread != nullptr);
      SDL_KillThread(thread);
      thread = NULL;
    }

    // Not the most elegant method, but it works
    inline int cThread::RunThreadFunction(void* pData)
    {
      ASSERT(pData != nullptr);
      cThread* pThis = static_cast<cThread*>(pData);
      ASSERT(pThis != nullptr);
      pThis->ThreadFunction();

      // We don't really do this accurately, we just return 0 for every thread
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

    inline void cMutex::Lock()
    {
      ASSERT(mutex != nullptr);
      SDL_mutexP(mutex);
    }

    inline void cMutex::Unlock()
    {
      ASSERT(mutex != nullptr);
      SDL_mutexV(mutex);
    }


    // *** cLockObject

    inline cLockObject::cLockObject(cMutex& _mutex) :
      mutex(_mutex)
    {
      mutex.Lock();
    }

    inline cLockObject::~cLockObject()
    {
      mutex.Unlock();
    }
  }
}

#endif // CTHREAD_H
