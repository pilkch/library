#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <vector>
#include <map>
#include <list>
#include <set>

#include <string>
#include <sstream>

#include <iostream>
#include <fstream>

// Boost headers
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

// OpenGL headers
#include <GL/GLee.h>

// SDL headers
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>


// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>
#include <spitfire/util/cTimer.h>
#include <spitfire/util/unittest.h>

#include <spitfire/storage/filesystem.h>
#include <spitfire/storage/xml.h>

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
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/physics/physics.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cParticleSystem.h>

#include <breathe/game/scenegraph.h>
#include <breathe/game/skysystem.h>

const unsigned int uiNodeNameDisplayTime = 100;


namespace breathe
{
  namespace scenegraph_common
  {
    string_t SceneNodeGenerateUniqueName()
    {
      static size_t i = math::random(32000);

      ostringstream_t o;
      o<<TEXT("SCENE_NODE_GENERATED_NAME_");
      o<<i;

      // Increment i for next time
      i++;

      return o.str();
    }
  }

  namespace scenegraph3d
  {
    void cStateSet::Clear()
    {
      ASSERT(render::MAX_TEXTURE_UNITS == 3);

      priority = PRIORITY::NORMAL;

      alphablending.Clear();
      texture[0].Clear();
      texture[1].Clear();
      texture[2].Clear();
      shader.Clear();
      vertexBufferObject.Clear();
      geometryType = scenegraph_common::GEOMETRY_TYPE::DEFAULT;
    }

    void cStateSet::SetStateFromMaterial(render::material::cMaterialRef pMaterial)
    {
      ASSERT(render::MAX_TEXTURE_UNITS == 3);
      ASSERT(pMaterial != nullptr);

      alphablending.Clear();
      texture[0].Clear();
      texture[1].Clear();
      texture[2].Clear();
      shader.Clear();



      const size_t nTextures = pMaterial->vLayer.size();

      // NOTE: These intentionally fall through and collect the textures below them
      switch (nTextures) {
        case 3: {
          if (pMaterial->vLayer[2]->pTexture != nullptr) {
            texture[2].bHasValidValue = true;
            texture[2].bTurnedOn = true;
            texture[2].pTexture = pMaterial->vLayer[2]->pTexture;
          }
        }
        case 2: {
          if (pMaterial->vLayer[1]->pTexture != nullptr) {
            texture[1].bHasValidValue = true;
            texture[1].bTurnedOn = true;
            texture[1].pTexture = pMaterial->vLayer[1]->pTexture;
          }
        }
        case 1: {
          if (pMaterial->vLayer[0]->pTexture != nullptr) {
            texture[0].bHasValidValue = true;
            texture[0].bTurnedOn = true;
            texture[0].pTexture = pMaterial->vLayer[0]->pTexture;
          }
        }
      }

      if (pMaterial->pShader != nullptr) {
        shader.bHasValidValue = true;
        shader.bTurnedOn = true;
        shader.pShader = pMaterial->pShader;
      }
    }


    cSceneNode::cSceneNode() :
#ifdef BUILD_DEBUG
      bIsShowingBoundingBox(false),
#endif
      bIsVisible(true),
      bIsDirty(true),
      bHasRelativePosition(false),
      bHasRelativeRotation(false)
    {
    }

    // Determines whether this node is in the scene graph, ie. whether it's ultimate ancestor is the root scene node.
    bool cSceneNode::IsWithinNode(cSceneNodeRef pNodeToFind) const
    {
      cSceneNodeRef pNode = GetParent();
      while (pNode != nullptr) {
        pNode = pNode->GetParent();
        if (pNode == pNodeToFind) return true;
      }

      return false;
    }

    bool cSceneNode::IsParentOfChild(const cSceneNodeRef pChild) const
    {
      return ((pChild != nullptr) && (pChild->GetParent() == shared_from_this()));
    }

    void cSceneNode::SetDirty()
    {
      if (!bIsDirty) {
        bIsDirty = true;
        if (pParent != nullptr) pParent->SetDirty();
      }
    }

    void cSceneNode::SetRelativePosition(const math::cVec3& position)
    {
      bHasRelativePosition = true;
      relativePosition = position;
      SetDirty();
    }

    void cSceneNode::SetRelativeRotation(const math::cQuaternion& rotation)
    {
      bHasRelativeRotation = true;
      relativeRotation = rotation;
      SetDirty();
    }

    // If we don't have a parent return our relative position, else return our parent's absolute position + our own
    spitfire::math::cMat4 cSceneNode::GetAbsoluteMatrix() const
    {
      spitfire::math::cMat4 mat;
      if (bHasRelativeRotation) mat.SetRotationPart(relativeRotation);
      if (bHasRelativePosition) mat.SetTranslationPart(relativePosition);

      if (pParent != nullptr) mat = pParent->GetAbsoluteMatrix() * mat;

      return mat;
    }

    spitfire::math::cVec3 cSceneNode::GetAbsolutePosition() const
    {
      return GetAbsoluteMatrix().GetPosition();
    }

    spitfire::math::cQuaternion cSceneNode::GetAbsoluteRotation() const
    {
      return GetAbsoluteMatrix().GetRotation();
    }

