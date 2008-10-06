#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <string>

// writing on a text file
#include <iostream>
#include <fstream>

#include <sstream>
#include <vector>
#include <map>
#include <list>
#include <set>

// Boost includes
#include <boost/shared_ptr.hpp>

#include <GL/GLee.h>


#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


#ifdef BUILD_PHYSICS_3D
#include <ode/ode.h>
#endif

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cSmartPtr.h>
#include <breathe/util/cString.h>
#include <breathe/util/log.h>
#include <breathe/util/cTimer.h>
#include <breathe/util/unittest.h>

#include <breathe/storage/filesystem.h>
#include <breathe/storage/xml.h>

#include <breathe/math/math.h>
#include <breathe/math/cVec2.h>
#include <breathe/math/cVec3.h>
#include <breathe/math/cVec4.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cPlane.h>
#include <breathe/math/cQuaternion.h>
#include <breathe/math/cFrustum.h>
#include <breathe/math/cOctree.h>
#include <breathe/math/cColour.h>
#include <breathe/math/geometry.h>

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/physics/physics.h>

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cMaterial.h>
#include <breathe/render/cRender.h>

#include <breathe/game/scenegraph.h>

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
  namespace scenegraph
  {
    cSceneNode::cSceneNode() :
      bIsVisible(true),
      bIsDirty(true),
      bHasRelativePosition(false),
      bHasRelativeRotation(false)
    {
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
      listOpaque.push_back(renderable);
    }


    cRenderVisitor::cRenderVisitor(cSceneGraph& _scenegraph) :
      scenegraph(_scenegraph)
    {
      ASSERT(scenegraph.GetRoot() != nullptr);

      cRenderGraph& rendergraph = scenegraph.GetRenderGraph();

      unsigned int uiTriangles = 0;

      // Opaque first
      {
        std::list<cRenderableRef>::iterator iter(rendergraph.listOpaque.begin());
        const std::list<cRenderableRef>::iterator iterEnd(rendergraph.listOpaque.end());
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

    void cSceneGraph::Cull(sampletime_t currentTime)
    {
      cCullVisitor visitor(*this);
    }

    void cSceneGraph::Render(sampletime_t currentTime)
    {
      cRenderVisitor visitor(*this);
    }

#ifdef BUILD_DEBUG
    class cSceneGraphUnitTest : protected breathe::util::cUnitTestBase
    {
    public:
      cSceneGraphUnitTest() :
        cUnitTestBase("cSceneGraphUnitTest")
      {
        printf("cSceneGraphUnitTest\n");
      }

      void Test()
      {
        cModelNode model;

        cLightNode light;
        //model.AddChild(&light);

        cSceneGraph scenegraph;
        cSceneNodeRef pRoot = scenegraph.GetRoot();
        //pRoot->AddChild(&model);

        const sampletime_t currentTime = util::GetTime();
        scenegraph.Update(currentTime);
        scenegraph.Cull(currentTime);
        scenegraph.Render(currentTime);
      }
    };

    cSceneGraphUnitTest gSceneGraphUnitTest;
#endif
	}
}
