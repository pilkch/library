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
#include <BREATHE/cBreathe.h>
#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>
#include <BREATHE/UTIL/cXML.h>

#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cOctree.h>
#include <BREATHE/MATH/cColour.h>

#include <BREATHE/UTIL/cBase.h>
#include <BREATHE/RENDER/MODEL/cMesh.h>
#include <BREATHE/RENDER/MODEL/cModel.h>
#include <BREATHE/RENDER/MODEL/cStatic.h>

#include <BREATHE/PHYSICS/cPhysics.h>
#include <BREATHE/PHYSICS/cContact.h>
#include <BREATHE/PHYSICS/cRayCast.h>
#include <BREATHE/PHYSICS/cPhysicsObject.h>

#include <BREATHE/RENDER/cTexture.h>
#include <BREATHE/RENDER/cTextureAtlas.h>
#include <BREATHE/RENDER/cMaterial.h>
#include <BREATHE/RENDER/cRender.h>

#include <BREATHE/GAME/cLevel.h>

#include <BREATHE/GAME/cPlayer.h>
#include <BREATHE/GAME/cPetrolBowser.h>
#include <BREATHE/VEHICLE/cPart.h>
#include <BREATHE/VEHICLE/cWheel.h>
#include <BREATHE/VEHICLE/cSeat.h>
#include <BREATHE/VEHICLE/cVehicle.h>

BREATHE::cLevel* pLevel = NULL;

const unsigned int uiNodeNameDisplayTime = 100;

namespace BREATHE
{
	/*
	std::string s=BREATHE::FILESYSTEM::GetMD5(sFilename);*/

	cLevel::cLevel()
	{
		fWaterLevel = 1.0f;
		fPreviousTime=0.0f;

		pRender->pLevel=this;

		pCurrentNode = NULL;

		uiDisplayNodeName = uiNodeNameDisplayTime;
	}

	cLevel::~cLevel()
	{
	}

	bool cLevel::LoadXML(std::string sNewFilename)
	{
		sFilename=sNewFilename;
		bool bNodes=false;

		cLevelSpawn *pSpawn;

		{
			LOG.Success("Level", "cLevelNode::Load " + sNewFilename);
			BREATHE::XML::cNode root(sNewFilename);

			BREATHE::XML::cNode::iterator iter(root);
			if (!iter) return BREATHE::BAD;
			
			iter.FindChild("level");
			if(iter)
			{
				iter.GetAttribute("fWaterLevel", &fWaterLevel);

				iter.FirstChild();

				while(iter)
				{
					if("nodes" == iter.GetName())
					{
						iter.GetAttribute("fWidth", &fNodeWidth);
						iter.GetAttribute("uiWidth", &uiNodeWidth);
						iter.GetAttribute("uiHeight", &uiNodeHeight);
						iter.GetAttribute("uiHeightMapPixelWidth", &uiNodeHeightMapPixelWidth);
						iter.GetAttribute("uiHeightMapPixelHeight", &uiNodeHeightMapPixelHeight);

						BREATHE::XML::cNode::iterator iterParent = iter;
							iter.FindChild("node");
							while(iter)
							{
								std::string sPath;
								if(iter.GetAttribute("path", &sPath))
									LoadNode(sPath);

								iter.Next("node");
							};
						iter = iterParent;
					}
					else if("spawns" == iter.GetName())
					{
						BREATHE::XML::cNode::iterator iterParent = iter;
							iter.FindChild("spawn");
							while(iter)
							{
								pSpawn = new cLevelSpawn();
								vSpawn.push_back(pSpawn);
	
								iter.GetAttribute("position", &pSpawn->v3Position);
								iter.GetAttribute("rotation", &pSpawn->v3Rotation);

								iter.Next("spawn");
							};
						iter = iterParent;
					}

					iter++;
				};

			}
		}

		return BREATHE::GOOD;
	}

	bool cLevel::Load(std::string sNewFilename)
	{
		bool bResult = BREATHE::GOOD;

		if (LoadXML(sNewFilename) != BREATHE::GOOD) bResult = BREATHE::BAD;
      

		// We don't have any spawns yet, add a default one
		if(0==vSpawn.size()) {
			LOG.Error("Level", "No spawns defined");
			cLevelSpawn *p = new cLevelSpawn();
			p->v3Position = MATH::cVec3(0.0f, 0.0f, 0.0f);
			p->v3Rotation = MATH::cVec3(0.0f, 0.0f, 90.0f);
			vSpawn.push_back(p);
		}

		unsigned int i=0;
		unsigned int n=vCubemap.size();

		if(0 == n)
		{
			LOG.Error("Level", "No cubemaps defined");
			bResult = BREATHE::BAD;
		}
		else
		{
			for(i=0;i<n;i++)
				pRender->AddCubeMap(vCubemap[i]->sFilename);
		}

		return bResult;
	}

