#ifndef CCONTEXT_H
#define CCONTEXT_H

// libopenglmm headers
#include <libopenglmm/cContext.h>

// Spitfire headers
#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/cFrustum.h>

#include <breathe/render/camera.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cTexture.h>
#include <breathe/render/cVertexBufferObject.h>
#include <breathe/render/model/cMesh.h>

namespace breathe
{
  namespace game
  {
    class cRenderGraph;
    class cRenderGraph2d;
  }

  namespace render
  {
    const size_t MAX_TEXTURE_UNITS = 3;
    const size_t MAX_TEXTURE_SIZE = 1024;
    const size_t MAX_LIGHTS = 8;

    enum class CUBE_MAP_FACE {
      POSITIVE_X,
      NEGATIVE_X,
      POSITIVE_Y,
      NEGATIVE_Y,
      POSITIVE_Z,
      NEGATIVE_Z
    };

    /*class cBatchModelContainer
    {
    public:
      cBatchModelContainer() : fDistanceFromCamera(0.0f) {}
      cBatchModelContainer(model::cStaticRef pModel, float fDistanceFromCamera);

      static bool SortBackToFront(const cBatchModelContainer* lhs, const cBatchModelContainer* rhs);
      static bool SortFrontToBack(const cBatchModelContainer* lhs, const cBatchModelContainer* rhs);

      float fDistanceFromCamera;
      model::cStaticRef pModel;
    };

    class cBatchList
    {
    public:
      typedef std::list<cBatchModelContainer*>::iterator iterator;
      typedef std::list<cBatchModelContainer*>::const_iterator const_iterator;

      void AddModel(model::cStaticRef pModel, float fDistanceFromCamera);
      void FinishAddingSortFrontToBack();
      void FinishAddingSortBackToFront();

    private:
      std::vector<cBatchModelContainer*> models;

      friend class cRender;
    };

    class cBatchController
    {
    public:
      typedef std::map<material::cMaterialRef, cBatchList*>::iterator iterator;
      typedef std::map<material::cMaterialRef, cBatchList*>::const_iterator const_iterator;

      void AddModel(model::cStaticRef pModel, float fDistanceFromCamera);
      void FinishAdding();

    private:
      std::map<material::cMaterialRef, cBatchList*> opaque;
      std::map<material::cMaterialRef, cBatchList*> transparent;

      friend class cRender;
    };*/

    class cStatistics
    {
    public:
      cStatistics();

      void Reset();

      size_t nStateChanges; // How many times a new stateset was applied
      size_t nModelsRendered;
      size_t nSceneObjectsRendered;
      size_t nVertexBufferObjectsBound; // How many vertex buffer objects were bound (ie. 30)
      size_t nVertexBufferObjectsRendered; // How many vertex buffer objects were rendered (ie. 300)
      size_t nTrianglesRendered; // How many triangles were rendered in the whole scene
    };

    using opengl::MODE2D_TYPE;
    using opengl::cContext;
  }
}

extern breathe::render::cContext* pContext; // TODO: Remove pContext

#endif // CCONTEXT_H
