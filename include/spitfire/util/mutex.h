#ifndef SPITFIRE_MUTEX_H
#define SPITFIRE_MUTEX_H

// Standard headers
#include <mutex>

// Standard headers
#include <chrono>

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/util/string.h>

namespace spitfire
{
  namespace util
  {
    class cLockObject;
    class cSignalObject;

    class cMutex
    {
    public:
      friend class cLockObject;
      friend class cSignalObject;

      explicit cMutex(const string_t& sName);

      const string_t& GetName() const { return sName; }

    private:
      cMutex(const cMutex&) = delete;
      cMutex& operator=(const cMutex&) = delete;

      const string_t sName;
      std::mutex mutex;
    };

    class cLockObject
    {
    public:
      explicit cLockObject(cMutex& mutex);

    private:
      cLockObject(const cLockObject&) = delete;
      cLockObject& operator=(const cLockObject&) = delete;

      std::lock_guard<std::mutex> lock;
    };


    // *** cMutex

    inline cMutex::cMutex(const string_t& _sName) :
      sName(_sName)
    {
    }

    // *** cLockObject

    inline cLockObject::cLockObject(cMutex& _mutex) :
      lock(_mutex.mutex)
    {
    }
  }
}

#endif // SPITFIRE_MUTEX_H
