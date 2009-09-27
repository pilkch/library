#ifndef CCURVE_H
#define CCURVE_H

#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>

namespace spitfire
{
  namespace math
  {
    // This can contain any mapping from x:y
    // In an editor or if you were to graph this on paper,
    // x would go from left (0.0f) to right (n)
    // y would go from bottom (0.0f) to top (n)
    // When x is < 0.0f or x is > n, y is 0.0f
    //
    // Typical uses are:
    // engineRPM : torqueNm
    // superchargerRPM: boostpsi
    // turbochargerRPM: boostpsi
    // speed : downforce // Or is this a simpler relationship?  downforce = speed^2
    //
    // cCurve can also be used for sparsely populated 2 heightmaps
    //
    class cCurve
    {
    public:
      cCurve();

      void AddPoint(float_t fX, float_t fY);
      float_t GetYAtPointX(float_t fX) const;

    private:
      std::map<float_t, float_t> points;
    };
  }
}

#endif // CCURVE_H