    void cSceneNode::GenerateBoundingVolume()
    {
      //TODO: Generate the bounding volume for this scenenode

      bIsDirty = false;
    }

    void cSceneNode::SetVisible(bool bVisible)
    {
      bIsVisible = bVisible;
    }

    void cSceneNode::UpdateBoundingVolumeAndSetNotDirty()
    {
      // We should only be calling this function on pRoot
      ASSERT(pParent == nullptr);

      // No point if we are not dirty
      if (!bIsDirty) return;

      boundingSphere.position = math::v3Zero;
      if (bHasRelativePosition) boundingSphere.position += relativePosition;

      // Set our boundingSphere volume from our possible children as only a derived class will know how
      boundingSphere.fRadius = UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius();

      // We have now updated our boundingSphere and we are not dirty any more
      bIsDirty = false;
    }

    float_t cSceneNode::UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius()
    {
      // We should not be calling this function on pRoot or on a child that does not have a parent set
      ASSERT(pParent != nullptr);

      // No point if we are not dirty
      if (!bIsDirty) return boundingSphere.fRadius;

      // If we are dirty our parents must also be dirty at this point
      ASSERT(pParent->IsDirty());

      // Set our boundingSphere position from our parent node plus our relative position if we have any
      boundingSphere.position = pParent->GetBoundingSphere().position;
      if (bHasRelativePosition) boundingSphere.position += relativePosition;

      // Set our boundingSphere volume from our possible children as only a derived class will know how
      boundingSphere.fRadius = UpdateBoundingVolumeAndSetNotDirtyReturningBoundingVolumeRadius();

      // We have now updated our boundingSphere and we are not dirty any more
      bIsDirty = false;

      return boundingSphere.fRadius;
    }

    void cSceneNode::AttachChild(cSceneNodeRef pChild)
    {
      ASSERT(pChild != nullptr);
      ASSERT(!IsParentOfChild(pChild));

      _AttachChild(pChild);

      pChild->pParent = shared_from_this();
    }

    void cSceneNode::DetachChildForUseLater(cSceneNodeRef pChild)
    {
      ASSERT(pChild != nullptr);
      ASSERT(IsParentOfChild(pChild));

      _DetachChild(pChild);

      pChild->pParent.reset();
    }

    void cSceneNode::DeleteChildRecursively(cSceneNodeRef pChild)
    {
      ASSERT(pChild != nullptr);
      ASSERT(IsParentOfChild(pChild));

      _DeleteChildRecursively(pChild);

      pChild->pParent.reset();
    }




    // These are the default behaviours of the attach/detach/delete child functions

    void cSceneNode::_AttachChild(cSceneNodeRef pChild)
    {
      children.push_back(pChild);
    }

    void cSceneNode::_DetachChild(cSceneNodeRef pChild)
    {
      children.remove(pChild);
    }

    void cSceneNode::_DeleteChildRecursively(cSceneNodeRef pChild)
    {
      pChild->DeleteAllChildrenRecursively();
    }

    void cSceneNode::_DeleteAllChildrenRecursively()
    {
      // If we don't have any children, return
      if (children.empty()) return;

      // Visit all child nodes
      child_iterator iter(children.begin());
      const child_iterator iterEnd(children.end());
      while (iter != iterEnd) {
        ASSERT(*iter != nullptr);
        (*iter)->DeleteAllChildrenRecursively();

        iter++;
      }

      children.clear();
    }

    void cSceneNode::_Update(cUpdateVisitor& visitor)
    {
      // If we don't have any children, return
      if (children.empty()) return;

      // Visit all child nodes
      child_iterator iter(children.begin());
      const child_iterator iterEnd(children.end());
      while (iter != iterEnd) {
        ASSERT(*iter != nullptr);
        visitor.Visit(*(*iter));

        iter++;
      }
    }

    void cSceneNode::_Cull(cCullVisitor& visitor)
    {
      // If we don't have any children, return
      if (children.empty()) return;

      // Visit all child nodes
      child_iterator iter(children.begin());
      const child_iterator iterEnd(children.end());
      while (iter != iterEnd) {
        ASSERT(*iter != nullptr);
        visitor.Visit(*(*iter));

        iter++;
      }
    }



    void cProjection2D::_Update(cUpdateVisitor& visitor)
    {
      cSceneNode::_Update(visitor);

      visitor.Visit(*pChild);
    }

    void cProjection2D::_Cull(cCullVisitor& visitor)
    {
      cSceneNode::_Cull(visitor);

      visitor.Visit(*pChild);
    }




    void cModelNode::_Update(cUpdateVisitor& visitor)
    {
      cSceneNode::_Update(visitor);
    }

    void cModelNode::_Cull(cCullVisitor& visitor)
    {
      cSceneNode::_Cull(visitor);

      visitor.Visit(&stateset, GetAbsoluteMatrix());
    }


    void cAnimationNode::_Update(cUpdateVisitor& visitor)
    {
      cSceneNode::_Update(visitor);

      const sampletime_t currentTime = spitfire::util::GetTimeMS();
      animation.model.Update(currentTime);
    }

    void cAnimationNode::_Cull(cCullVisitor& visitor)
    {
      cSceneNode::_Cull(visitor);

      visitor.Visit(&animation.model, GetAbsoluteMatrix());
    }


