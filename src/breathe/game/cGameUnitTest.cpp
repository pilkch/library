#include <cmath>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <iostream>
#include <fstream>
#include <sstream>

#include <list>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <bitset>

// Boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

//FreeType Headers
#include <freetype/ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

// OpenGL headers
#include <GL/GLee.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_net.h>

#ifdef BUILD_PHYSICS_2D
#include <Box2D/Box2D.h>
#elif defined(BUILD_PHYSICS_3D)
#include <ode/ode.h>
#endif

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/cTimer.h>
#include <spitfire/util/unittest.h>
#include <spitfire/util/lang.h>
#include <spitfire/util/process.h>
#include <spitfire/util/thread.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

#include <spitfire/math/math.h>
#include <spitfire/math/cVec2.h>
#include <spitfire/math/cVec3.h>
#include <spitfire/math/cVec4.h>
#include <spitfire/math/cMat4.h>
#include <spitfire/math/cPlane.h>
#include <spitfire/math/cQuaternion.h>
#include <spitfire/math/cColour.h>
#include <spitfire/math/cFrustum.h>
#include <spitfire/math/cOctree.h>
#include <spitfire/math/geometry.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/util/app.h>
#include <breathe/util/cVar.h>
#include <breathe/util/base.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cFont.h>

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
#include <breathe/physics/physics.h>
#endif

#ifdef BUILD_LEVEL
#include <breathe/game/cLevel.h>
#endif

#include <breathe/game/cGameUnitTest.h>
#include <breathe/game/scenegraph.h>
#include <breathe/game/skysystem.h>

#include <breathe/communication/network.h>

#include <breathe/audio/audio.h>

namespace breathe
{
#ifdef BUILD_DEBUG

  void CreateCrate(render::cVertexBufferObjectRef& pCrateVBO, render::material::cMaterialRef& pCrateMaterial)
  {
    std::vector<float> vertices;
    std::vector<float> textureCoordinates;
    //std::vector<uint16_t> indices;

    const float_t fHalfSize = 0.5f;
    const spitfire::math::cVec3 vMin(-fHalfSize, -fHalfSize, -fHalfSize);
    const spitfire::math::cVec3 vMax(fHalfSize, fHalfSize, fHalfSize);

    // Bottom Square
    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMin.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMin.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMin.z);

    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMin.z);

    // Side Squares
    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMin.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMin.z);



    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMin.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMin.z);


    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMin.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMin.z);


    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMin.z);

    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMin.z);

    // Upper Square
    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(0.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMin.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(1.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMax.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMax.z);

    textureCoordinates.push_back(0.0f);
    textureCoordinates.push_back(1.0f);
    vertices.push_back(vMin.x);
    vertices.push_back(vMax.y);
    vertices.push_back(vMax.z);

    pCrateVBO.reset(new breathe::render::cVertexBufferObject);
    pCrateVBO->SetVertices(vertices);
    pCrateVBO->SetTextureCoordinates(textureCoordinates);
    //pCrateVBO->SetIndices(indices);
    pCrateVBO->Compile();


    const spitfire::string_t sFilename(TEXT("materials/crate.mat"));
    pCrateMaterial = pRender->GetMaterial(sFilename);
  }


  cGameUnitTest::cGameUnitTest(cApplication& _app) :
    app(_app)
  {
  }

  cGameUnitTest::~cGameUnitTest()
  {
  }

  bool cGameUnitTest::Init()
  {
    // Create our scenenodes attached to a new group node
    breathe::scenegraph3d::cGroupNodeRef pGroupNode(new breathe::scenegraph3d::cGroupNode);

    // Attach our group node to the scenegraph root node
    {
      breathe::scenegraph3d::cSceneNodeRef pRoot = app.scenegraph.GetRoot();
      breathe::scenegraph3d::cGroupNode* pRootAsGroupNode = static_cast<breathe::scenegraph3d::cGroupNode*>(pRoot.get());
      pRootAsGroupNode->AttachChild(pGroupNode);
    }




    // Create two crates, the first is rotated and positioned relative to the group node, the second is rotated and positioned relative to the first.
    // Then create a spiral of crates
    {
      render::cVertexBufferObjectRef pCrateVBO;
      render::material::cMaterialRef pCrateMaterial;

      CreateCrate(pCrateVBO, pCrateMaterial);

      {
        // Create the first crate
        breathe::scenegraph3d::cModelNodeRef pNodeCrate0(new breathe::scenegraph3d::cModelNode);
        {
          spitfire::math::cQuaternion rotation;
          rotation.SetFromAxisAngle(spitfire::math::v3Up, spitfire::math::DegreesToRadians(10.0f));

          pNodeCrate0->SetRelativePosition(spitfire::math::cVec3(10.0f, 10.0f, 1.0f));
          pNodeCrate0->SetRelativeRotation(rotation);

          breathe::scenegraph3d::cStateSet& stateset = pNodeCrate0->GetStateSet();
          stateset.SetStateFromMaterial(pCrateMaterial);
          stateset.SetGeometryTypeQuads();

          breathe::scenegraph_common::cStateVertexBufferObject& vertexBufferObject = stateset.GetVertexBufferObject();
          vertexBufferObject.pVertexBufferObject = pCrateVBO;
          vertexBufferObject.SetEnabled(true);
          vertexBufferObject.bHasValidValue = true;

          pGroupNode->AttachChild(pNodeCrate0);
        }


        {
          // Create the second crate
          breathe::scenegraph3d::cModelNodeRef pNodeCrate1(new breathe::scenegraph3d::cModelNode);

          spitfire::math::cQuaternion rotation;
          rotation.SetFromAxisAngle(spitfire::math::v3Front, spitfire::math::DegreesToRadians(30.0f));

          pNodeCrate1->SetRelativePosition(spitfire::math::cVec3(0.0f, 0.0f, 3.0f));
          pNodeCrate1->SetRelativeRotation(rotation);

          breathe::scenegraph3d::cStateSet& stateset = pNodeCrate1->GetStateSet();
          stateset.SetStateFromMaterial(pCrateMaterial);
          stateset.SetGeometryTypeQuads();

          breathe::scenegraph_common::cStateVertexBufferObject& vertexBufferObject = stateset.GetVertexBufferObject();
          vertexBufferObject.pVertexBufferObject = pCrateVBO;
          vertexBufferObject.SetEnabled(true);
          vertexBufferObject.bHasValidValue = true;

          // Child of the first crate
          pNodeCrate0->AttachChild(pNodeCrate1);
        }
      }

      // Create a spiral of crates, note, each crate is positioned and rotated relatively to the crate before it
      {
        breathe::scenegraph3d::cModelNodeRef pNodeFirst(new breathe::scenegraph3d::cModelNode);
        {
          pNodeFirst->SetRelativePosition(spitfire::math::cVec3(30.0f, 10.0f, 1.0f));

          breathe::scenegraph3d::cStateSet& stateset = pNodeFirst->GetStateSet();
          stateset.SetStateFromMaterial(pCrateMaterial);
          stateset.SetGeometryTypeQuads();

          breathe::scenegraph_common::cStateVertexBufferObject& vertexBufferObject = stateset.GetVertexBufferObject();
          vertexBufferObject.pVertexBufferObject = pCrateVBO;
          vertexBufferObject.SetEnabled(true);
          vertexBufferObject.bHasValidValue = true;

          pGroupNode->AttachChild(pNodeFirst);
        }

        breathe::scenegraph3d::cModelNodeRef pNodePrevious = pNodeFirst;

        const size_t n = 30;
        for (size_t i = 0; i < n; i++) {
          breathe::scenegraph3d::cModelNodeRef pNodeCurrent(new breathe::scenegraph3d::cModelNode);

          spitfire::math::cQuaternion rotation;
          rotation.SetFromAxisAngle(spitfire::math::v3Front, spitfire::math::DegreesToRadians(15.0f));

          pNodeCurrent->SetRelativePosition(spitfire::math::cVec3(0.0f, 0.5f, 1.0f));
          pNodeCurrent->SetRelativeRotation(rotation);

          breathe::scenegraph3d::cStateSet& stateset = pNodeCurrent->GetStateSet();
          stateset.SetStateFromMaterial(pCrateMaterial);
          stateset.SetGeometryTypeQuads();

          breathe::scenegraph_common::cStateVertexBufferObject& vertexBufferObject = stateset.GetVertexBufferObject();
          vertexBufferObject.pVertexBufferObject = pCrateVBO;
          vertexBufferObject.SetEnabled(true);
          vertexBufferObject.bHasValidValue = true;

          // Child of the previous crate
          pNodePrevious->AttachChild(pNodeCurrent);

          pNodePrevious = pNodeCurrent;
        }
      }
    }



    // Create a grid to get an idea of how big the world is
    grid.Create();
    grid.SetResolutionInMeters(1);

    return true;
  }


  bool cGameUnitTest::Destroy()
  {
    return true;
  }

  void cGameUnitTest::Update(sampletime_t currentTime)
  {
  }

