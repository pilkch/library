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

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>
#include <breathe/render/model/cHeightmapPatch.h>
#include <breathe/render/model/cHeightmap.h>

namespace breathe
{
  namespace render
  {
    namespace model
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
          cTextureRef pTexture(new cTexture);
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
      }

      float cTerrainHeightMapLoader::GetHeight(float x, float y) const
      {
        x /=  fWidthOrHeightOfEachTile;
        y /=  fWidthOrHeightOfEachTile;

        if (x < 0.0f) x = 0.0f;
        if (y < 0.0f) y = 0.0f;
        if (x > static_cast<float>(width)) x = static_cast<float>(width);
        if (y > static_cast<float>(height)) y = static_cast<float>(height);

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
    }
  }
}