    void cLightNode::_Update(cUpdateVisitor& visitor)
    {
      cSceneNode::_Update(visitor);

      //if (pChild != nullptr) pChild->Update(visitor);
    }

    void cLightNode::_Cull(cCullVisitor& visitor)
    {
      cSceneNode::_Cull(visitor);

      //if (pChild != nullptr) pChild->Cull(visitor);

      //visitor.Visit(*this);
    }


    void cSwitchNode::_AttachChild(cSceneNodeRef pChild)
    {
      node.push_back(pChild);
    }

    void cSwitchNode::_DetachChild(cSceneNodeRef pChild)
    {
      ASSERT(false);
      //node.remove(pChild);
    }

    void cSwitchNode::_Update(cUpdateVisitor& visitor)
    {
      if (node.empty()) return;
      ASSERT(index < node.size());

      // Only visit the node that we require
      visitor.Visit(*node[index]);
    }

    void cSwitchNode::_Cull(cCullVisitor& visitor)
    {
      if (node.empty()) return;
      ASSERT(index < node.size());

      // Only visit the node that we require
      visitor.Visit(*node[index]);
    }


    void cLODNode::_AttachChild(cSceneNodeRef pChild)
    {
      node.push_back(pChild);
    }

    void cLODNode::_DetachChild(cSceneNodeRef pChild)
    {
      ASSERT(false);
      //node.remove(pChild);
    }

    void cLODNode::_Update(cUpdateVisitor& visitor)
    {
      if (node.empty()) return;
      ASSERT(index < node.size());

      // Only visit the node that we require
      visitor.Visit(*node[index]);
    }

    void cLODNode::_Cull(cCullVisitor& visitor)
    {
      if (node.empty()) return;
      ASSERT(index < node.size());

    // Only visit the node that we require
      visitor.Visit(*node[index]);
    }



    void cPagedLODNodeChild::Create(size_t x, size_t y)
    {
      ASSERT(x < 100);
      ASSERT(y < 100);

      spitfire::ostringstream_t o;
      o<<"models/terrain_";
      if (x < 10) o<<"0";
      o<<x;
      o<<"_";
      if (y < 10) o<<"0";
      o<<y;
      o<<".png";

      LOG<<"Looking for file \""<<o.str()<<"\""<<std::endl;

      spitfire::string_t sFilename;
      filesystem::FindResourceFile(o.str(), sFilename);
      loader.LoadFromFile(sFilename);

      terrain.reset(new cLODNode);
      //grass.reset(new cLODNode);
      trees.reset(new cLODNode);

      // We need at least one level of detail
      LoadAndSetTerrainLOD0();

      AttachChild(terrain);
      terrain->SetRelativePosition(spitfire::math::cVec3(float(x) * 512.0f, float(y) * 512.0f, 0.0f));

      AttachChild(trees);
    }

    void cPagedLODNodeChild::LoadTerrainLOD(size_t index, size_t nWidthOrHeight)
    {
      // If we have not already loaded this level of detail then we have to load it
      if (terrain->GetNumberOfChildren() < (index + 1)) {
        render::model::cTerrain terrainCreator;
        terrainCreator.Create(loader, nWidthOrHeight);


        cModelNodeRef pNode(new cModelNode);

        scenegraph3d::cStateSet& stateset = pNode->GetStateSet();
        stateset.SetStateFromMaterial(terrainCreator.GetMaterial());

        scenegraph_common::cStateVertexBufferObject& vertexBufferObject = stateset.GetVertexBufferObject();
        vertexBufferObject.SetVertexBufferObject(terrainCreator.GetVBO());
        vertexBufferObject.SetEnabled(true);
        vertexBufferObject.SetHasValidValue(true);

        terrain->AttachChild(pNode);
      }

      ASSERT(terrain->GetNumberOfChildren() != 4);
    }

    void cPagedLODNodeChild::LoadAndSetTerrainLOD0()
    {
      LoadTerrainLOD(0, 16);

      terrain->SetLOD(0);
    }

    void cPagedLODNodeChild::LoadAndSetTerrainLOD1()
    {
      // We need to make sure that the previous levels of detail have been loaded
      LoadAndSetTerrainLOD0();

      LoadTerrainLOD(1, 32);

      terrain->SetLOD(1);
    }

    void cPagedLODNodeChild::LoadAndSetTerrainLOD2()
    {
      // We need to make sure that the previous levels of detail have been loaded
      LoadAndSetTerrainLOD0();
      LoadAndSetTerrainLOD1();

      LoadTerrainLOD(2, 64);

      terrain->SetLOD(2);
    }

    void cPagedLODNodeChild::_Update(cUpdateVisitor& visitor)
    {
      //LOG<<"cPagedLODNodeChild::_Update"<<std::endl;

      visitor.Visit(*terrain);
      visitor.Visit(*trees);
      //visitor.Visit(*grass);
    }

