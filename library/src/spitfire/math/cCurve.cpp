#include <cstdlib>
#include <cmath>

#include <limits>
#include <vector>
#include <map>

// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cCurve.h>

namespace spitfire
{
  namespace math
  {
    cCurve::cCurve()
    {
      AddPoint(0.0f, 0.0f);
    }

    // x has to be >= 0.0f, it can however go past the last point in the list, it will become the new last point
    // y can be any value it likes
    void cCurve::AddPoint(float_t fX, float_t fY)
    {
      ASSERT(fX >= 0.0f);

      points[fX] = fY;
    }

    float_t cCurve::GetYAtPointX(float_t fMu) const
    {
      if (fMu < 0.0f) return 0.0f;

      float_t fX0 = 0.0f;
      float_t fX1 = 0.0f;
      float_t fY0 = 0.0f;
      float_t fY1 = 0.0f;

      bool bFound = false;

      std::map<float_t, float_t>::const_iterator iter(points.begin());
      const std::map<float_t, float_t>::const_iterator iterEnd(points.end());
      while (iter != iterEnd) {
        const float_t fCurrentX = iter->first;
        const float_t fCurrentY = iter->second;

        // Shuffle the previous values
        fX0 = fX1;
        fX1 = fCurrentX;

        fY0 = fY1;
        fY1 = fCurrentY;

        if (fCurrentX > fMu) {
          // We have found the pair that we are between
          bFound = true;
          break;
        }

        iter++;
      };

      if (!bFound) return 0.0f;

      // Finally return the y value at our x value
      return math::interpolate_linear(fX0, fY0, fX1, fY1, fMu);
    }
  }
}
