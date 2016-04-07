#include <iostream>
#include <fstream>

#include <vector>
#include <map>
#include <stack>

// Boost headers
#include <boost/bind.hpp>

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

/*
#ifdef BUILD_DEBUG

#include <spitfire/util/string.h>
#include <spitfire/util/log.h>
#include <spitfire/util/unittest.h>

class MyProcess : public spitfire::util::cProcess
{
public:
  explicit MyProcess(spitfire::util::cProcessInterface& interface);

private:
  spitfire::util::PROCESS_RESULT ProcessFunction();

  bool DoStuff(size_t i);
};

MyProcess::MyProcess(spitfire::util::cProcessInterface& _interface) :
  spitfire::util::cProcess(_interface)
{
}

spitfire::util::PROCESS_RESULT MyProcess::ProcessFunction()
{
  float_t fPercentageCompletePrimary0To100 = 0.0f;

  const size_t n = 10;
  const float_t fDelta = 100.0f / float_t(n);

  for (size_t i = 0; i < n; i++) {
    bool bIsInterfaceWantingToStop = interface.IsToStop();
    if (bIsInterfaceWantingToStop) return spitfire::util::PROCESS_RESULT::STOPPED_BY_INTERFACE;

    if (!DoStuff(i)) return spitfire::util::PROCESS_RESULT::FAILED;

    fPercentageCompletePrimary0To100 += fDelta;
    interface.SetPercentageCompletePrimary0To100(fPercentageCompletePrimary0To100);
  }

  interface.SetPercentageCompletePrimary0To100(100.0f);

  return spitfire::util::PROCESS_RESULT::COMPLETE;
}

bool MyProcess::DoStuff(size_t i)
{
  std::cout<<"MyProcess::DoStuff i="<<i<<std::endl;

  return true;
}




class MyProcessInterface : public spitfire::util::cProcessInterface
{
public:
  MyProcessInterface();

private:
  bool _IsToStop() const { return bIsToStop; }

  void _SetTextPrimary(const spitfire::string_t& sText) { std::cout<<"Text Primary: "<<spitfire::string::ToUTF8(sText).c_str()<<std::endl; }
  void _SetTextSecondary(const spitfire::string_t& sText) { std::cout<<"Text Secondary: "<<spitfire::string::ToUTF8(sText).c_str()<<std::endl; }

  void _SetPercentageCompletePrimary0To100(float_t fPercentageComplete0To100) { std::cout<<"Percentage Primary: "<<fPercentageComplete0To100<<std::endl; }
  void _SetPercentageCompleteSecondary0To100(float_t fPercentageComplete0To100) { std::cout<<"Percentage Primary: "<<fPercentageComplete0To100<<std::endl; }

  bool bIsToStop;
};

MyProcessInterface::MyProcessInterface() :
  bIsToStop(false)
{
}


class cProcessUnitTest : protected spitfire::util::cUnitTestBase
{
public:
  cProcessUnitTest() :
    cUnitTestBase(TEXT("cProcessUnitTest"))
  {
  }

  void Test()
  {
    MyProcessInterface interface;

    MyProcess process(interface);

    spitfire::util::PROCESS_RESULT result = process.Run();

    if (result == spitfire::util::PROCESS_RESULT::COMPLETE) std::cout<<"UnitTest process returned PROCESS_RESULT::COMPLETE"<<std::endl;
    else if (result == spitfire::util::PROCESS_RESULT::FAILED) std::cout<<"UnitTest process returned PROCESS_RESULT::FAILED"<<std::endl;
    else if (result == spitfire::util::PROCESS_RESULT::STOPPED_BY_INTERFACE) std::cout<<"UnitTest process returned PROCESS_RESULT::STOPPED_BY_INTERFACE"<<std::endl;
  }
};

cProcessUnitTest gProcessUnitTest;

#endif // BUILD_DEBUG
*/
