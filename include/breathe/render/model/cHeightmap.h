#ifndef CMODELHEIGHTMAP_H
#define CMODELHEIGHTMAP_H

#include <breathe/render/cVertexBufferObject.h>
#include <breathe/game/cHeightmapLoader.h>
#include <breathe/game/scenegraph.h>

// TODO: Rename heightmap to terrain?  It's a little bit of both at the moment
// NOTE: cTerrain and cGrass are purely for rendering, no collision detection etc.

// http://www.ogre3d.org/wiki/index.php/Practical_Application
// http://freeworld3d.org/
// http://www.freeworld3d.org/downloads.html
// http://www.freeworld3d.org/tutorials/roadsystem.html
// http://www.bundysoft.com/L3DT/
// http://www.bundysoft.com/wiki/doku.php?id=tutorials:l3dt:roads
// http://www.bundysoft.com/wiki/doku.php?id=tutorials:l3dt:bumpmap
// http://www.bundysoft.com/wiki/doku.php?id=tutorials:l3dt:tips
// http://www.bundysoft.com/wiki/doku.php?id=tutorials:l3dt:dead_rabit
// http://www.bundysoft.com/wiki/doku.php?id=tutorials:l3dt:newclimate:02
// http://images.google.com.au/images?q=road+cutting
// http://www.swiftless.com/tutorials/terrain/terraintuts.html

namespace breathe
{
  namespace render
  {
    namespace model
    {
      class cTerrain
      {
      public:
        void Create(const game::cTerrainHeightMap& loader, const size_t nWidthOrHeight);

        // TODO: REMOVE ME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*********************!!!!!!!!
        void Create(const game::cTerrainHeightMap& loader) { Create(loader, 512); }

        void Update(spitfire::durationms_t currentTime);
        void Render(spitfire::durationms_t currentTime);

        cVertexBufferObjectRef GetVBO() const { return pVBO; }
        material::cMaterialRef GetMaterial() const { return pMaterial; }

      private:
        cVertexBufferObjectRef pVBO;
        material::cMaterialRef pMaterial;
      };

      class cGrassStatic
      {
      public:
        void Create(const game::cTerrainHeightMap& loader, float fOffsetX, float fOffsetY, float fWidth, float fHeight);

        void Update(spitfire::durationms_t currentTime) {}
        void Render(spitfire::durationms_t currentTime);

      private:
        cStaticVertexBuffer vbo;
        material::cMaterialRef pMaterial;
      };

      class cGrassAnimated
      {
      public:
        void Create(const game::cTerrainHeightMap& loader, float fOffsetX, float fOffsetY, float fWidth, float fHeight);

        void Update(spitfire::durationms_t currentTime) {}
        void Render(spitfire::durationms_t currentTime);

      private:
        cStaticVertexBuffer vbo;
        material::cMaterialRef pMaterial;
      };

      typedef cGrassAnimated cGrass;
    }
  }
}

#endif // CMODELHEIGHTMAP_H
