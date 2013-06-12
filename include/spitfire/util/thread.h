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
      cThread(cSignalObject& soAction, const std::string& sName);
      virtual ~cThread();

      const std::string& GetName() const { return sName; }

      void Run();            // Runs the thread in the background
      void WaitToStop();     // Blocks forever until thread is done
      void StopThreadSoon(); // Tells the thread to stop soon (Does not actually wait for it to stop)
      void StopThreadNow();  // Tells the thread to stop soon and waits until it stops

      bool IsRunning() const;
      bool IsToStop() const { return _IsToStop(); }

    protected:
      void Yield();

    private:
      virtual bool _IsToStop() const override;

      static int RunThreadFunction(void* pThis);
      virtual void ThreadFunction() = 0;

      cSignalObject& soAction; // Signals that something has happened (Either stop, or done, or something else has triggered action)

      const std::string sName;

      std::thread* pThread;
      cSignalObject soStop;    // Signals that the thread should stop
      cSignalObject soDone;    // Signals that the thread has finished

      NO_COPY(cThread);
    };


    // *** cThread

    inline cThread::cThread(cSignalObject& _soAction, const std::string& _sName) :
      soAction(_soAction),
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

    inline bool cThread::IsRunning() const
    {
      return (pThread != nullptr) && !soDone.IsSignalled();
    }

    inline bool cThread::_IsToStop() const
    {
      return soStop.IsSignalled();
    }

    inline void cThread::WaitToStop()
    {
      // Wait for the thread to stop
      if (pThread != nullptr) pThread->join();

      SAFE_DELETE(pThread);
    }

    inline void cThread::StopThreadSoon()
    {
      // Tell the thread to stop soon
      soStop.Signal();

      // Tell the thread that something has happened
      soAction.Signal();
    }

    inline void cThread::StopThreadNow()
    {
      // Tell the thread to stop
      soStop.Signal();

      // Tell the thread that something has happened
      soAction.Signal();

      // Wait for the thread to stop
      WaitToStop();
    }

    inline void cThread::Yield()
    {
      YieldThisThread();
    }
  }
}

#endif // CTHREAD_H
