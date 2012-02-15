#ifndef CPROCESS_H
#define CPROCESS_H

#include <spitfire/util/string.h>
#include <spitfire/util/thread.h>

//
// *** cProcess/cProcessInterface
//
// Usage:
// Refer to cProcess/cProcessInterface unit test for usage
//

namespace spitfire
{
  namespace util
  {
    enum class PROCESS_RESULT {
      COMPLETE,
      FAILED,
      STOPPED_BY_INTERFACE,
    };

    class cProcessInterface;

    class cProcess
    {
    public:
      explicit cProcess(cProcessInterface& interface);
      virtual ~cProcess() {}

      PROCESS_RESULT Run();

    protected:
      cProcessInterface& interface;

    private:
      virtual PROCESS_RESULT ProcessFunction() = 0;
    };




    class cProcessInterface
    {
    public:
      virtual ~cProcessInterface() {}

      bool IsToStop() const { return _IsToStop(); }

      void SetTextPrimary(const string_t& sText) { _SetTextPrimary(sText); }
      void SetTextSecondary(const string_t& sText) { _SetTextSecondary(sText); }

      void SetPercentageCompletePrimary0To100(float_t fPercentageComplete0To100) { _SetPercentageCompletePrimary0To100(fPercentageComplete0To100); }
      void SetPercentageCompleteSecondary0To100(float_t fPercentageComplete0To100) { _SetPercentageCompleteSecondary0To100(fPercentageComplete0To100); }

    private:
      virtual bool _IsToStop() const { return false; }

      virtual void _SetTextPrimary(const string_t& sText) {}
      virtual void _SetTextSecondary(const string_t& sText) {}

      virtual void _SetPercentageCompletePrimary0To100(float_t fPercentageComplete0To100) {}
      virtual void _SetPercentageCompleteSecondary0To100(float_t fPercentageComplete0To100) {}
    };







    class cRunOnMainThreadTask;

    class cRunOnMainThreadQueue
    {
    public:
      static void Create();
      static void Destroy();

      static void UpdateFromMainThread();

      static bool PushTask(cRunOnMainThreadTask* pTask);

    private:
      cRunOnMainThreadQueue();

      void _UpdateFromMainThread();
      bool _PushTask(cRunOnMainThreadTask* pTask);

      static cRunOnMainThreadQueue* pQueue;

      cMutex mutex;
      std::vector<cRunOnMainThreadTask*> tasks;
    };



    class cRunOnMainThreadTask
    {
    public:
      friend class cRunOnMainThreadQueue;

      virtual ~cRunOnMainThreadTask() {}

      bool Run();

    protected:
      void RunFromMainThread();

    private:
      virtual void _Run() = 0;

      cSignalObject soStarted;
      cSignalObject soDone;
    };
  }
}

#endif // CPROCESS_H