#if defined(BUILD_PHYSICS_2D) || defined(BUILD_PHYSICS_3D)
  void cGameUnitTest::UpdatePhysics(sampletime_t currentTime)
  {
  }
#endif

  void cGameUnitTest::PreRender(sampletime_t currentTime)
  {
  }

  void cGameUnitTest::RenderScene(breathe::sampletime_t currentTime)
  {
    {
      // Draw an array of boxes unfortunately because they are all different, unless we have a colour attribute on a scenenode? we have to do this dynamically instead of through the scenegraph.
      breathe::render::material::cMaterialRef pMaterial(pRender->GetMaterial(TEXT("materials/coloured_texture.mat")));
      if (pMaterial == nullptr) {
        LOG.Error("cGame::cSudokuStatePlaying::_RenderScene", "coloured_texture.mat not found");
        ASSERT(pMaterial != nullptr);
        return;
      }

      pRender->ClearMaterial();
      glEnable(GL_TEXTURE_2D);
      pRender->SetMaterial(pMaterial);

        spitfire::math::cVec3 colour(1.0f, 0.0f, 0.0f);

        const float fHalfWidth = 1.0f;
        const spitfire::math::cVec3 vMin(-fHalfWidth, -fHalfWidth, -fHalfWidth);
        const spitfire::math::cVec3 vMax(fHalfWidth, fHalfWidth, fHalfWidth);

        const size_t n = 10;
        for (size_t z = 0; z < n; z++) {
          for (size_t y = 0; y < n; y++) {
            for (size_t x = 0; x < n; x++) {
              colour.x = float_t(x) / float_t(n); // Red
              colour.y = float_t(y) / float_t(n); // Green
              colour.z = float_t(z) / float_t(n); // Blue
              pRender->SetShaderConstant("colour", colour);

              glMatrixMode(GL_MODELVIEW);
              glPushMatrix();
                glTranslatef(float(x) * 10.0f, float(y) * 10.0f, 90.0f + (float(z) * 10.0f));

                pRender->RenderBoxTextured(vMin, vMax);

                glMatrixMode(GL_MODELVIEW);
              glPopMatrix();
            }
          }
        }

      pRender->ClearMaterial();
    }


    // Render our grid
    grid.Render(currentTime);
  }

  void cGameUnitTest::RenderScreenSpace(sampletime_t currentTime)
  {
  }
#endif
}

