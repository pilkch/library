#ifndef CHEIGHTMAPLOADER_H
#define CHEIGHTMAPLOADER_H

#include <spitfire/algorithm/algorithm.h>

#include <spitfire/math/geometry.h>

// TODO: Rename heightmap to terrain?  It's a little bit of both at the moment

namespace breathe
{
  namespace game
  {
    class cTerrainHeightMap
    {
    public:
      cTerrainHeightMap();

      void LoadFromFile(const string_t& sFilename);
      void SaveToFile(const string_t& sFilename) const;

      void GetDimensions(size_t& width, size_t& height) const;

      float GetHeight(float x, float y) const;
      void SetHeight(float x, float y, float fValue);

      math::cVec3 GetNormal(float x, float y) const;

      bool CollideWithRayVerySlowFunction(const math::cRay3& ray, float& fDepth) const;

    private:
      void Smooth();
      math::cVec3 GetNormalOfTriangle(const math::cVec3& p0, const math::cVec3& p1, const math::cVec3& p2) const;

      // How many tiles in each direction
      size_t width;
      size_t height;

      // Width of each tile
      float fWidthOrHeightOfEachTile;

      // Vertical scale
      float fScaleZ;

      cDynamicContainer2D<float> heightmap;
    };
  }
}

#endif // CHEIGHTMAPLOADER_H
