#ifndef ALGORITHM_H
#define ALGORITHM_H

namespace breathe
{
  namespace vector
  {
    template <class T>
    inline void push_back(std::vector<T>& v, const size_t n, const T& value)
    {
      v.insert(v.begin(), n, value);
      //v.reserve(n);
      //for (size_t i = 0; i != n; i++) v.push_back(value);
    }
  }


  template <class T>
  class constant_stack
  {
  public:
    typedef std::vector<T> container_t;
    typedef typename container_t::iterator iterator;
    typedef typename container_t::reverse_iterator reverse_iterator;

    constant_stack(size_t n);

    void push_back(const T& rhs);

    iterator begin() { return elements.begin(); }
    iterator end() { return elements.end(); }

    reverse_iterator rbegin() { return elements.rbegin(); }
    reverse_iterator rend() { return elements.rend(); }

    size_t size() const;
    bool empty() const;

    T& operator[](size_t i);

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
    else
    {
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
  T& constant_stack<T>::operator[](size_t i)
  {
    assert(i < elements.size());
    return elements[(first + i) % n];
  }


  template <class T, size_t width, size_t height>
  class cContainer2D
  {
  public:
    cContainer2D();

    size_t size() const { return width * height; }

    size_t GetWidth() const { return width; }
    size_t GetHeight() const { return height; }

    T& GetElement(size_t i) { ASSERT(i < elements.size()); return elements[i]; }
    T& GetElement(size_t x, size_t y) { ASSERT(y * width + x < elements.size()); return elements[y * width + x]; }

    const T& GetElement(size_t i) const { ASSERT(i < elements.size()); return elements[i]; }
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


  template <class T>
  class cDynamicContainer2D
  {
  public:
    cDynamicContainer2D(const cDynamicContainer2D& rhs);
    cDynamicContainer2D(size_t width, size_t height);

    cDynamicContainer2D& operator=(const cDynamicContainer2D& rhs);

    size_t size() const { return width * height; }

    size_t GetWidth() const { return width; }
    size_t GetHeight() const { return height; }

    T& GetElement(size_t i) { ASSERT(i < elements.size()); return elements[i]; }
    T& GetElement(size_t x, size_t y) { ASSERT(y * width + x < elements.size()); return elements[y * width + x]; }

    const T& GetElement(size_t i) const { ASSERT(i < elements.size()); return elements[i]; }
    const T& GetElement(size_t x, size_t y) const { ASSERT(y * width + x < elements.size()); return elements[y * width + x]; }

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
  class cCircularBuffer
  {
  public:
    cCircularBuffer();

    void push_back(T& t);
    size_t size() const { return data.size(); }

    T& operator[](size_t i);

  private:
    size_t at;
    std::vector<T> data;
  };

  template<class T>
  inline cCircularBuffer<T>::cCircularBuffer() :
    at(0)
  {
  }

  template<class T>
  inline T& cCircularBuffer<T>::operator[](size_t i)
  {
    const size_t n = (at + i) % data.size();
    return data[n];
  }
}

#endif // ALGORITHM_H
