#ifndef CMODELGRID_H
#define CMODELGRID_H

#include <breathe/render/cVertexBufferObject.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      // Grid with fixed resolution: meters
      // In the future:
      // if (distance from grid to camera < 20m) resolution = 10cm, thick lines at 1m
      // else resolution = 1m, thick lines at 10m
      //
      // Each line is at 100m, thick lines at 1000m
      // Each line is at 10m, thick lines at 100m
      // Each line is at 1m, thick lines at 10m

      class cGridRenderer
      {
      public:
        cGridRenderer();

        void Create();

        void SetResolutionInMeters(size_t resolutionInMeters); // Must be 1, 10 or 100
        void SetPosition(const spitfire::math::cVec3& position);

        void Render(spitfire::durationms_t currentTime);

      private:
        breathe::render::cStaticVertexBuffer vboLight; // This is the normal line
        breathe::render::cStaticVertexBuffer vboSolid; // This is the slightly more solid line that is used for every 10th row

        spitfire::math::cVec3 position; // This is rounded based on the resolution
        size_t distanceBetweenEachLineInMeters;
      };
    }
  }
}

#endif // CMODELGRID_H
