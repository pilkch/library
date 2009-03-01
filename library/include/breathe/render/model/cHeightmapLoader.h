#ifndef CMODELHEIGHTMAPLOADER_H
#define CMODELHEIGHTMAPLOADER_H

// TODO: Rename heightmap to terrain?  It's a little bit of both at the moment

namespace breathe
{
  namespace render
  {
    namespace model
    {
      class cTerrainHeightMapLoader
      {
      public:
        cTerrainHeightMapLoader();

        float GetHeight(float x, float y) const;
      };

      // *** Inlines

      inline float cTerrainHeightMapLoader::GetHeight(float x, float y) const
      {
        return (2.0f * sinf(0.05f * x)) + (2.0f * cosf(0.05f * y));
      }
    }
  }
}

#endif // CMODELHEIGHTMAPLOADER_H