	void cLevel::LoadNode(std::string sNewFilename)
	{
		RENDER::MODEL::cStatic *p=pRender->AddModel("data/level/" + sNewFilename + "/mesh.3ds");
		
		if(p)
		{
			cLevelNode *pNode=new cLevelNode(this, "data/level/" + sNewFilename + "/");
			pNode->Load();

			/*
			unsigned int i=0;
			unsigned int n=p->vCamera.size();
			for(i=0;i<n;i++)
			{
				LOG.Success("Level", "Spawn");

				vSpawn.push_back(new cLevelSpawn());
				cLevelSpawn *pSpawn=vSpawn.back();

				MATH::cVec3 cam=p->vCamera[i]->eye;
				MATH::cVec3 objPos=p->vCamera[i]->target;

				MATH::cVec3 objToCamProj(cam.x - objPos.x, 0.0f, cam.z - objPos.z);
				
				float angleCosine;
				
				MATH::cVec3 v3out;

				objToCamProj.Normalize();

				// compute the angle
				angleCosine = cam.DotProduct(objToCamProj);

				float a=acosf(angleCosine)*MATH::c180_DIV_PI;

				// perform the rotation. The if statement is used for stability reasons
				// if the lookAt and objToCamProj vectors are too close together then 
				// |angleCosine| could be bigger than 1 due to lack of precision
				if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
					v3out.z=90.0f+a;
				else
					v3out.z=-90.0f;

				pSpawn->v3Rotation=v3out;
				pSpawn->v3Position=p->vCamera[i]->eye;
				LOG.Error("Level", "Upate in 3ds and remove this");
				pSpawn->v3Position.z=-1.5f;
			}*/

			vNode.push_back(pNode);
		}
		else
			LOG.Error("Node", "Mesh not found " + sNewFilename);
	}
	
	void cLevel::LoadCubemap(std::string line)
	{
		vCubemap.push_back(new cLevelCubemap());

		cLevelCubemap *p=vCubemap.back();

		std::stringstream stm(line);
		stm >> p->sFilename;

		stm >> p->v3Position.x;
		stm >> p->v3Position.y;
		stm >> p->v3Position.z;
	}

	/*void cModel::staticMeshAddToWorld(int i, WORLD * world, 
																							COLLISIONPACKET * collisionPacket, 
																							float x, float y, float z)
	{
		if(i>-1 && i<animation.size())
		{
			cModel_StaticMesh * m=&vStatic[i];

			if(NULL==m)
			{
				LOGFAIL("Model", "vStatic failed");
				return;
			}

			int index=0;
			int f=0;
			const float ex=1.0f/collisionPacket->eRadius.x;
			const float ey=1.0f/collisionPacket->eRadius.y;
			const float ez=1.0f/collisionPacket->eRadius.z;

			x*=ex;
			y*=ey;
			z*=ez;

			TRI_T2_N3_V3 * t;
			TRI_COL temp;

			for(f=0;f<m->uiTriangles;f++)
			{
				t=&m->mesh[f];

				temp.p0.vx=t->p0.vx*ex+x;
				temp.p0.vy=t->p0.vy*ey+y;
				temp.p0.vz=t->p0.vz*ez+z;
				
				temp.p1.vx=t->p1.vx*ex+x;
				temp.p1.vy=t->p1.vy*ey+y;
				temp.p1.vz=t->p1.vz*ez+z;
				
				temp.p2.vx=t->p2.vx*ex+x;
				temp.p2.vy=t->p2.vy*ey+y;
				temp.p2.vz=t->p2.vz*ez+z;

				world->AddTriangle(&temp);
			}
		}
	}*/

	void cLevel::Update(float fCurrentTime)
	{
		//TODO: Calculate the current nodes
		if(pRender->pFrustum->eye.x > 0.0f && pRender->pFrustum->eye.y > 0.0f)
		{
			unsigned int currentX=static_cast<unsigned int>(pRender->pFrustum->eye.x/fNodeWidth);
			unsigned int currentY=static_cast<unsigned int>(pRender->pFrustum->eye.y/fNodeWidth);
			
			unsigned int uiCurrentNode = currentY * uiNodeWidth + currentX;

			if(uiCurrentNode < vNode.size())
			{
				cLevelNode* pNode = vNode[uiCurrentNode];
				
				// Have we changed nodes in the last time step?  
				if(pNode && pCurrentNode != pNode)
				{
					if(pCurrentNode && (pCurrentNode->sName != pNode->sName))
						uiDisplayNodeName = uiNodeNameDisplayTime;

					// Anything that must happen on the boundary happens here
					pCurrentNode = pNode;
				}
			}
		}

		{
			std::list<PHYSICS::cPhysicsObject *>::iterator iter=PHYSICS::GetObjectListBegin();
			std::list<PHYSICS::cPhysicsObject *>::iterator end=PHYSICS::GetObjectListEnd();

			while(end != iter)
				(*iter++)->Update(fCurrentTime);
		}


		if((fCurrentTime-fPreviousTime)>1000.0f)
		{
			fPreviousTime=fCurrentTime;

			unsigned int n=vNode.size();
			unsigned int i=0;
			for(i=0;i<n;i++)
				vNode[i]->Update(fCurrentTime);
		}

		
		{
			VEHICLE::cVehicle *pVehicle=NULL;
		
			std::list<VEHICLE::cVehicle *>::iterator iter=lVehicle.begin();
			while(iter!=lVehicle.end())
			{
				(*iter)->Update(fCurrentTime);
				iter++;
			};
		}

		// Update the counter that says whether to display the node name
		if(uiDisplayNodeName) uiDisplayNodeName--;
	}

