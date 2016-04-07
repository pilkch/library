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
      const string_t sName;
      std::mutex mutex;

      NO_COPY(cMutex);
    };

    class cLockObject
    {
    public:
      explicit cLockObject(cMutex& mutex);

    private:
      std::lock_guard<std::mutex> lock;

      NO_COPY(cLockObject);
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
