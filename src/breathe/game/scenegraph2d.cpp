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

// Boost includes
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

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

#include <breathe/game/scenegraph2d.h>

namespace breathe
{
  namespace scenegraph2d
  {
    void cStateSet::Clear()
    {
      ASSERT(render::MAX_TEXTURE_UNITS == 3);

      alphablending.Clear();
      colour.Clear();
      texture[0].Clear();
      texture[1].Clear();
      texture[2].Clear();
      geometryType = scenegraph_common::GEOMETRY_TYPE::DEFAULT;
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

    void cSceneNode::SetRelativeRotation(float_t rotation)
    {
      bHasRelativeRotation = true;
      relativeRotation = rotation;
      SetDirty();
    }

    // If we don't have a parent return our relative position, else return our parent's absolute position + our own
    math::cVec3 cSceneNode::GetAbsolutePosition() const
    {
      if (pParent != nullptr) return pParent->GetAbsolutePosition() + relativePosition;

      return relativePosition;
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


    void cModelNode::_Update(cUpdateVisitor& visitor)
    {
      //visitor.Visit(*this);
    }

    void cModelNode::_Cull(cCullVisitor& visitor)
    {
      //visitor.Visit(*this);
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



    void cLineBufferNode::_Update(cUpdateVisitor& visitor)
    {
      visitor.Visit(*this);
    }

    void cLineBufferNode::AddLine(const math::cVec2& p0, const math::cVec2& p1)
    {
      vertices.push_back(p0);
      vertices.push_back(p1);
    }



    void cGeometryBufferNode::_Update(cUpdateVisitor& visitor)
    {
      visitor.Visit(*this);
    }

    void cGeometryBufferNode::AddTriangle(const math::cVec2& p0, const math::cVec2& p1, const math::cVec2& p2, const math::cVec2& uv0, const math::cVec2& uv1, const math::cVec2& uv2)
    {
      vertices.push_back(p0);
      vertices.push_back(p1);
      vertices.push_back(p2);

      textureCoordinates.push_back(uv0);
      textureCoordinates.push_back(uv1);
      textureCoordinates.push_back(uv2);
    }

    void cGeometryBufferNode::AddRectangle(const math::cVec2& p0, const math::cVec2& p1, const math::cVec2& p2, const math::cVec2& p3, const math::cVec2& uv0, const math::cVec2& uv1, const math::cVec2& uv2, const math::cVec2& uv3)
    {
      // TODO: check this
      ASSERT(false);
      AddTriangle(p0, p1, p2, uv0, uv1, uv2);
      AddTriangle(p1, p2, p3, uv1, uv2, uv3);
    }

    void cGeometryBufferNode::AddAxisAlignedRectangle(const math::cVec2& p0, const math::cVec2& p3, const math::cVec2& uv0, const math::cVec2& uv3)
    {
      // TODO: check this
      ASSERT(false);
      const math::cVec2 p1(p0.x, p3.y);
      const math::cVec2 p2(p3.x, p0.y);

      const math::cVec2 uv1(uv0.x, uv3.y);
      const math::cVec2 uv2(uv3.x, uv0.y);

      AddRectangle(p0, p1, p2, p3, uv0, uv1, uv2, uv3);
    }

    void cGeometryBufferNode::AddAxisAlignedRectangle(const math::cVec2& p0, const math::cVec2& p1)
    {
      // TODO: check this
      ASSERT(false);
      const math::cVec2 uv0(0.0f, 1.0f);
      const math::cVec2 uv1(0.0f, 1.0f);
      AddAxisAlignedRectangle(p0, p1, uv0, uv1);
    }



    cGraphNode::cGraphNode(size_t nPoints) :
      fDistanceBetweenEachPoint(1.0f / float_t(nPoints - 1)),
      fMax(100.0f),
      points(nPoints)
    {
    }

    void cGraphNode::AddPoint(float_t fValue)
    {
      points.push_back(fValue);
    }

    void cGraphNode::_Update(cUpdateVisitor& visitor)
    {
      cSceneNode::_Update(visitor);

      vertexBuffer.Clear();

      const float_t fOneOverMax = 1.0f / fMax;

      const size_t n = points.size();
      for (size_t i = 0; i < n; i++) {
        vertexBuffer.AddPoint(math::cVec2(fDistanceBetweenEachPoint * float_t(i), 1.0f - (fOneOverMax * points[i])));
      }
    }

    void cGraphNode::_Cull(cCullVisitor& visitor)
    {
      cSceneNode::_Cull(visitor);

      visitor.Visit(&stateset, &vertexBuffer);
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

    void cCullVisitor::Visit(cStateSet* pStateSet, const cVertexBuffer* pVertexBuffer)
    {
      scenegraph.GetRenderGraph().AddRenderable(pStateSet, pVertexBuffer);
    }




    void cRenderVisitor::ApplyStateSet(cStateSet& stateSet)
    {
      size_t n = render::MAX_TEXTURE_UNITS;
      size_t unit = 0;

      unit = GL_TEXTURE0_ARB;

      const bool bIsAlphaBlending = stateSet.alphablending.IsValidAndTurnedOn();
      const bool bIsColour = stateSet.colour.IsValidAndTurnedOn();

      for (size_t i = 0; i < n; i++, unit++) {
        // Activate the current texture unit
        glActiveTexture(unit);

        if (bIsAlphaBlending) {
          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
          glEnable(GL_BLEND);
        }

        if (bIsColour) {
          glColor4f(stateSet.colour.colour.r, stateSet.colour.colour.g, stateSet.colour.colour.b, stateSet.colour.colour.a);
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
    }

    void cRenderVisitor::UnApplyStateSet(cStateSet& stateSet)
    {
      size_t n = render::MAX_TEXTURE_UNITS;
      size_t unit = 0;

      unit = GL_TEXTURE0_ARB;

      const bool bIsAlphaBlending = stateSet.alphablending.IsValidAndTurnedOn();
      const bool bIsColour = stateSet.colour.IsValidAndTurnedOn();

      for (size_t i = 0; i < n; i++, unit++) {
        // Activate the current texture unit
        glActiveTexture(unit);

        if (bIsAlphaBlending) {
          glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
          glBlendFunc(GL_ONE, GL_ZERO);
          glDisable(GL_BLEND);
        }

        if (bIsColour) glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

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

      glActiveTexture(GL_TEXTURE0_ARB);
      glEnable(GL_TEXTURE_2D);
    }


    cRenderVisitor::cRenderVisitor(cSceneGraph& scenegraph, render::cGraphicsContext& context)
    {
      ASSERT(scenegraph.GetRoot() != nullptr);

      cRenderGraph& rendergraph = scenegraph.GetRenderGraph();

      {
        std::vector<cRenderGraph::cRenderablePair>::iterator iter(rendergraph.items.begin());
        const std::vector<cRenderGraph::cRenderablePair>::iterator iterEnd(rendergraph.items.end());
        while(iter != iterEnd) {
          cStateSet* pStateSet = iter->first;
          const cVertexBuffer* pVertexBuffer = iter->second;

          const std::vector<float>& vertices = pVertexBuffer->GetVertices();
          //const std::vector<float>& textureCoordinates = pVertexBuffer->GetTextureCoordinates();

          switch (pStateSet->geometryType) {
            case scenegraph_common::GEOMETRY_TYPE::LINES:
            default: {
              std::vector<math::cVec2> points;
              const size_t n = vertices.size();
              ASSERT(math::IsDivisibleByTwo(n));
              for (size_t i = 0; (i + 1) < n; i += 2) {
                points.push_back(math::cVec2(vertices[i], vertices[i + 1]));
              }

              ApplyStateSet(*pStateSet);
                pRender->RenderScreenSpaceLines(points);
              UnApplyStateSet(*pStateSet);
              break;
            }
            /*case : {
              ASSERT(vertices.size() == 8);
              ASSERT(textureCoordinates.size() == 8);
              pRender->RenderScreenSpacePolygon(
                vertices[0], vertices[1], textureCoordinates[0], textureCoordinates[1],
                vertices[2], vertices[3], textureCoordinates[2], textureCoordinates[3],
                vertices[4], vertices[5], textureCoordinates[4], textureCoordinates[5],
                vertices[6], vertices[7], textureCoordinates[6], textureCoordinates[7]
              );
            }*/
          }

          //uiTriangles += 2;

          iter++;
        }
      }



      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        glLoadIdentity();

        std::vector<math::cVec2> points;

        points.push_back(math::cVec2(0.0f, 0.0f));
        points.push_back(math::cVec2(0.1f, 0.1f));
        points.push_back(math::cVec2(0.2f, -0.1f));
        points.push_back(math::cVec2(0.5f, 0.5f));
        points.push_back(math::cVec2(1.0f, 1.0f));

        pRender->RenderScreenSpaceLines(points);

        glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
    }


    cSceneGraph::cSceneGraph() :
      bIsCullingEnabled(true)
    {
      pRoot.reset(new cGroupNode);
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
      cUpdateVisitor visitor(*this);
    }

    void cSceneGraph::Cull(sampletime_t currentTime, const render::cCamera& camera)
    {
      renderGraph.Clear();
      cCullVisitor visitor(*this, camera);
    }

    void cSceneGraph::Render(sampletime_t currentTime, render::cGraphicsContext& context)
    {
      cRenderVisitor visitor(*this, context);
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

#ifdef BUILD_DEBUG
      bool _IsParentOfChild(cSceneNodeRef pChild) const;
#endif
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

#ifdef BUILD_DEBUG
    bool cSceneNodeUnitTest::_IsParentOfChild(cSceneNodeRef pChild) const
    {
      std::list<cSceneNodeRef>::const_iterator iter = std::find(children.begin(), children.end(), pChild);
      return (iter != children.end());
    }
#endif

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
        printf("cSceneGraphUnitTest 2d\n");
      }

      void Test()
      {
        counter = 0;

        cModelNode model;

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




            //pRoot->AddChild(&model);

            const sampletime_t currentTime = util::GetTimeMS();
            render::cCamera camera;

            scenegraph.Update(currentTime);
            scenegraph.Cull(currentTime, camera);
            scenegraph.Render(currentTime, *pRender);









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
