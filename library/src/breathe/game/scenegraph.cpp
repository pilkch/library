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

#include <GL/Glee.h>


#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>


#include <ode/ode.h>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>

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

#include <breathe/util/base.h>
#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>
#include <breathe/render/model/cStatic.h>

#include <breathe/physics/physics.h>
#include <breathe/physics/cContact.h>
#include <breathe/physics/cRayCast.h>
#include <breathe/physics/cPhysicsObject.h>

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
		cSceneGraph::cSceneGraph() :
			pRoot(nullptr)
		{
		}
		
		cSceneGraphSpawn::cSceneGraphSpawn()
			: v3Position(0.0f, 0.0f, 0.0f),
				v3Rotation(1.0f, 0.0f, 0.0f)
		{

		}


		cSceneGraphNode::cSceneGraphNode(cSceneGraph* p, std::string sNewFilename)
		{
			uiStatus=0;

			pLevel=p;

			sFilename=sNewFilename;
		}

		void cSceneGraphNode::Load()
		{
			LOG.Success("LevelNode", "Load");
			LOG.Success("LevelNode", "Load returning");
		}
			
		void cSceneGraphNode::Unload()
		{
			uiStatus=NODE_INACTIVE;
		}

		void cSceneGraphNode::Update(sampletime_t currentTime)
		{
			if(NODE_INACTIVE!=uiStatus)
				uiStatus--;
			if(NODE_UNLOAD==uiStatus)
				Unload();
		}

		unsigned int cSceneGraphNode::Render()
		{
			unsigned int uiTriangles = 0;

			uiTriangles+=pRender->RenderStaticModel(pRender->GetModel(sFilename + "mesh.3ds"));

			return uiTriangles;
		}
	}
}
