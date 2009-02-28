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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>


#include <GL/GLee.h>


#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


#ifdef BUILD_PHYSICS_3D
#include <ode/ode.h>
#endif

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

#include <breathe/game/cLevel.h>
#include <breathe/game/cPlayer.h>
#include <breathe/game/cPetrolBowser.h>
#include <breathe/vehicle/cPart.h>
#include <breathe/vehicle/cWheel.h>
#include <breathe/vehicle/cSeat.h>
#include <breathe/vehicle/cVehicle.h>

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

    void cSceneNode::SetDirty()
    {
      if (!bIsDirty) {
        bIsDirty = true;
        if (pParent != nullptr) pParent->SetDirty();
      }
    }

    void cSceneNode::SetLocalPosition(const math::cVec3& position)
    {
      bHasRelativePosition = true;
      relativePosition = position;
      SetDirty();
    }

    void cSceneNode::SetLocalRotation(const math::cVec3& rotation)
    {
      bHasRelativeRotation = true;
      relativeRotation = rotation;
      SetDirty();
    }

    // If we don't have a parent return our relative position, else return our parent's global position + our own
    math::cVec3 cSceneNode::GetGlobalPosition() const
    {
      if (pParent != nullptr) return pParent->GetGlobalPosition() + relativePosition;

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

    void cSceneNode::SetPosition(const math::cVec3& position)
    {
      bHasRelativePosition = true;
      relativePosition = position;
      SetDirty();
    }

    void cSceneNode::SetRotation(const math::cQuaternion& rotation)
    {
      bHasRelativeRotation = true;
      relativeRotation = rotation;
      SetDirty();
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

    void cSceneNode::_Update(cUpdateVisitor& visitor)
    {
      //if (pChild != nullptr) pChild->Update(visitor);

      visitor.Visit(*this);
    }

    void cSceneNode::_Cull(cCullVisitor& visitor)
    {
      //if (pChild != nullptr) pChild->Cull(visitor);

      visitor.Visit(*this);
    }


    void cGroupNode::_Update(cUpdateVisitor& visitor)
    {
      // If we don't have any children, return
      if (children.empty()) return;

      // Visit all child nodes
      std::vector<cSceneNodeRef>::iterator iter(children.begin());
      const std::vector<cSceneNodeRef>::iterator iterEnd(children.end());
      while (iter != iterEnd) {
        ASSERT(*iter != nullptr);
        visitor.Visit(*(*iter));

        iter++;
      }
    }

    void cGroupNode::_Cull(cCullVisitor& visitor)
    {
      // If we don't have any children, return
      if (children.empty()) return;

      // Visit all child nodes
      std::vector<cSceneNodeRef>::iterator iter(children.begin());
      const std::vector<cSceneNodeRef>::iterator iterEnd(children.end());
      while (iter != iterEnd) {
        ASSERT(*iter != nullptr);
        visitor.Visit(*(*iter));

        iter++;
      }
    }


    void cProjection2D::_Update(cUpdateVisitor& visitor)
    {
      visitor.Visit(*pChild);
    }

    void cProjection2D::_Cull(cCullVisitor& visitor)
    {
      visitor.Visit(*pChild);
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


    void cLightNode::_Update(cUpdateVisitor& visitor)
    {
      //if (pChild != nullptr) pChild->Update(visitor);

      //visitor.Visit(*this);
    }

    void cLightNode::_Cull(cCullVisitor& visitor)
    {
      //if (pChild != nullptr) pChild->Cull(visitor);

      //visitor.Visit(*this);
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


    void cPagedLODNodeChild::_Update(cUpdateVisitor& visitor)
    {
      visitor.Visit(terrain);
      visitor.Visit(trees);
      //visitor.Visit(grass);
    }
    void cPagedLODNodeChild::_Cull(cCullVisitor& visitor)
    {
      visitor.Visit(terrain);
      visitor.Visit(trees);
      //visitor.Visit(grass);
    }

    void cPagedLODNode::_Update(cUpdateVisitor& visitor)
    {
      loader.Update();
    }

    void cPagedLODNode::_Cull(cCullVisitor& visitor)
    {
      //std::vector<cGeometryNodeRef>::iterator iter(node.begin());
      //const std::vector<cGeometryNodeRef>::iterator iter(node.end());
      //while (iter != iterEnd) {
      //  visitor.Visit(*(*iter));

      //  iter++;
      //}
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

    cCullVisitor::cCullVisitor(cSceneGraph& _scenegraph) :
      scenegraph(_scenegraph)
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


    void cRenderGraph::AddRenderable(const cRenderableRef renderable)
    {
      std::map<cStateSet*, cRenderableList*>::iterator iter(mOpaque.begin());
      const std::map<cStateSet*, cRenderableList*>::iterator iterEnd(mOpaque.end());
      while (iter != iterEnd) {
        cStateSet* pState = iter->first;
        ASSERT(pState != nullptr);

        //if (*pState == renderable.state) {
        //  cRenderableList* pList = iter->second;
        //  pList->push_back(&renderable);
        //  return;
        //}

        iter++;
      }

      //cRenderableList* pList = new cRenderableList;
      //pList->push_back(&renderable);
      //mOpaque[&renderable.state] = pList;
    }

    cRenderVisitor::cRenderVisitor(cSceneGraph& _scenegraph) :
      scenegraph(_scenegraph)
    {
      ASSERT(scenegraph.GetRoot() != nullptr);

      cRenderGraph& rendergraph = scenegraph.GetRenderGraph();

      /*unsigned int uiTriangles = 0;

      iterator iter(mRenderables.begin());
      const iterator iterEnd(mRenderables.end());
      while (iter != iterEnd) {
        cStateSet* pState = iter->first;
        ASSERT(pState != nullptr);

        cRenderableList* pList = iter->second;
        ASSERT(pList != nullptr);

        PushState(pState);
        cRenderableList::iterator iterList(pList.begin());
        const cRenderableList::iterator iterListEnd(pList.end());
        while (iterList != iterListEnd) {
          cRenderable* pRenderable = *iterList;
          Render(pRenderable);
          iterList++;
        }
        PopState(pState);

        iter++;
      }*/

      if (scenegraph.pSkySystem != nullptr) {

        // http://www.flipcode.com/archives/Sky_Domes.shtml

        // Positions of all the sky bodies and clouds are computed.
        // From the position of the sun, all the colors for the bodies, clouds, and frame buffer are computed
        // Frame buffer is cleared with the computed sky color
        // Z buffer writes are disabled
        // Render all sky clouds that are marked as being a star layer (a simple texture with dots for stars)
        // Render all sky bodies
        // Render all sky clouds that are clouds and not stars

        // Cloud layers are blended onto the frame buffer.

        // In the case of sky bodies such as the sun, they are not blended, but required an alpha component so the texture does not overdraw
        // elements of the dome that are not part of the actual sun.

        // The moon has to be rendered in 2 passes. First, from the moon texture, which also has an alpha component, a mask is generated that has alpha
        // values of 1.0 for texels inside the moon and alpha values of 0.0 for texels outside the moon. This mask is rendered onto the dome without blending
        // using the current sky color. This is done to remove any stars that might appear behind the moon. Next, the actual moon texture is blended onto the
        // sky dome. The reason it is blended is because during the day, the moon will show a bit of blue or red hue of the sky.

        // Although not a real sky body, the code supports drawing of flares around the sun. These are done by creating duplicate sky
        // body like the sun, but using a flare texture instead of the sun texture. Flares are blended onto the sky dome.

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
          glLoadIdentity();
          glMultMatrixf(pRender->pFrustum->m.GetOpenGLMatrix());

          // Sky first
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


          // Render the stars and planets
          render::cParticleSystemCustomBillboard* pParticleSystemPlanets = scenegraph.pSkySystem->GetPlanetParticleSystem();
          pParticleSystemPlanets->Update(spitfire::util::GetTime());
          pParticleSystemPlanets->Render();

          pRender->ClearMaterial();

          render::cParticleSystemCustomBillboard* pParticleSystemStars = scenegraph.pSkySystem->GetStarParticleSystem();
          pParticleSystemStars->Update(spitfire::util::GetTime());
          pParticleSystemStars->Render();

          glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
      }


      // Opaque first
      {
        std::map<cStateSet*, cRenderGraph::cRenderableList*>::iterator iter(rendergraph.mOpaque.begin());
        const std::map<cStateSet*, cRenderGraph::cRenderableList*>::iterator iterEnd(rendergraph.mOpaque.end());
        while(iter != iterEnd) {
          //uiTriangles += (*iter)->Render();
          iter++;
        }
      }

      // Transparent second
      {
        std::map<float, cRenderableRef>::iterator iter(rendergraph.mTransparent.begin());
        const std::map<float, cRenderableRef>::iterator iterEnd(rendergraph.mTransparent.end());
        while(iter != iterEnd) {
          //uiTriangles += (iter->second)->Render();
          iter++;
        }
      }
    }


    cSceneGraph::cSceneGraph() :
      bIsCullingEnabled(true)
    {
      pRoot.reset(new cGroupNode);
      pSkySystem.reset(new sky::cSkySystem);
    }

    void cSceneGraph::Create()
    {
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
      pSkySystem->GetSkyDomeAtmosphereRenderer().Update(currentTime);

      cUpdateVisitor visitor(*this);

      pRender->SetClearColour(backgroundColour);
    }

    void cSceneGraph::Cull(sampletime_t currentTime)
    {
      cCullVisitor visitor(*this);
    }

    void cSceneGraph::Render(sampletime_t currentTime)
    {
      cRenderVisitor visitor(*this);
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
        printf("cSceneGraphUnitTest\n");
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
            breathe::stringstream_t o;
            o<<TEXT("cSceneGraphUnitTest FAILED No nodes added, counter should equal 0, counter=")<<counter;
            SetFailed(o.str());
          }

          cSceneNodeRef a(new cSceneNodeUnitTest);
          pRoot->AttachChild(a);

          if (counter != 1) {
            breathe::stringstream_t o;
            o<<TEXT("cSceneGraphUnitTest FAILED Added a, counter should equal 1, counter=")<<counter;
            SetFailed(o.str());
          }

          cSceneNodeRef b(new cSceneNodeUnitTest);
          a->AttachChild(b);

          if (counter != 2) {
            breathe::stringstream_t o;
            o<<TEXT("cSceneGraphUnitTest FAILED Added b, counter should equal 2, counter=")<<counter;
            SetFailed(o.str());
          }

          {
            cSceneNodeRef c(new cSceneNodeUnitTest);
            b->AttachChild(c);

            if (counter != 3) {
              breathe::stringstream_t o;
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
                breathe::stringstream_t o;
                o<<TEXT("cSceneGraphUnitTest FAILED d attached to a, counter should equal 4, counter=")<<counter;
                SetFailed(o.str());
              }

              cSceneNodeRef e(new cSceneNodeUnitTest);
              d->AttachChild(e);

              if (counter != 5) {
                breathe::stringstream_t o;
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
                breathe::stringstream_t o;
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
                breathe::stringstream_t o;
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
              breathe::stringstream_t o;
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
              breathe::stringstream_t o;
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
            breathe::stringstream_t o;
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
              breathe::stringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED f attached to a, counter should equal 4, counter=")<<counter;
              SetFailed(o.str());
            }

            cSceneNodeRef g(new cSceneNodeUnitTest);
            f->AttachChild(g);

            if (counter != 5) {
              breathe::stringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED g attached to f, counter should equal 5, counter=")<<counter;
              SetFailed(o.str());
            }

            cSceneNodeRef h(new cSceneNodeUnitTest);
            f->AttachChild(h);

            if (counter != 6) {
              breathe::stringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED h attached to f, counter should equal 6, counter=")<<counter;
              SetFailed(o.str());
            }




            //model.AddChild(&light);

            //pRoot->AddChild(&model);

            const sampletime_t currentTime = util::GetTime();
            scenegraph.Update(currentTime);
            scenegraph.Cull(currentTime);
            scenegraph.Render(currentTime);









            // Now we delete all children of a recursively
            //       root
            //     a      b
            a->DeleteAllChildrenRecursively();

            // Still referenced by variables f, g and h
            if (counter != 6) {
              breathe::stringstream_t o;
              o<<TEXT("cSceneGraphUnitTest FAILED Children of a deleted recursively, counter should equal 6, counter=")<<counter;
              SetFailed(o.str());
            }
          }

          // Ok, now f, g and h have fallen out of scope and been deleted
          // Now we should have just 4 objects left
          if (counter != 3) {
            breathe::stringstream_t o;
            o<<TEXT("cSceneGraphUnitTest FAILED f, g and h have now fallen out of scope, counter should equal 3, counter=")<<counter;
            SetFailed(o.str());
          }
        }

        // Now the whole scenegraph has fallen out of scope so our counter should be 0 again
        if (counter != 0) {
          breathe::stringstream_t o;
          o<<TEXT("cSceneGraphUnitTest FAILED All children of the scenegraph have now fallen out of scope, counter should equal 0, counter=")<<counter;
          SetFailed(o.str());
        }
      }
    };

    cSceneGraphUnitTest gSceneGraphUnitTest;
#endif
  }
}