    void cPagedLODNodeChild::_Cull(cCullVisitor& visitor)
    {
      //LOG<<"cPagedLODNodeChild::_Cull"<<std::endl;

      const float_t fLength = (visitor.GetCameraPosition() - terrain->GetAbsolutePosition()).GetLength();
      if (fLength < 1000.0f) LoadAndSetTerrainLOD2();
      else if (fLength < 1800.0f) LoadAndSetTerrainLOD1();
      else LoadAndSetTerrainLOD0();

      if (fLength < 2500.0f) {
        visitor.Visit(*terrain);

        if (fLength < 1000.0f) {
          visitor.Visit(*trees);

          if (fLength < 650.0f) {
            //visitor.Visit(*grass);
          }
        }
      }
    }


    void cPagedLODNode::Clear()
    {
      node.clear();
    }

    void cPagedLODNode::SetNumberOfNodes(size_t width, size_t height)
    {
      Clear();

      const size_t n = width * height;
      spitfire::vector::push_back(node, n, cPagedLODNodeChildRef());

      for (size_t y = 0; y < height; y++) {
        for (size_t x = 0; x < width; x++) {
          const size_t i = (y * width) + x;
          node[i].reset(new cPagedLODNodeChild);
          node[i]->Create(x, y);
          AttachChild(node[i]);
        }
      }
    }

    void cPagedLODNode::_Update(cUpdateVisitor& visitor)
    {
      // Visit each node
      const size_t n = node.size();
      for (size_t i = 0; i < n; i++) visitor.Visit(*node[i]);
    }

    void cPagedLODNode::_Cull(cCullVisitor& visitor)
    {
      // Visit each node
      const size_t n = node.size();
      for (size_t i = 0; i < n; i++) visitor.Visit(*node[i]);
    }





    cSpatialGraphNode::cSpatialGraphNode()
    {
      memset(&pChild[0], 0, sizeof(cSpatialGraphNode*) * 8);
    }

    void cSpatialGraphNode::_Update(cUpdateVisitor& visitor)
    {
    }

    void cSpatialGraphNode::_Cull(cCullVisitor& visitor)
    {
    }





    cUpdateVisitor::cUpdateVisitor(cSceneGraph& scenegraph)
    {
      ASSERT(scenegraph.GetRoot() != nullptr);
      Visit(*scenegraph.GetRoot());
    }

    cCullVisitor::cCullVisitor(cSceneGraph& _scenegraph, const render::cCamera& _camera) :
      scenegraph(_scenegraph),
      camera(_camera)
    {
      ASSERT(scenegraph.GetRoot() != nullptr);
      Visit(*scenegraph.GetRoot());

      /*listOpaque.clear();
      mTransparent.clear();

      for each item in list
      if (opaque)
      listOpaque.add(item);
      else
      mTransparent.add(fDistance, item);*/
    }

    void cCullVisitor::Visit(cStateSet* pStateSet, const spitfire::math::cMat4& matAbsolutePositionAndRotation)
    {
      scenegraph.GetRenderGraph().AddRenderable(pStateSet, matAbsolutePositionAndRotation);
    }

    void cCullVisitor::Visit(character::cMd3* pModel, const spitfire::math::cMat4& matAbsolutePositionAndRotation)
    {
      scenegraph.GetRenderGraph().AddMD3Model(pModel, matAbsolutePositionAndRotation);
    }


    void cRenderGraph::AddMD3Model(character::cMd3* pModel, const spitfire::math::cMat4& matAbsolutePositionAndRotation)
    {
      cRenderGraphMd3Pair pair;
      pair.pModel = pModel;
      pair.matAbsolutePositionAndRotation = matAbsolutePositionAndRotation;
      md3Models.push_back(pair);
    }

    void cRenderGraph::AddRenderable(cStateSet* pNewStateSet, const spitfire::math::cMat4& matAbsolutePositionAndRotation)
    {
      std::map<cStateSet*, cRenderableList*>::iterator iter(mOpaque.begin());
      const std::map<cStateSet*, cRenderableList*>::iterator iterEnd(mOpaque.end());
      while (iter != iterEnd) {
        cStateSet* pState = iter->first;
        ASSERT(pState != nullptr);

        if (*pState == *pNewStateSet) {
          // We found at least one other renderable with the same state as us so we will add to the list for that state
          cRenderableList* pList = iter->second;
          pList->push_back(matAbsolutePositionAndRotation);
          return;
        }

        iter++;
      }

      cRenderableList* pList = new cRenderableList;
      pList->push_back(matAbsolutePositionAndRotation);
      mOpaque[pNewStateSet] = pList;
    }





