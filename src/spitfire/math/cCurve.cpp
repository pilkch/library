// Spitfire Includes
#include <spitfire/spitfire.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cCurve.h>

namespace spitfire
{
  namespace math
  {
    void cCurve::AddPoint(float_t fX, float_t fY)
    {
      auto iter = points.begin();
      auto iterEnd = points.end();
      while (iter != iterEnd) {
        if (iter->x > fX) {
          points.insert(iter, cVec2(fX, fY));
          return;
        }

        iter++;
      }

      // All the existing points were before this one, so add it at the end
      points.push_back(cVec2(fX, fY));
    }

    float_t cCurve::GetYAtPointX(float_t fMu) const
    {
      if (points.empty()) {
        // No point data
        return 0.0f;
      } else if (points.size() == 1) {
        // Not enough points to integrate
        return points[0].y;
      }

      bool bFound = false;

      auto iter = points.begin();
      auto iterEnd = points.end();

      // Default these values to the first point
      cVec2 first = *iter;
      cVec2 second = *iter;

      while (iter != iterEnd) {
        // Shuffle the previous values
        first = second;
        second = *iter;

        if (second.x >= fMu) {
          // We have found the pair that we are between
          bFound = true;
          break;
        }

        iter++;
      };

      // We are outside the range so just return the last y value
      if (!bFound) return first.y;

      // Check if these are actually the same point in which case we can just return the y value directly
      if (first.x == second.y) {
        return first.y;
      }

      // Finally return the y value at our x value
      return math::interpolate_linear(first.x, first.y, second.x, second.y, fMu);
    }
  }
}
