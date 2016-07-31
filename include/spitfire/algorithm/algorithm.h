#ifndef ALGORITHM_H
#define ALGORITHM_H

// Standard headers
#include <list>
#include <vector>

// Spitfire headers
#include <spitfire/math/math.h>

// In general it would be great if we could get rid of this whole file and just use STL/Boost/TR1 classes only, but
// I haven't found replacements for everything just yet, waiting for C++0x.

namespace spitfire
{
  namespace vector
  {
    // This is just a convenience function (Is there already a way to do this without resorting to .begin()?)
    // I think we can just call insert directly, is this really needed?
    template <class T>
    inline void push_back(std::vector<T>& v, const size_t n, const T& value)
    {
      v.insert(v.end(), n, value);
    }
  }

  namespace algorithm
  {
    // If std::sort would just take userdata we could remove this function
    template <class RandomAccessIterator, class Compare, class UserData>
    void SortWithUserData(RandomAccessIterator first, const RandomAccessIterator last, Compare comp, const UserData& userData)
    {
      RandomAccessIterator current = first;
      RandomAccessIterator next = first;
      next++;

      size_t n = 0;
      {
        RandomAccessIterator count = first;
        while (count != last) {
          n++;

          count++;
        }
      }

      bool bIsSorted = false; // Flag to stop unnecessary passes, flase when swap occurs
      for (size_t pass = 1; (pass < n) && !bIsSorted; ++pass) {
        bIsSorted = true;

        RandomAccessIterator passCurrent = current;
        RandomAccessIterator passNext = next;
        for(; (passCurrent != last) && (passNext != last); passCurrent++, passNext++) {
          // Compare both values and swap if necessary
          if (comp(*passCurrent, *passNext, userData)) {
            std::iter_swap(passCurrent, passNext); // Should this be swap(*passCurrent, *passNext)?
            bIsSorted = false;
          }
        }

        current++;
        next++;
      }
    }
  }


  // ** constant_stack
  //
  // A vector with a constant size
  //
  // TODO: This is a dodgy class, it should either be a ring buffer or a list/double ended stack that doesn't offer random access
  //
  template <class T>
  class constant_stack
  {
  public:
    typedef std::vector<T> container_t;
    typedef typename container_t::iterator iterator;
    typedef typename container_t::reverse_iterator reverse_iterator;

    explicit constant_stack(size_t n);

    void push_back(const T& rhs);

    iterator begin() { return elements.begin(); }
    iterator end() { return elements.end(); }

    reverse_iterator rbegin() { return elements.rbegin(); }
    reverse_iterator rend() { return elements.rend(); }

    size_t size() const;
    bool empty() const;

    T& operator[](size_t i);
    const T& operator[](size_t i) const;

  private:
    container_t elements;
    size_t first;
    size_t n;
  };

  template <class T>
  constant_stack<T>::constant_stack(size_t _n) :
    first(0),
    n(_n)
  {
    elements.reserve(n);
  }

  template <class T>
  void constant_stack<T>::push_back(const T& rhs)
  {
    if (elements.size() < n) elements.push_back(rhs);
    else {
      if (first >= n) first = 0;
      elements[first++] = rhs;
    }
  }

  template <class T>
  size_t constant_stack<T>::size() const
  {
    return elements.size();
  }

  template <class T>
  bool constant_stack<T>::empty() const
  {
    return elements.empty();
  }

  template <class T>
  const T& constant_stack<T>::operator[](size_t i) const
  {
    ASSERT(i < elements.size());
    return elements[(first + i) % n];
  }

  template <class T>
  T& constant_stack<T>::operator[](size_t i)
  {
    ASSERT(i < elements.size());
    return elements[(first + i) % n];
  }


  // Creates a vector that can be indexed into with x, y coordinates as well as normal i indexing
  // The dimensions must be known at compile time
  template <class T, size_t width, size_t height>
  class cContainer2D
  {
  public:
    cContainer2D();

    size_t size() const { return width * height; }

    size_t GetWidth() const { return width; }
    size_t GetHeight() const { return height; }

    T& GetElement(size_t i) { ASSERT(i < elements.size()); return elements[i]; }
    const T& GetElement(size_t i) const { ASSERT(i < elements.size()); return elements[i]; }

    T& GetElement(size_t x, size_t y) { ASSERT(y * width + x < elements.size()); return elements[y * width + x]; }
    const T& GetElement(size_t x, size_t y) const { ASSERT(y * width + x < elements.size()); return elements[y * width + x]; }

    T& operator[](size_t i) { return GetElement(i); }
    const T& operator[](size_t i) const { return GetElement(i); }

  private:
    std::vector<T> elements;
  };


