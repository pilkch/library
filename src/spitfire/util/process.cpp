#include <iostream>
#include <fstream>

#include <vector>
#include <map>
#include <stack>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/string.h>
#include <spitfire/util/log.h>
#include <spitfire/util/process.h>
#include <spitfire/util/thread.h>

namespace spitfire
{
  namespace util
  {
    // *** cRunOnMainThreadQueue

    //cRunOnMainThreadQueue* cRunOnMainThreadQueue::pQueue = nullptr;



    // *** cProcess

    cProcess::cProcess(cProcessInterface& _interface) :
      interface(_interface)
    {
    }

    PROCESS_RESULT cProcess::Run()
    {
      interface.SetPercentageCompletePrimary0To100(0.0f);

      return ProcessFunction();
    }







    /*// *** cRunOnMainThreadQueue

    cRunOnMainThreadQueue::cRunOnMainThreadQueue() :
      soAction("cRunOnMainThreadQueue::soAction"),
      tasks(soAction)
    {
    }

    void cRunOnMainThreadQueue::Create()
    {
      ASSERT(pQueue == nullptr);
      pQueue = new cRunOnMainThreadQueue;
    }

    void cRunOnMainThreadQueue::Destroy()
    {
      SAFE_DELETE(pQueue);
    }

    void cRunOnMainThreadQueue::UpdateFromMainThread()
    {
      ASSERT(IsMainThread());

      ASSERT(pQueue != nullptr);
      pQueue->_UpdateFromMainThread();
    }

    void cRunOnMainThreadQueue::_UpdateFromMainThread()
    {
      cLockObject lock(mutex);

      const size_t n = tasks.size();
      for (size_t i = 0; i < n; i++) {
        cRunOnMainThreadTask* pTask = tasks[i];
        ASSERT(pTask != nullptr);
        pTask->RunFromMainThread();
      }

      tasks.clear();
    }

    bool cRunOnMainThreadQueue::PushTask(cRunOnMainThreadTask* pTask)
    {
      ASSERT(!IsMainThread());

      if (pQueue == nullptr) return false;

      return pQueue->_PushTask(pTask);
    }

    bool cRunOnMainThreadQueue::_PushTask(cRunOnMainThreadTask* pTask)
    {
      cLockObject lock(mutex);
      tasks.push_back(pTask);

      return true;
    }


    // *** cRunOnMainThreadTask

    cRunOnMainThreadTask::cRunOnMainThreadTask() :
      soStarted("cRunOnMainThreadTask::soStarted"),
      soDone("cRunOnMainThreadTask::soDone")
    {
    }

    bool cRunOnMainThreadTask::Run()
    {
      soDone.Reset();
      soStarted.Reset();

      cRunOnMainThreadQueue::PushTask(this);

      // Wait a short while to see if main is going to run our thread
      if (!soStarted.WaitSignalTimeOutMS(1000)) {
        LOG<<"cRunOnMainThreadTask::Run Timed out waiting for soStart"<<std::endl;
        return false;
      }

      // Ok, main is currently running our function we shall wait forever for it to stop
      soDone.WaitSignalForever();

      return true;
    }

    void cRunOnMainThreadTask::RunFromMainThread()
    {
      ASSERT(spitfire::util::IsMainThread());

      soStarted.Signal();

      _Run();

      soDone.Signal();
    }*/
  }
}
