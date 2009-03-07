#ifndef CHEIGHTMAPLOADER_H
#define CHEIGHTMAPLOADER_H

#include <spitfire/algorithm/algorithm.h>

// TODO: Rename heightmap to terrain?  It's a little bit of both at the moment

namespace breathe
{
  namespace game
  {
    class cTerrainHeightMapLoader
    {
    public:
      cTerrainHeightMapLoader();

      void LoadFromFile(const string_t& sFilename);

      float GetHeight(float x, float y) const;
      math::cVec3 GetNormal(float x, float y) const;

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