    void cRenderVisitor::ApplyStateSet(cStateSet& stateSet)
    {
      size_t n = render::MAX_TEXTURE_UNITS;
      size_t unit = 0;

      unit = GL_TEXTURE0_ARB;

      const bool bIsAlphaBlending = stateSet.alphablending.IsValidAndTurnedOn();

      for (size_t i = 0; i < n; i++, unit++) {
        // Activate the current texture unit
        glActiveTexture(unit);

        if (bIsAlphaBlending) {
          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glEnable(GL_BLEND);
        }

        if (stateSet.texture[i].IsValidAndTurnedOn()) {
          if (stateSet.texture[i].pTexture->uiMode == render::TEXTURE_MODE::TEXTURE_CUBE_MAP) {
             // Cube map texture

            glEnable(GL_TEXTURE_CUBE_MAP);
            glBindTexture(GL_TEXTURE_CUBE_MAP, stateSet.texture[i].pTexture->uiTexture);

            glMatrixMode(GL_TEXTURE);
            glPushMatrix();
              glLoadIdentity();

#if 0
              float y = -Angle(spitfire::math::cVec2(frustum.eye.x, frustum.eye.y), spitfire::math::cVec2(frustum.target.x, frustum.target.y));
              float x = -Angle(spitfire::math::cVec2(frustum.eye.y, frustum.eye.z), spitfire::math::cVec2(frustum.target.y, frustum.target.z));
              //std::cout<<y<<"\t"<<x<<"\n";

              glRotatef(y, 0.0f, 1.0f, 0.0f);
              glRotatef(x, 1.0f, 0.0f, 0.0f);
#elif 0
              float mat[16];
              glGetFloatv(GL_MODELVIEW_MATRIX, mat);

              math::cQuaternion q(mat[8], mat[9], -mat[10]);

              glLoadMatrixf(static_cast<float* >(q.GetMatrix()));
#endif

              glMatrixMode(GL_MODELVIEW);


              glEnable(GL_TEXTURE_GEN_S);
              glEnable(GL_TEXTURE_GEN_T);
              glEnable(GL_TEXTURE_GEN_R);

              glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
              glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
              glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
          } else {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, stateSet.texture[i].pTexture->uiTexture);
          }
        }
      }

      if (stateSet.shader.IsValidAndTurnedOn()) pRender->BindShader(stateSet.shader.pShader);
    }

    void cRenderVisitor::UnApplyStateSet(cStateSet& stateSet)
    {
      size_t n = render::MAX_TEXTURE_UNITS;
      size_t unit = 0;

      unit = GL_TEXTURE0_ARB;

      const bool bIsAlphaBlending = stateSet.alphablending.IsValidAndTurnedOn();

      for (size_t i = 0; i < n; i++, unit++) {
        // Activate the current texture unit
        glActiveTexture(unit);

        if (bIsAlphaBlending) {
          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          glBlendFunc(GL_ONE, GL_ZERO);
          glDisable(GL_BLEND);
        }

        if (stateSet.texture[i].IsValidAndTurnedOn()) {
          if (stateSet.texture[i].pTexture->uiMode == render::TEXTURE_MODE::TEXTURE_CUBE_MAP) {
             // Cube map texture

              glDisable(GL_TEXTURE_GEN_R);
              glDisable(GL_TEXTURE_GEN_T);
              glDisable(GL_TEXTURE_GEN_S);


              glMatrixMode(GL_TEXTURE);
              glPopMatrix();

            glMatrixMode(GL_MODELVIEW);

            glDisable(GL_TEXTURE_CUBE_MAP);
          } else {
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_TEXTURE_2D);
          }
        }
      }

      if (stateSet.shader.IsValidAndTurnedOn()) pRender->UnBindShader();

      glActiveTexture(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
    }


