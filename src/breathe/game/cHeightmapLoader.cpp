#include <cstdio>
#include <cstring>
#include <cmath>
#include <cassert>

#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

// OpenGL headers
#include <GL/GLee.h>

// SDL headers
#include <SDL/SDL.h>
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

#include <breathe/render/cContext.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cVertexBufferObject.h>

#include <breathe/game/cHeightmapLoader.h>

namespace breathe
{
  namespace game
  {
    // .heightmap File Format
    //
    // This class reads and writes our custom .heightmap files
    // A .heightmap file is little endian and looks like this:
    // char 'H'
    // char 'E'
    // char 'I'
    // char 'G'
    // char 'H'
    // char 'T'
    // char 'M'
    // char 'A'
    // char 'P'
    // uint8_t version (ie. 1, 2, 3, etc.)
    // uint32_t width
    // uint32_t height
    // float32_t fValue at 0, 0
    // float32_t fValue at 1, 0
    // ...
    // float32_t fValue at width, height

    class cHeightmapFile
    {
    public:
      cHeightmapFile();

      void Clear();

      bool Load(const string_t& sFilename);
      bool Save(const string_t& sFilename);

      size_t GetWidth() const { return uiWidth; }
      size_t GetHeight() const { return uiHeight; }

      void SetDimensions(size_t _uiWidth, size_t _uiHeight) { uiWidth = _uiWidth; uiHeight = _uiHeight; }

      std::vector<float_t> data;

    private:
      size_t uiWidth;
      size_t uiHeight;
    };

    cHeightmapFile::cHeightmapFile() :
      uiWidth(0),
      uiHeight(0)
    {
    }

    void cHeightmapFile::Clear()
    {
      data.clear();

      uiWidth = 0;
      uiHeight = 0;
    }

    bool cHeightmapFile::Load(const string_t& sFilename)
    {
      std::ifstream file;
      file.open(spitfire::string::ToUTF8(sFilename).c_str(), std::ios::in | std::ios::binary);
      if (!file) return false;

      {
        const char szHeightmap[] = { "HEIGHTMAP" };
        char szHeightmapValue[] = { "123456789" };
        const size_t n = std::strlen(szHeightmap);
        file.read((char*)&szHeightmapValue, n); // NOTE: The null terminator is not read

        // We have to null terminate it ourselves
        szHeightmapValue[n] = 0;

        // Make sure that we are reading a heightmap file
        ASSERT(std::strcmp(szHeightmapValue, szHeightmap) == 0);
      }

      uint8_t version = 0;
      file.read((char*)&version, sizeof(version));
      ASSERT(version == 1);

      uint32_t width = 0;
      uint32_t height = 0;
      file.read((char*)&width, sizeof(width));
      file.read((char*)&height, sizeof(height));

      uiWidth = width;
      uiHeight = height;

      const size_t n = uiWidth * uiHeight;
      data.insert(data.begin(), n, 0.0f);
      ASSERT(sizeof(float_t) == 4);
      file.read((char*)data.data(), n * sizeof(float_t));

      return true;
    }

    bool cHeightmapFile::Save(const string_t& sFilename)
    {
      std::ofstream file;
      file.open(spitfire::string::ToUTF8(sFilename).c_str(), std::ios::out | std::ios::binary);
      if (!file) return false;

      const char szHeightmap[] = { "HEIGHTMAP" };
      file.write((char*)&szHeightmap, std::strlen(szHeightmap));

      const uint8_t version = 1;
      file.write((char*)&version, sizeof(version));

      uint32_t width = uiWidth;
      uint32_t height = uiHeight;
      file.write((char*)&width, sizeof(width));
      file.write((char*)&height, sizeof(height));

      const size_t n = uiWidth * uiHeight;
      ASSERT(sizeof(float_t) == 4);
      file.write((char*)data.data(), n * sizeof(float_t));

      return true;
    }



    // Lower scale = smaller hills but smoother due to higher resolution, larger scale = higher range of hills, but less resolution which means more jagged transitions
    cTerrainHeightMap::cTerrainHeightMap() :
      width(0),
      height(0),

      fWidthOrHeightOfEachTile(1.0f),
      fScaleZ(1.0f),
      heightmap(1, 1)
    {
    }

