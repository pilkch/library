#ifndef SPITFIRE_MUTEX_H
#define SPITFIRE_MUTEX_H

// Standard headers
#include <mutex>

// Boost headers
#include <boost/date_time/posix_time/posix_time.hpp>

// Spitfire headers
#include <spitfire/spitfire.h>

namespace spitfire
{
  namespace util
  {
    class cLockObject;

    class cMutex
    {
    public:
      friend class cLockObject;
      friend class cSignalObject;

      explicit cMutex(const std::string& sName);

      const std::string& GetName() const { return sName; }

    private:
      const std::string sName;
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

    inline cMutex::cMutex(const std::string& _sName) :
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