    cRenderVisitor::cRenderVisitor(cSceneGraph& scenegraph, const math::cFrustum& frustum)
    {
      ASSERT(scenegraph.GetRoot() != nullptr);

      size_t uiTriangles = 0;

      cRenderGraph& rendergraph = scenegraph.GetRenderGraph();

      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        glLoadIdentity();
        glMultMatrixf(frustum.m.GetOpenGLMatrixPointer());

          pRender->ClearMaterial();

          // Opaque first
          {
            std::map<cStateSet*, cRenderGraph::cRenderableList*>::iterator iter(rendergraph.mOpaque.begin());
            const std::map<cStateSet*, cRenderGraph::cRenderableList*>::iterator iterEnd(rendergraph.mOpaque.end());
            while (iter != iterEnd) {
              cStateSet* pStateSet = iter->first;
              cRenderGraph::cRenderableList* pRenderableList = iter->second;

              ApplyStateSet(*pStateSet);

                render::cVertexBufferObjectRef pVbo = pStateSet->vertexBufferObject.GetVertexBufferObject();
                pVbo->Bind();

                  cRenderGraph::cRenderableList::iterator renderableIter((*pRenderableList).begin());
                  const cRenderGraph::cRenderableList::iterator renderableIterEnd((*pRenderableList).end());

                  while (renderableIter != renderableIterEnd) {
                    glMatrixMode(GL_MODELVIEW);
                    glPushMatrix();
                      glMultMatrixf((*renderableIter).GetOpenGLMatrixPointer());

#ifdef BUILD_DEBUG
                      pRender->RenderAxisReference(0.0f, 0.0f, 0.0f);
#endif

                      switch (pStateSet->geometryType) {
                        case scenegraph_common::GEOMETRY_TYPE::TRIANGLES: {
                          pVbo->RenderTriangles();
                          break;
                        }
                        case scenegraph_common::GEOMETRY_TYPE::QUADS: {
                          pVbo->RenderQuads();
                          break;
                        }
                      };

                      glMatrixMode(GL_MODELVIEW);
                    glPopMatrix();

                    uiTriangles += pVbo->GetApproximateTriangleCount();

                    renderableIter++;
                  }
                pVbo->Unbind();

              UnApplyStateSet(*pStateSet);

              iter++;
            }
          }

          // MD3 Models, ideally this would not be a special case
          {
            const sampletime_t currentTime = spitfire::util::GetTimeMS();
            std::list<cRenderGraphMd3Pair>::iterator iter(rendergraph.md3Models.begin());
            const std::list<cRenderGraphMd3Pair>::iterator iterEnd(rendergraph.md3Models.end());
            while (iter != iterEnd) {
              glMatrixMode(GL_MODELVIEW);
              glPushMatrix();
                glMultMatrixf((*iter).matAbsolutePositionAndRotation.GetOpenGLMatrixPointer());
                pRender->RenderAxisReference(0.0f, 0.0f, 0.0f);
                (*iter).pModel->Render(currentTime);
              glMatrixMode(GL_MODELVIEW);
              glPopMatrix();

              iter++;
            }
          }







          // Render the sky
          // We do this lastish to avoid redraw (Hopefully the sky will be mostly occluded and we will only have to draw sky for part of the screen)
          // Another way to do this is first:
          // glDepthMask(GL_FALSE);
          // DrawSky();
          // glDepthMask(GL_TRUE);
          // ... Draw everything else
          if (scenegraph.pSkySystem != nullptr) {

            pRender->ClearMaterial();

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
              glLoadIdentity();
              glMultMatrixf(pRender->GetFrustum().m.GetOpenGLMatrixPointer());

              // Position the whole sky at the the viewpoint so that it gives the illusion that it is infinitely far away
              glTranslatef(pRender->GetFrustum().eye.x, pRender->GetFrustum().eye.y, pRender->GetFrustum().eye.z);

              glMatrixMode(GL_MODELVIEW);
              glPushMatrix();
                const spitfire::math::cQuaternion rotation(scenegraph.pSkySystem->GetSkyDomeAtmosphereRenderer().GetRotationSun());
                const math::cVec3 axis(rotation.GetAxis());
                glRotatef(spitfire::math::RadiansToDegrees(rotation.GetAngle()), axis.x, axis.y, axis.z);

                pRender->SelectTextureUnit0();

                // Set our texture
                if (!pRender->IsWireFrame()) pRender->SetTexture0(scenegraph.pSkySystem->GetSkyDomeAtmosphereRenderer().GetTexture());

                {
                  const sky::cSkyDomeAtmosphereRenderer::cVertex* pVertices = scenegraph.pSkySystem->GetSkyDomeAtmosphereRenderer().GetVertices();
                  const size_t n = scenegraph.pSkySystem->GetSkyDomeAtmosphereRenderer().GetNumberOfVertices();

                  // Render the top half of the sphere
                  glBegin(GL_TRIANGLE_STRIP);
                    for (size_t i = 0; i < n; i++) {
                      glTexCoord2f(pVertices[i].u, pVertices[i].v);
                      glVertex3f(pVertices[i].x, pVertices[i].y, -pVertices[i].z);
                    }
                  glEnd();

                  // Flip 180 degrees and render the bottom half of the sphere
                  glMatrixMode(GL_MODELVIEW);
                  glPushMatrix();
                    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

                    glBegin(GL_TRIANGLE_STRIP);
                      for (size_t i = 0; i < n; i++) {
                        glTexCoord2f(pVertices[i].u, pVertices[i].v);
                        glVertex3f(pVertices[i].x, pVertices[i].y, -pVertices[i].z);
                      }
                    glEnd();

                    glMatrixMode(GL_MODELVIEW);
                  glPopMatrix();
                }

                glMatrixMode(GL_MODELVIEW);
              glPopMatrix();

              glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

            pRender->ClearMaterial();
          }





          // Transparent second
          {
            std::map<float, cRenderGraphTransparentPair>::iterator iter(rendergraph.mTransparent.begin());
            const std::map<float, cRenderGraphTransparentPair>::iterator iterEnd(rendergraph.mTransparent.end());
            while (iter != iterEnd) {
              //uiTriangles += (iter->second)->Render();
              iter++;
            }
          }






          // Render the stars and planets
          // TODO: This is really slow at the moment, probably because every particle could potentially move every single frame so we can't optimise at all, we can't just throw everything into a static vbo and be done with it unfortunately
          if (scenegraph.pSkySystem != nullptr) {
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
              glLoadIdentity();
              glMultMatrixf(pRender->GetFrustum().m.GetOpenGLMatrixPointer());

              // Position the whole sky at the the viewpoint so that it gives the illusion that it is infinitely far away
              glTranslatef(pRender->GetFrustum().eye.x, pRender->GetFrustum().eye.y, pRender->GetFrustum().eye.z);

              render::cParticleSystemCustomBillboard* pParticleSystemPlanets = scenegraph.pSkySystem->GetPlanetParticleSystem();
              pParticleSystemPlanets->Update(spitfire::util::GetTimeMS());
              pParticleSystemPlanets->Render();

              pRender->ClearMaterial();

              render::cParticleSystemCustomBillboard* pParticleSystemStars = scenegraph.pSkySystem->GetStarParticleSystem();
              pParticleSystemStars->Update(spitfire::util::GetTimeMS());
              pParticleSystemStars->Render();

              glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
          }

        glMatrixMode(GL_MODELVIEW);
      glPopMatrix();


      pRender->uiTriangles += uiTriangles;
    }


