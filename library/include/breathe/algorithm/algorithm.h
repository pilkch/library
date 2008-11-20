#ifndef ALGORITHM_H
#define ALGORITHM_H

namespace breathe
{
  namespace vector
  {
    template <class T>
    inline void push_back(std::vector<T>& v, const size_t n, const T& value)
    {
      v.reserve(n);
      for (size_t i = 0; i != n; i++) v.push_back(value);
    }
  }

  template <class T>
  class cContainer2D
  {
  public:
    cContainer2D(size_t width, size_t height);

    size_t size() const { return width * height; }

    size_t GetWidth() const { return width; }
    size_t GetHeight() const { return height; }

    T& GetElement(size_t i) { ASSERT(i < elements.size()); return elements[i]; }
    T& GetElement(size_t x, size_t y) { ASSERT(y * width + x < elements.size()); return elements[y * width + x]; }

    T& GetElementConst(size_t i) const { ASSERT(i < elements.size()); return elements[i]; }
    T& GetElementConst(size_t x, size_t y) const { ASSERT(y * width + x < elements.size()); return elements[y * width + x]; }

  private:
    size_t width;
    size_t height;
    std::vector<T> elements;
  };


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
