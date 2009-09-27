#ifndef CSPLINE_H
#define CSPLINE_H

#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>

namespace spitfire
{
  namespace math
  {
    template <class T>
    class cSpline
    {
    public:
      cSpline();
      explicit cSpline(const cSpline& rhs);

      cSpline& operator=(const cSpline& rhs);

      void AddPoint(const T& v);
      void Clear();

      T GetPosition(float fPositionAlongSpline0To1) const;
      T GetTangent(float fPositionAlongSpline0To1) const;

      size_t GetNumPoints() const;
      const T& GetNthPoint(int n) const;

    private:
      void Assign(const cSpline& rhs);

      // Compute the Catmull-Rom parametric equation given a time (t) and a vector quadruple (p1,p2,p3,p4).
      T GetPositionBetweenPoints(float t, const T& p1, const T& p2, const T& p3, const T& p4) const;

      std::vector<T> vp;
      float fDelta;
    };


    template <class T>
    cSpline<T>::cSpline() :
      vp(),
      fDelta(0)
    {
    }

    template <class T>
    cSpline<T>::cSpline(const cSpline<T>& rhs)
    {
      Assign(rhs);
    }

    template <class T>
    cSpline<T>& cSpline<T>::operator=(const cSpline<T>& rhs)
    {
      Assign(rhs);

      return *this;
    }

    template <class T>
    void cSpline<T>::Assign(const cSpline<T>& rhs)
    {
      vp = rhs.vp;
      fDelta = rhs.fDelta;
    }

    template <class T>
    size_t cSpline<T>::GetNumPoints() const
    {
      return vp.size();
    }

    template <class T>
    const T& cSpline<T>::GetNthPoint(int n) const
    {
      return vp[n];
    }

    // Solve the Catmull-Rom parametric equation for a given time(t) and vector quadruple (p1,p2,p3,p4)
    template <class T>
    T cSpline<T>::GetPositionBetweenPoints(float t, const T& p1, const T& p2, const T& p3, const T& p4) const
    {
      const float t2 = t * t;
      const float t3 = t2 * t;

      const float b1 = 0.5f * (-t3 + (2.0f * t2) - t);
      const float b2 = 0.5f * ((3.0f * t3) - (5.0f * t2) + 2.0f);
      const float b3 = 0.5f * ((-3.0f * t3) + (4.0f * t2) + t);
      const float b4 = 0.5f * (t3 - t2);

      return ((p1 * b1) + (p2 * b2) + (p3 * b3) + (p4 * b4));
    }

    template <class T>
    void cSpline<T>::AddPoint(const T& v)
    {
      vp.push_back(v);
      fDelta = 1.0f / float(vp.size());
    }

    template <class T>
    void cSpline<T>::Clear()
    {
      vp.clear();
    }

    template <class T>
    T cSpline<T>::GetPosition(float t) const
    {
      t = clamp(t, 0.0f, 1.0f);

      // Find out in which interval we are on the spline
      size_t p = size_t(t / fDelta);

      const size_t n = vp.size() - 1;

      // Compute local control point indices
      size_t p0 = clamp<size_t>(p - 1, 0, n);
      size_t p1 = clamp<size_t>(p, 0, n);
      size_t p2 = clamp<size_t>(p + 1, 0, n);
      size_t p3 = clamp<size_t>(p + 2, 0, n);

      // Convert t to relative
      float lt = (t - fDelta * float(p)) / fDelta;

      // Interpolate
      return GetPositionBetweenPoints(lt, vp[p0], vp[p1], vp[p2], vp[p3]);
    }

    template <class T>
    T cSpline<T>::GetTangent(float fPositionAlongSpline0To1) const
    {
      const float fDelta = 0.01f;
      const T p0 = GetPosition(fPositionAlongSpline0To1 - fDelta);
      const T p1 = GetPosition(fPositionAlongSpline0To1 + fDelta);

      const T line = p1 - p0;
      T tangent;
      tangent.x = -line.y;
      tangent.y = line.x;

      tangent.Normalise();

      return tangent;
    }



    typedef cSpline<cVec2> cSpline2D;
    typedef cSpline<cVec3> cSpline3D;
  }
}

#endif // CSPLINE_H