    cSceneGraph::cSceneGraph() :
      bIsCullingEnabled(true),
      backgroundColour(0.0f, 0.0f, 1.0f)
    {
      pRoot.reset(new cGroupNode);
    }

    void cSceneGraph::Create()
    {
      pSkySystem.reset(new sky::cSkySystem);

      string_t sFullPath;
      spitfire::filesystem::FindResourceFile(TEXT("skysystem.xml"), sFullPath);

      sky::cSkySystemLoader loader;
      loader.LoadFromFile(*pSkySystem, sFullPath);

      pSkySystem->Create();
    }

    /*cSceneGraphSpawn::cSceneGraphSpawn()
      : v3Position(0.0f, 0.0f, 0.0f),
        v3Rotation(1.0f, 0.0f, 0.0f)
    {

    }*/


    /*cSceneNode::cSceneNode(cSceneGraph* p, std::string sNewFilename)
    {
      uiStatus=0;

      pLevel=p;

      sFilename=sNewFilename;
    }

    void cSceneNode::Load()
    {
      LOG.Success("LevelNode", "Load");
      LOG.Success("LevelNode", "Load returning");
    }

    void cSceneNode::Unload()
    {
      uiStatus=NODE_INACTIVE;
    }

    void cSceneNode::Update(sampletime_t currentTime)
    {
      if (NODE_INACTIVE!=uiStatus)
        uiStatus--;
      if (NODE_UNLOAD==uiStatus)
        Unload();
    }

    unsigned int cSceneNode::Render()
    {
      unsigned int uiTriangles = 0;

      uiTriangles+=pRender->RenderStaticModel(pRender->GetModel(breathe::string::ToString_t(sFilename + "mesh.3ds")));

      return uiTriangles;
    }*/

    void cSceneGraph::Update(sampletime_t currentTime)
    {
      if (pSkySystem != nullptr) {
        pSkySystem->Update(currentTime);
        ambientColour = pSkySystem->GetAmbientColour();
      }

      cUpdateVisitor visitor(*this);

      // TODO: Huh?  Do we need this?  Do we need this here?
      pRender->SetClearColour(backgroundColour);
    }

    void cSceneGraph::Cull(sampletime_t currentTime, const render::cCamera& camera)
    {
      renderGraph.Clear();
      cCullVisitor visitor(*this, camera);
    }

    void cSceneGraph::Render(sampletime_t currentTime, const math::cFrustum& frustum)
    {
      cRenderVisitor visitor(*this, frustum);
    }

#ifdef BUILD_DEBUG
    int counter = 0;


    class cSceneNodeUnitTest : public cSceneNode
    {
    public:
      cSceneNodeUnitTest();
      ~cSceneNodeUnitTest();

    private:
      typedef std::list<cSceneNodeRef>::iterator iterator;

      void _AttachChild(cSceneNodeRef pChild);
      void _DetachChild(cSceneNodeRef pChild);

      std::list<cSceneNodeRef> children;
    };

    cSceneNodeUnitTest::cSceneNodeUnitTest()
    {
      LOG<<"cSceneNodeUnitTest::cSceneNodeUnitTest"<<std::endl;
      counter++;
    }

    cSceneNodeUnitTest::~cSceneNodeUnitTest()
    {
      LOG<<"cSceneNodeUnitTest::~cSceneNodeUnitTest"<<std::endl;
      counter--;
    }

    void cSceneNodeUnitTest::_AttachChild(cSceneNodeRef pChild)
    {
      children.push_back(pChild);
    }

    void cSceneNodeUnitTest::_DetachChild(cSceneNodeRef pChild)
    {
      children.remove(pChild);
    }




    class cSceneGraphUnitTest : protected breathe::util::cUnitTestBase
    {
    public:
      cSceneGraphUnitTest() :
        cUnitTestBase(TEXT("cSceneGraphUnitTest"))
      {
        printf("cSceneGraphUnitTest 3d\n");
      }

