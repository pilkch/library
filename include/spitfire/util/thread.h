#ifndef CTHREAD_H
#define CTHREAD_H

// Standard headers
#include <thread>

// Boost headers
#include <boost/date_time/posix_time/posix_time.hpp>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/mutex.h>
#include <spitfire/util/process.h>
#include <spitfire/util/signalobject.h>

namespace spitfire
{
  namespace util
  {
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


    // ** cThread

    class cThread : public cProcessInterface
    {
    public:
      explicit cThread(const std::string& sName);
      virtual ~cThread();

      const std::string& GetName() const { return sName; }

      void Run();            // Runs the thread in the background
      void WaitToStop();     // Blocks forever until thread is done
      void StopThreadSoon(); // Tells the thread to stop soon (Does not actually wait for it to stop)

      bool IsRunning();

    private:
      virtual bool _IsToStop() const override;

      static int RunThreadFunction(void* pThis);
      virtual void ThreadFunction() = 0;

      const std::string sName;

      std::thread* pThread;
      cSignalObject soStop;
      cSignalObject soDone;

      NO_COPY(cThread);
    };


    // *** cThread

    inline cThread::cThread(const std::string& _sName) :
      sName(_sName),
      pThread(nullptr),
      soStop("cThread::soStop"),
      soDone("cThread::soDone")
    {
    }

    inline cThread::~cThread()
    {
      WaitToStop();
    }

    inline bool cThread::IsRunning()
    {
      return (pThread != nullptr) && !soDone.IsSignalled();
    }

    inline bool cThread::_IsToStop() const
    {
      return soStop.IsSignalled();
    }

    inline void cThread::WaitToStop()
    {
      if (pThread != nullptr) pThread->join();

      SAFE_DELETE(pThread);
    }

    inline void cThread::StopThreadSoon()
    {
      soStop.Signal();
    }

    // Not the most elegant method, but it works
    inline int cThread::RunThreadFunction(void* pData)
    {
      ASSERT(pData != nullptr);
      cThread* pThis = static_cast<cThread*>(pData);
      ASSERT(pThis != nullptr);
      pThis->ThreadFunction();

      pThis->soDone.Signal();

      // We don't really do this correctly, we just return 0 for every thread
      return 0;
    }
  }
}

#endif // CTHREAD_H