	unsigned int cLevel::Render(float fCurrentTime)
	{
		unsigned int uiTriangles=0;
		unsigned int i=0;
		unsigned int n=vNode.size();
		for(i=0;i<n;i++)
			uiTriangles += vNode[i]->Render();

		//n=vStatic.size();
		//for(i=0;i<n;i++)
		//	uiTriangles+=RenderStaticModel(vStatic[i]);));
		
		std::list<PHYSICS::cPhysicsObject*>::iterator iter = lPhysicsObject.begin();
		std::list<PHYSICS::cPhysicsObject*>::iterator end = lPhysicsObject.end();
		while(end != iter)
		{
			glPushMatrix();
				glMultMatrixf((*iter)->m);
				
				uiTriangles+=pRender->RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>((*iter++)->pModel));

				//pRender->SetMaterial();
			glPopMatrix();
		}

		return uiTriangles;
	}

	unsigned int cLevel::RenderVehicles(float fCurrentTime, VEHICLE::cVehicle *pOwnVehicle)
	{
		unsigned int uiTriangles=0;
		VEHICLE::cVehicle *pVehicle=NULL;
		
		std::list<VEHICLE::cVehicle *>::iterator iter=lVehicle.begin();
		while(iter!=lVehicle.end())
		{
			pVehicle=*iter;

			if((NULL == pOwnVehicle) || (pVehicle != pOwnVehicle))
			{
				glPushMatrix();
					glMultMatrixf(pVehicle->m);
					uiTriangles+=pRender->RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>(pVehicle->pModel), BREATHE::MATH::cColour(1.0f, 0.0f, 0.0f));
				glPopMatrix();


				glPushMatrix();
					glMultMatrixf(pVehicle->lfWheel_->m);
					uiTriangles+=pRender->RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>(pVehicle->lfWheel_->pModel));
				glPopMatrix();
				
				glPushMatrix();
					glMultMatrixf(pVehicle->lrWheel_->m);
					uiTriangles+=pRender->RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>(pVehicle->lfWheel_->pModel));
				glPopMatrix();
				
				
				BREATHE::MATH::cMat4 r;
				r.SetRotationZ(BREATHE::MATH::cPI);

				glPushMatrix();
					glMultMatrixf(pVehicle->rfWheel_->m*r);
					uiTriangles+=pRender->RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>(pVehicle->lfWheel_->pModel));
				glPopMatrix();

				glPushMatrix();
					glMultMatrixf(pVehicle->rrWheel_->m*r);
					uiTriangles+=pRender->RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>(pVehicle->lfWheel_->pModel));
				glPopMatrix();
			}

			iter++;
		};

		return uiTriangles;
	}

	cLevelSpawn cLevel::GetSpawn()
	{
		unsigned int i=rand()%(vSpawn.size());
		return *vSpawn[i];
	}
		
	cLevelSpawn cLevel::GetSpawn(MATH::cVec3 &p)
	{
		//Have to have a spawn in the level before calling this
		unsigned int i=0;
		unsigned int n=vSpawn.size();

		if(n<1) LOG.Error("Level", "No spawns defined");

		float d=(vSpawn[0]->v3Position-p).GetLength();
		float t=0.0f;
		cLevelSpawn *s=vSpawn[0];

    for(i=1;i<n;i++)
		{
			t=(vSpawn[i]->v3Position-p).GetLength();
			if(t<d)
			{
				d=t;
				s=vSpawn[i];
			}
		}

		return *s;
	}

	void cLevel::AddPhysicsObject(PHYSICS::cPhysicsObject *d)
	{
		PHYSICS::AddPhysicsObject(d);
		lPhysicsObject.push_back(d);
	}

	void cLevel::RemovePhysicsObject(PHYSICS::cPhysicsObject *d)
	{
		PHYSICS::RemovePhysicsObject(d);
		lPhysicsObject.remove(d);
	}


	void cLevel::AddVehicle(VEHICLE::cVehicle *v)
	{
		lVehicle.push_back(v);
	}

	void cLevel::RemoveVehicle(VEHICLE::cVehicle *v)
	{
		lVehicle.remove(v);
	}

	RENDER::cTexture *cLevel::FindClosestCubeMap(MATH::cVec3 pos)
	{
		unsigned int n=vCubemap.size();

		if(n<1)
			return NULL;

		unsigned int i=0;
		cLevelCubemap *c=vCubemap[0];
		float f=(vCubemap[0]->v3Position-pos).GetLength();
		float a=0.0f;

		for(i=1;i<n;i++)
		{
			a=(vCubemap[i]->v3Position-pos).GetLength();
			if(a<f)
			{
				c=vCubemap[i];
				f=a;
			}
		}

		return pRender->GetCubeMap(c->sFilename);
	}

	VEHICLE::cVehicle *cLevel::FindClosestVehicle(MATH::cVec3 pos, float fMaxDistance)
	{
		if(lVehicle.size()<1)
			return NULL;

		std::list<VEHICLE::cVehicle *>::iterator iter=lVehicle.begin();

		float d=fMaxDistance;
		float t=fMaxDistance;
		BREATHE::VEHICLE::cVehicle *v=NULL;

		while(iter!=lVehicle.end())
		{
			t=((*iter)->p-pos).GetLength();
			if(t<d)
			{
				d=t;
				v=(*iter);
			}

			iter++;
		}

		return v;
	}

	
	cLevelSpawn::cLevelSpawn()
		: v3Position(0.0f, 0.0f, 0.0f),
			v3Rotation(1.0f, 0.0f, 0.0f)
	{

	}


	cLevelNode::cLevelNode(cLevel *p, std::string sNewFilename)
	{
		uiStatus=0;

		pLevel=p;

		sFilename=sNewFilename;
	}

	void cLevelNode::Load()
	{
		LOG.Success("LevelNode", "Load");
			
		uiStatus=NODE_ACTIVE;		
		
		bool bModels=false;
		bool bCubemaps=false;
		

		XML::cNode root(sFilename + "node.xml");

		XML::cNode::iterator iter(root);
		if (!iter) return;
			
		iter.FindChild("node");
		if(iter)
		{
			iter.GetAttribute("crc", &sCRC);
			iter.GetAttribute("name", &sName);
		}

		iter.FirstChild();
		while(iter)
		{
			if("fog" == iter.GetName())
			{
        iter.GetAttribute("colour", &colourFog);
				iter.GetAttribute("distance", &fFogDistance);
			}
			else if("models" == iter.GetName())
			{
				BREATHE::XML::cNode::iterator iterParent = iter;
					iter.FindChild("model");
					while(iter)
					{
						std::string sPath;
						if(iter.GetAttribute("path", &sPath))
						{
							cLevelModel* pModel = new cLevelModel();
							vModel.push_back(pModel);

							// Pre load the mesh for this model
							pModel->pModel = pRender->AddModel(sPath + "/mesh.3ds");

							iter.GetAttribute("position", &pModel->p);

							MATH::cVec3 v;
							if(iter.GetAttribute("position", &v))
                pModel->m.SetTranslation(v);
						}

						iter.Next("model");
					};
				iter = iterParent;
			}
			else if("cubemaps" == iter.GetName())
			{
				BREATHE::XML::cNode::iterator iterParent = iter;
					iter.FindChild("cubemap");
					while(iter)
					{
						std::string sPath;
						if(iter.GetAttribute("texture", &sPath))
							pLevel->LoadCubemap(sPath);

						//TODO: position="10.0, 10.0, 0.0"

						iter.Next("cubemap");
					};
				iter = iterParent;
			}

			iter++;
		};

		LOG.Success("LevelNode", "Load returning");
	}
		
	void cLevelNode::Unload()
	{
		uiStatus=NODE_INACTIVE;
	}

	void cLevelNode::Update(float fCurrentTime)
	{
		if(NODE_INACTIVE!=uiStatus)
			uiStatus--;
		if(NODE_UNLOAD==uiStatus)
			Unload();
	}

	unsigned int cLevelNode::Render()
	{
		unsigned int uiTriangles = 0;

		uiTriangles+=pRender->RenderStaticModel(pRender->GetModel(sFilename + "mesh.3ds"));

		std::vector<BREATHE::cLevelModel*>::iterator iter = vModel.begin();
		std::vector<BREATHE::cLevelModel*>::iterator end = vModel.end();
		
		while(end != iter)
		{
			glPushMatrix();
				glMultMatrixf((*iter)->m);
				uiTriangles+=pRender->RenderStaticModel((*iter)->pModel);
			glPopMatrix();

			iter++;
		}

		return uiTriangles;
	}
}