      void Test()
      {
        counter = 0;

        cModelNode model;

        cLightNode light;
        cSceneGraph scenegraph;
        cSceneNodeRef pRoot = scenegraph.GetRoot();

        {
          // We build a structure that looks like this:
          //       root
          //     a
          //   b
          // c

          if (counter != 0) {
            ostringstream_t o;
            o<<TEXT("cSceneGraphUnitTest FAILED No nodes added, counter should equal 0, counter=")<<counter;
            SetFailed(o.str());
          }

          cSceneNodeRef a(new cSceneNodeUnitTest);
          pRoot->AttachChild(a);

          if (counter != 1) {
            ostringstream_t o;
            o<<TEXT("cSceneGraphUnitTest FAILED Added a, counter should equal 1, counter=")<<counter;
            SetFailed(o.str());
          }

          cSceneNodeRef b(new cSceneNodeUnitTest);
          a->AttachChild(b);

          if (counter != 2) {
            ostringstream_t o;
            o<<TEXT("cSceneGraphUnitTest FAILED Added b, counter should equal 2, counter=")<<counter;
            SetFailed(o.str());
          }

          {
            cSceneNodeRef c(new cSceneNodeUnitTest);
            b->AttachChild(c);

            if (counter != 3) {
              ostringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED c attached to b, counter should equal 3, counter=")<<counter;
              SetFailed(o.str());
            }


            {
              // Now we add to the structure to create something like this:
              //       root
              //     a
              //   b   d
              // c       e

              cSceneNodeRef d(new cSceneNodeUnitTest);
              a->AttachChild(d);

              if (counter != 4) {
                ostringstream_t o;
                o<<TEXT("cSceneGraphUnitTest FAILED d attached to a, counter should equal 4, counter=")<<counter;
                SetFailed(o.str());
              }

              cSceneNodeRef e(new cSceneNodeUnitTest);
              d->AttachChild(e);

              if (counter != 5) {
                ostringstream_t o;
                o<<TEXT("cSceneGraphUnitTest FAILED e attached to d, counter should equal 5, counter=")<<counter;
                SetFailed(o.str());
              }


              // Now we detach b and c to to create something like this:
              //       root
              //     a
              //       d            b
              //         e        c

              a->DetachChildForUseLater(b);

              // Even though we detached two nodes nothing should have been destroyed or created, we should still have the same amount
              if (counter != 5) {
                ostringstream_t o;
                o<<TEXT("cSceneGraphUnitTest FAILED b detached from a, counter should equal 5, counter=")<<counter;
                SetFailed(o.str());
              }

              // Now we attach b and c to to create something like this:
              //       root
              //     a      b
              //       d      c
              //         e

              pRoot->AttachChild(b);

              if (counter != 5) {
                ostringstream_t o;
                o<<TEXT("cSceneGraphUnitTest FAILED b attached to root, counter should equal 5, counter=")<<counter;
                SetFailed(o.str());
              }
            }

            // Now we test d and e falling out of scope but not deleting as the scenegraph still references d which references e
            //       root
            //     a      b
            //              c

            // d and e are still referenced, nothing should have been added or deleted
            if (counter != 5) {
              ostringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED e has fallen out of scope but should not affect the counter, counter should equal 5, counter=")<<counter;
              SetFailed(o.str());
            }


            // Now we delete d and e from the scenegraph and if they are not referenced anywhere else they will be deleted automatically
            //       root
            //     a      b
            //              c
            /*a->DeleteChildRecursively(d);

            // Now we should have just 4 objects left
            if (counter != 3) {
              ostringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED d and e deleted recursively from a, counter should equal 3, counter=")<<counter;
              SetFailed(o.str());
            }*/
          }

          // Now we test c falling out of scope but not deleting as the scenegraph still references it
          //       root
          //     a      b
          //              c

          // c is still referenced, nothing should have been added or deleted
          if (counter != 3) {
            ostringstream_t o;
            o<<TEXT("cSceneGraphUnitTest FAILED c has fallen out of scope, counter should equal 3, counter=")<<counter;
            SetFailed(o.str());
          }

          {
            // Now we add f, g and h to a
            //       root
            //     a      b
            //   f
            // g   h

            cSceneNodeRef f(new cSceneNodeUnitTest);
            a->AttachChild(f);

            if (counter != 4) {
              ostringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED f attached to a, counter should equal 4, counter=")<<counter;
              SetFailed(o.str());
            }

            cSceneNodeRef g(new cSceneNodeUnitTest);
            f->AttachChild(g);

            if (counter != 5) {
              ostringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED g attached to f, counter should equal 5, counter=")<<counter;
              SetFailed(o.str());
            }

            cSceneNodeRef h(new cSceneNodeUnitTest);
            f->AttachChild(h);

            if (counter != 6) {
              ostringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED h attached to f, counter should equal 6, counter=")<<counter;
              SetFailed(o.str());
            }




            //model.AddChild(&light);

            //pRoot->AddChild(&model);

            const sampletime_t currentTime = util::GetTimeMS();
            render::cCamera camera;

            scenegraph.Update(currentTime);
            scenegraph.Cull(currentTime, camera);
            scenegraph.Render(currentTime, pRender->GetFrustum());









            // Now we delete all children of a recursively
            //       root
            //     a      b
            a->DeleteAllChildrenRecursively();

            // Still referenced by variables f, g and h
            if (counter != 6) {
              ostringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED Children of a deleted recursively, counter should equal 6, counter=")<<counter;
              SetFailed(o.str());
            }
          }

          // Ok, now f, g and h have fallen out of scope and been deleted
          // Now we should have just 4 objects left
          if (counter != 3) {
            ostringstream_t o;
            o<<TEXT("cSceneGraphUnitTest FAILED f, g and h have now fallen out of scope, counter should equal 3, counter=")<<counter;
            SetFailed(o.str());
          }
        }

        // Now the whole scenegraph has fallen out of scope so our counter should be 0 again
        if (counter != 0) {
          ostringstream_t o;
          o<<TEXT("cSceneGraphUnitTest FAILED All children of the scenegraph have now fallen out of scope, counter should equal 0, counter=")<<counter;
          SetFailed(o.str());
        }
      }
    };

    cSceneGraphUnitTest gSceneGraphUnitTest;
#endif
  }
}
