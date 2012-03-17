#ifndef CQUEUE_H
#define CQUEUE_H

// Standard headers
#include <list>

// Spitfire headers
#include <spitfire/util/thread.h>

namespace spitfire
{
  namespace util
  {
    // ** cQueue
    // A general purpose queue typically used between a producer and consumer
    template <class T>
    class cQueue
    {
    public:
      ~cQueue();

      bool IsEmpty() const { return items.empty(); }
      size_t GetSize() const { return items.size(); }

      void AddItemToBack(T* pItem); // The queue takes ownership of pItem
      T* RemoveItemFromFront();     // The caller takes ownership of the item that is returned

    private:
      std::list<T*> items;
    };

    template <class T>
    inline cQueue<T>::~cQueue()
    {
      // NOTE: All items must be removed before the queue is destroyed
      ASSERT(items.empty());
    }

    template <class T>
    inline void cQueue<T>::AddItemToBack(T* pItem)
    {
      items.push_back(pItem);
    }

    template <class T>
    inline T* cQueue<T>::RemoveItemFromFront()
    {
      ASSERT(!items.empty());
      return items.pop_front();
    }


    // ** cThreadSafeQueue
    // Exactly the same as cQueue except that the items are protected with a mutex
    template <class T>
    class cThreadSafeQueue
    {
    public:
      explicit cThreadSafeQueue(cSignalObject& soAction);
      ~cThreadSafeQueue();

      // NOTE: No IsEmpty or GetSize functions because by the time you call the next function this may have changed

      void AddItemToBack(T* pItem); // The queue takes ownership of pItem
      T* RemoveItemFromFront(); // The caller takes ownership of the item that is returned

    private:
      cSignalObject& soAction;

      cMutex mutex;
      std::list<T*> items;
    };

    template <class T>
    inline cThreadSafeQueue<T>::cThreadSafeQueue(cSignalObject& _soAction) :
      soAction(_soAction),
      mutex("cThreadSafeQueue<T>::mutex")
    {
    }

    template <class T>
    inline cThreadSafeQueue<T>::~cThreadSafeQueue()
    {
      cLockObject lock(mutex);
      // NOTE: All items must be removed before the queue is destroyed
      ASSERT(items.empty());
    }

    template <class T>
    inline void cThreadSafeQueue<T>::AddItemToBack(T* pItem)
    {
      {
        cLockObject lock(mutex);
        items.push_back(pItem);
      }

      // Tell anyone listening that something happened
      soAction.Signal();
    }

    template <class T>
    inline T* cThreadSafeQueue<T>::RemoveItemFromFront()
    {
      T* pItem = nullptr;
      {
        cLockObject lock(mutex);
        if (!items.empty()) {
          pItem = items.front();
          items.pop_front();
        }
      }
      return pItem;
    }
  }
}

#endif // CQUEUE_H