  template<class T, size_t width, size_t height>
  inline cContainer2D<T, width, height>::cContainer2D() :
    elements(width * height)
  {
  }


  // Creates a vector that can be indexed into with x, y coordinates as well as normal i indexing
  // The dimensions are not known at compile time
  template <class T>
  class cDynamicContainer2D
  {
  public:
    cDynamicContainer2D(const cDynamicContainer2D& rhs);
    cDynamicContainer2D(size_t width, size_t height);

    cDynamicContainer2D& operator=(const cDynamicContainer2D& rhs);

    void RemoveAllEntriesAndSetNewSize(size_t width, size_t height);

    size_t size() const { return width * height; }

    size_t GetWidth() const { return width; }
    size_t GetHeight() const { return height; }

    T& GetElement(size_t i) { ASSERT(i < elements.size()); return elements[i]; }
    const T& GetElement(size_t i) const { ASSERT(i < elements.size()); return elements[i]; }

    T& GetElement(size_t x, size_t y) { ASSERT((y * width) + x < elements.size()); return elements[(y * width) + x]; }
    const T& GetElement(size_t x, size_t y) const { ASSERT((y * width) + x < elements.size()); return elements[(y * width) + x]; }

    T& operator[](size_t i) { return GetElement(i); }
    const T& operator[](size_t i) const { return GetElement(i); }

  private:
    size_t width;
    size_t height;
    std::vector<T> elements;
  };


  template<class T>
  inline cDynamicContainer2D<T>::cDynamicContainer2D(size_t _width, size_t _height) :
    width(_width),
    height(_height),
    elements(_width * _height)
  {
  }

  template<class T>
  inline cDynamicContainer2D<T>::cDynamicContainer2D(const cDynamicContainer2D& rhs)
  {
    width = rhs.width;
    height = rhs.height;
    elements = rhs.elements;
  }

  template<class T>
  inline cDynamicContainer2D<T>& cDynamicContainer2D<T>::operator=(const cDynamicContainer2D& rhs)
  {
    width = rhs.width;
    height = rhs.height;
    elements = rhs.elements;

    return *this;
  }

  template<class T>
  void cDynamicContainer2D<T>::RemoveAllEntriesAndSetNewSize(size_t _width, size_t _height)
  {
    width = _width;
    height = _height;
    std::vector<T> replacement(width * height);
    elements = replacement;
    ASSERT(elements.size() == (width * height));
  }


  // ** cRandomBucket
  //
  // For selecting items randomly from a list where each item must occur with a uniform distribution
  //
  template <class T>
  class cRandomBucket
  {
  public:
    // NOTE: Added items will not be returned via GetRandomItem until the possible list has been cleared first
    void AddItem(T value);
    void AddItems(T value, size_t number);

    const std::list<T>& GetPossibleItems() const;

    T GetRandomItem();
    void Clear();

  private:
    std::list<T> original;
    std::list<T> possible;
  };

  template <class T>
  void cRandomBucket<T>::AddItem(T value)
  {
    original.push_back(value);
  }

  template <class T>
  void cRandomBucket<T>::AddItems(T value, size_t number)
  {
    for (size_t i = 0; i < number; i++) original.push_back(value);
  }

  template <class T>
  const std::list<T>& cRandomBucket<T>::GetPossibleItems() const
  {
    return possible;
  }

  template <class T>
  T cRandomBucket<T>::GetRandomItem()
  {
    // If original is empty we have a problem
    assert(!original.empty());

    // If we don't have any more items to select then refresh the list to the original items
    if (possible.empty()) possible = original;

    // Get an index to a random item in the list
    uint32_t index = spitfire::math::random(int(possible.size()));

    typename std::list<T>::iterator iter = possible.begin();
    const typename std::list<T>::iterator iterEnd = possible.end();

    // Iterate through to index
    std::advance(iter, index);

    // Make sure we are not past the end of the list
    assert(iter != iterEnd);

    // Get the item
    T item = *iter;

    // Remove the item
    possible.erase(iter);

    return item;
  }

  template <class T>
  void cRandomBucket<T>::Clear()
  {
    original.clear();
    possible.clear();
  }


  namespace algorithm
  {
    // Conversion from 12 to 0x12 for example
    inline uint8_t ToBinaryCodedDecimal(uint8_t uValue)
    {
      return ((uValue / 10) << 4) | (uValue % 10);
    }

    // Conversion from 0x12 to 12 for example
    inline uint8_t FromBinaryCodedDecimal(uint8_t uBCD)
    {
      return ((uBCD >> 4) * 10) + (uBCD & 0x0F);
    }
  }
}

#endif // ALGORITHM_H
