#ifndef CSTATICMODELLOADER_H
#define CSTATICMODELLOADER_H

#include <breathe/game/scenegraph.h>

#include <breathe/render/cMaterial.h>
#include <breathe/render/cVertexBufferObject.h>
#include <breathe/render/model/cStatic.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      class cStaticModelLoader
      {
      public:
        bool Load(const string_t& sFilename, cStaticModel& model) const;
      };

      class cStaticModelWriter
      {
      public:
        bool Save(const string_t& sFilename, const cStaticModel& model) const;
      };






      class cStaticModelSceneNodeFactoryItem
      {
      public:
        breathe::render::cVertexBufferObjectRef pVBO;
        breathe::render::material::cMaterialRef pMaterial;
      };

      class cStaticModelSceneNodeFactory
      {
      public:
        void LoadFromFile(const spitfire::string_t& sFilename, std::vector<cStaticModelSceneNodeFactoryItem>& meshes) const;

        void CreateSceneNodeAttachedTo(std::vector<cStaticModelSceneNodeFactoryItem>& meshes, breathe::scenegraph3d::cGroupNodeRef pGroupNode) const;
      };
    }
  }
}

#endif // CSTATICMODELLOADER_H