    template <class T>
    void LoadFromGeneric(const T& t, size_t& width, size_t& height, float fScaleZ, cDynamicContainer2D<float>& heightmap)
    {
      LOG<<"LoadFromGeneric"<<std::endl;

      width = t.GetWidth();
      height = t.GetWidth();

      cDynamicContainer2D<float> newHeightmap(width + 1, height + 1);

#ifdef BUILD_DEBUG
      // We adjust this value to get a smoother although shorter heightmap
      const float fAdjustedScaleZ = 0.5f * fScaleZ;
#endif

      size_t uiCount = 0;
      for (size_t h = 0; h < height; h++) {
        for (size_t w = 0; w < width; w++) {
          const float fValue = fAdjustedScaleZ * t.data[uiCount++];

          newHeightmap.GetElement(w, h) = fValue;
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

    void cTerrainHeightMap::LoadFromFile(const string_t& sFilename)
    {
      width = 0;
      height = 0;

      // Load heightmap
      const string_t sExtension = spitfire::filesystem::GetExtension(sFilename);
      if (sExtension == TEXT("heightmap")) {
        cHeightmapFile heightmapFile;
        if (!heightmapFile.Load(sFilename)) {
          LOG.Error("Heightmap", "cTerrainHeightMap::LoadFromFile Failed to load " + breathe::string::ToUTF8(sFilename));
          return;
        }

        LoadFromGeneric(heightmapFile, width, height, fScaleZ, heightmap);
      }
      {
        render::cTextureRef pTexture(new render::cTexture);
        if (!pTexture->Load(sFilename)) {
          LOG.Error("Heightmap", "cTerrainHeightMap::LoadFromFile Failed to load " + breathe::string::ToUTF8(sFilename));
          return;
        }

        LoadFromGeneric(*(pTexture.get()), width, height, fScaleZ, heightmap);

        for (size_t i = 0; i < 10; i++) Smooth();
      }


      // For testing saving of .heightmap files
      //SaveToFile(TEXT("/media/development/dev/sudoku/data/textures/terrain.heightmap"));
    }

    void cTerrainHeightMap::SaveToFile(const string_t& sFilename) const
    {
      // Make sure that we are saving a .heightmap file
      const string_t sExtension = spitfire::filesystem::GetExtension(sFilename);
      ASSERT(sExtension == TEXT("heightmap"));


      cHeightmapFile heightmapFile;

      ASSERT(width != 0);
      ASSERT(height != 0);
      heightmapFile.SetDimensions(width, height);

      // For saving we want to scale down to the original values
      const float fOneOverAdjustedScaleZ = 1.0f / (0.1f * fScaleZ);

      // Reserve the space for all of our values
      heightmapFile.data.insert(heightmapFile.data.begin(), width * height, 0.0f);

      // Fill out our heightmap file with the data from our heightmap
      for (size_t h = 0; h < height; h++) {
        for (size_t w = 0; w < width; w++) {
          heightmapFile.data[(h * width) + w] = heightmap.GetElement(w, h) * fOneOverAdjustedScaleZ;
        }
      }


      // Save heightmap
      if (!heightmapFile.Save(sFilename)) {
        LOG.Error("Heightmap", "cTerrainHeightMap::SaveToFile Failed to save " + breathe::string::ToUTF8(sFilename));
        return;
      }
    }

    void cTerrainHeightMap::Smooth()
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

    float cTerrainHeightMap::GetHeight(float x, float y) const
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
      return (h0 + xfrac * (h1 - h0) + yfrac * (h3 - h0)) - 1.0f; // - 1.0f is just a temporary hack so that a car can get onto it
    }

    math::cVec3 cTerrainHeightMap::GetNormalOfTriangle(const math::cVec3& p0, const math::cVec3& p1, const math::cVec3& p2) const
    {
      const math::cVec3 v0 = p1 - p0;
      const math::cVec3 v1 = p2 - p0;

      return v0.CrossProduct(v1);
    }

    math::cVec3 cTerrainHeightMap::GetNormal(float x, float y) const
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

    bool cTerrainHeightMap::CollideWithRayVerySlowFunction(const math::cRay3& ray, float& fDepth) const
    {
      fDepth = 0.0f;

      const float_t fResolution = 5.0f;
      spitfire::math::cVec3 position = ray.GetOrigin();
      const spitfire::math::cVec3 direction = ray.GetDirection();
      ASSERT(direction.GetLength() > spitfire::math::cEPSILON);

      const spitfire::math::cVec3 increment = fResolution * direction;

      const float_t fMaxLength = ray.GetLength();
      while (fDepth < fMaxLength) {
        if (GetHeight(position.x, position.y) > position.z) {
          return true;
        }

        position += increment;
        fDepth += fResolution;
      }

      return false;
    }
  }
}
