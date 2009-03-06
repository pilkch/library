#include <cstdio>
#include <cmath>
#include <cassert>

#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

// Boost includes
#include <boost/shared_ptr.hpp>

#include <GL/GLee.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/base.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cVertexBufferObject.h>

#include <breathe/game/cHeightmapLoader.h>

namespace breathe
{
  namespace game
  {
      // Lower scale = smaller hills but smoother due to higher resolution, larger scale = higher range of hills, but less resolution which means more jagged transitions
    cTerrainHeightMapLoader::cTerrainHeightMapLoader() :
      width(0),
      height(0),

      fWidthOrHeightOfEachTile(1.0f),
      fScaleZ(1.0f),
      heightmap(1, 1)
    {
    }

    void cTerrainHeightMapLoader::LoadFromFile(const string_t& sFilename)
    {
      width = 0;
      height = 0;

      // Load heightmap
      {
        render::cTextureRef pTexture(new render::cTexture);
        if (pTexture->Load(sFilename) == breathe::BAD) {
          LOG.Error("Heightmap", "cTerrainHeightMapLoader::LoadFromFile Failed to load " + breathe::string::ToUTF8(sFilename));
          return;
        }

        width = pTexture->uiWidth;
        height = pTexture->uiHeight;

        float fHighest = -math::cINFINITY;
        float fLowest = math::cINFINITY;

        cDynamicContainer2D<float> newHeightmap(width + 1, height + 1);

        size_t uiCount = 0;
        for (size_t h = 0; h < height; h++) {
          for (size_t w = 0; w < width; w++) {
            const float fValue = fScaleZ * pTexture->data[uiCount++];

            newHeightmap.GetElement(w, h) = fValue;

            if (fValue > fHighest) fHighest = fValue;
            if (fValue < fLowest) fLowest = fValue;
          }
        }

        // Set the last extra column on the end
        for (size_t h = 0; h < height; h++) {
          newHeightmap.GetElement(width, h) = newHeightmap.GetElement(width - 1, h);
        }

        // Set the last extra row on the bottom
        for (size_t w = 0; w < width; w++) {
          newHeightmap.GetElement(w, height) = newHeightmap.GetElement(w, height - 1);
        }

        // Set the last extra element
        newHeightmap.GetElement(width, height) = newHeightmap.GetElement(width - 1, height - 1);

        heightmap = newHeightmap;
      }

      for (size_t i = 0; i < 10; i++) Smooth();
    }

    void cTerrainHeightMapLoader::Smooth()
    {
      cDynamicContainer2D<float> smoothed(width + 1, height + 1);

      float fHeights[5];

      for (size_t y = 0; y < width + 1; y++) {
        for (size_t x = 0; x < height + 1; x++) {
          fHeights[0] = fHeights[1] = fHeights[2] = fHeights[3] = fHeights[4] = heightmap.GetElement(x, y);

          if (x != 0) fHeights[0] = heightmap.GetElement(x - 1, y);
          if (y != height) fHeights[1] = heightmap.GetElement(x, y + 1);
          if (y != 0) fHeights[3] = heightmap.GetElement(x, y - 1);
          if (x != width) fHeights[4] = heightmap.GetElement(x + 1, y);

          const float fAverageHeightOfSurrounding = 0.25f * (fHeights[0] + fHeights[1] + fHeights[3] + fHeights[4]);
          smoothed.GetElement(x, y) = 0.5f * (fHeights[2] + fAverageHeightOfSurrounding);
        }
      }

      heightmap = smoothed;
    }

    float cTerrainHeightMapLoader::GetHeight(float x, float y) const
    {
      x /=  fWidthOrHeightOfEachTile;
      y /=  fWidthOrHeightOfEachTile;

      if (x < 0.0f) x = 0.0f;
      if (y < 0.0f) y = 0.0f;
      if (x >= static_cast<float>(width)) x = static_cast<float>(width - 1);
      if (y >= static_cast<float>(height)) y = static_cast<float>(height - 1);

      const size_t xi = static_cast<size_t>(x);
      const size_t yi = static_cast<size_t>(y);

      //   0---1
      //   |   |
      //   3---2

      const float h0 = heightmap.GetElement(xi, yi); // This value
      const float h1 = heightmap.GetElement(xi + 1, yi); // The adjacent value in this row
      const float h3 = heightmap.GetElement(xi, yi + 1); // The adjacent value in the next row

      const float xfrac = x - static_cast<float>(xi);
      const float yfrac = y - static_cast<float>(yi);

      // Calculate interpolated ground height
      return 4.0f + (h0 + xfrac * (h1 - h0) + yfrac * (h3 - h0));
    }

    math::cVec3 cTerrainHeightMapLoader::GetNormalOfTriangle(const math::cVec3& p0, const math::cVec3& p1, const math::cVec3& p2) const
    {
      const math::cVec3 v0 = p1 - p0;
      const math::cVec3 v1 = p2 - p0;

      return v0.CrossProduct(v1);
    }

    math::cVec3 cTerrainHeightMapLoader::GetNormal(float x, float y) const
    {
      x /=  fWidthOrHeightOfEachTile;
      y /=  fWidthOrHeightOfEachTile;

      // Get the height of the target point and the 4 heights in a cross shape around the target
      const math::cVec3 points[] = {
        // First column
        math::cVec3(x - 1.0f, y, GetHeight(x - 1.0f, y)),

        // Second column
        math::cVec3(x, y - 1.0f, GetHeight(x, y - 1.0f)),
        math::cVec3(x, y, GetHeight(x, y)),
        math::cVec3(x, y + 1.0f, GetHeight(x, y + 1.0f)),

        // Third column
        math::cVec3(x + 1.0f, y, GetHeight(x + 1.0f, y)),
      };

      spitfire::math::cVec3 normal;

      normal += GetNormalOfTriangle(points[0], points[2], points[1]);
      normal += GetNormalOfTriangle(points[0], points[2], points[3]);
      normal += GetNormalOfTriangle(points[4], points[2], points[1]);
      normal += GetNormalOfTriangle(points[4], points[2], points[3]);

      normal.Normalise();

      return normal;
    }
  }
}
