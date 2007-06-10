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

#include <BREATHE/PHYSICS/cPhysicsObject.h>
#include <BREATHE/PHYSICS/cPhysics.h>
#include <BREATHE/PHYSICS/cContact.h>



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
		LOG.Success("Delete", "Static Mesh");
		std::map<std::string, RENDER::MODEL::cStatic*>::iterator iter=mStatic.begin();
		while(iter!=mStatic.end())
		{
			SAFE_DELETE(iter->second);
			iter++;
		};
	}

	bool cLevel::Load(std::string sNewFilename)
	{
		sFilename=sNewFilename;
		
		bool bNodes=false;

		cLevelSpawn *pSpawn;

		{
			LOG.Success("Level", "cLevelNode::Load " + sNewFilename);
			BREATHE::XML::cNode root(sNewFilename);

			BREATHE::XML::cNode* p=root.FindChild("level");

			if(p)
			{
				p->GetAttribute("fWaterLevel", &fWaterLevel);

				p=p->FirstChild();

				while(p)
				{
					if("nodes" == p->sName)
					{
						p->GetAttribute("fWidth", &fNodeWidth);
						p->GetAttribute("uiWidth", &uiNodeWidth);
						p->GetAttribute("uiHeight", &uiNodeHeight);
						p->GetAttribute("uiHeightMapPixelWidth", &uiNodeHeightMapPixelWidth);
						p->GetAttribute("uiHeightMapPixelHeight", &uiNodeHeightMapPixelHeight);

						BREATHE::XML::cNode *pParent=p;
							p=p->FindChild("node");
							while(p)
							{
								std::string sPath;
								if(p->GetAttribute("path", &sPath))
									LoadNode(sPath);

								p=p->Next("node");
							};
						p=pParent;
					}
					else if("spawns" == p->sName)
					{
						BREATHE::XML::cNode *pParent=p;
							p=p->FindChild("spawn");
							while(p)
							{
								pSpawn = new cLevelSpawn();
								vSpawn.push_back(pSpawn);
	
								p->GetAttribute("position", &pSpawn->v3Position);
								p->GetAttribute("rotation", &pSpawn->v3Rotation);

								p=p->Next("spawn");
							};
						p=pParent;
					}

					p=p->Next();
				};

			}
		}


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

		if(0==n) {
			LOG.Error("Level", "No cubemaps defined");
			return BREATHE::BAD;
		}

		for(i=0;i<n;i++)
			pRender->AddCubeMap(vCubemap[i]->sFilename);


		return BREATHE::GOOD;
	}

	void cLevel::LoadNode(std::string sNewFilename)
	{
		RENDER::MODEL::cStatic *p=AddModel("data/level/" + sNewFilename + "/mesh.3ds");
		
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
			LOG.Error("Node", "Mesh not found" + sNewFilename);
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

	void cLevel::TransformModels()
	{
		RENDER::cTexture *t=NULL;
		RENDER::MATERIAL::cMaterial *mat=NULL;

		RENDER::MODEL::cStatic *s=NULL;
		RENDER::MODEL::cMesh *m;
		float *fTextureCoords=NULL;
		unsigned int nMeshes=0;
		unsigned int uiTriangles=0;
		unsigned int nTexcoords=0;
		unsigned int mesh=0;
		unsigned int texcoord=0;
		unsigned int triangle=0;

		//Transform uv texture coordinates
		std::map<std::string, RENDER::MODEL::cStatic*>::iterator iter=mStatic.begin();
		for(;iter!=mStatic.end();iter++)
		{
			s=iter->second;

			if(s)
			{
				nMeshes=s->vMesh.size();

				std::ostringstream sOut;
				sOut<<nMeshes;
				LOG.Success("Transform", "UV model=" + iter->first + " meshes=" + sOut.str());

				for(mesh=0;mesh<nMeshes;mesh++)
				{
					m=s->vMesh[mesh];
					fTextureCoords=&m->vTextureCoord[0];
					nTexcoords=m->vTextureCoord.size();

					mat=pRender->GetMaterial(m->sMaterial);

					if(mat)
					{
						if(mat->vLayer.size() > 0)
						{
							t = mat->vLayer[0]->pTexture;//t=pRender->GetTexture(mat->vLayer[0]->sTexture);
						
							if(t)
							{
								for(texcoord=0;texcoord<nTexcoords;texcoord+=2)
									t->Transform(fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
							}
							else
								LOG.Error("Transform", "Texture not found " + mat->vLayer[0]->sTexture);
						}
						else
							LOG.Error("Transform", "Material doesn't have any layers");
					}
					else
						LOG.Error("Transform", "Material not found " + m->sMaterial);
				}
			}
			else
				LOG.Error("Transform", "Model==NULL");
		}


		float *fNormals=NULL;

		//Calculate normals
		for(iter=mStatic.begin();iter!=mStatic.end();iter++)
		{
			LOG.Success("Transform", "Normals " + iter->first);

			s=iter->second;
			
			if(s)
			{
				nMeshes=s->vMesh.size();
				
				for(mesh=0;mesh<nMeshes;mesh++)
				{
					m=s->vMesh[mesh];
					fNormals=&m->vNormal[0];

					/*Init all vertex normals to zero

					for all faces:
						compute face normal  
						  
					for every vertex in every face:
						add face normal to vertex normal
						for all adjacent faces:
								if the dotproduct of the face normal and the adjacentface normal is > 0.71:
										add adjacentface normal to vertex normal

					for all vertex normals:
						normalize vertex normal*/
				}
			}
		}


		//Swap sub meshes around to optimise rendering
		RENDER::MODEL::cMesh *pTemp;
		unsigned int uiPass=0;
		unsigned int i=0;
		unsigned int uiMode0=0;
		unsigned int uiMode1=0;

		for(iter=mStatic.begin();iter!=mStatic.end();iter++)
		{
			LOG.Success("Transform", "Optimising " + iter->first);

			s=iter->second;

			if(s)
			{
				nMeshes=s->vMesh.size();

				for(uiPass=1; uiPass < nMeshes; uiPass++) 
				{
					for (i=0; i < nMeshes-uiPass; i++) 
					{
						uiMode0=pRender->GetMaterial(s->vMesh[i]->sMaterial)->vLayer[0]->uiTextureMode;

						//x[i] > x[i+1]
						if(RENDER::TEXTURE_MASK==uiMode0 || RENDER::TEXTURE_BLEND==uiMode0)
						{
							pTemp = s->vMesh[i]; 
							s->vMesh[i] = s->vMesh[i+1]; 
							s->vMesh[i+1] = pTemp;
						}
					}
				}
			}
		}

		LOG.Success("Level", "TransformModels returning");
	}

	RENDER::MODEL::cStatic *cLevel::AddModel(std::string sNewfilename)
	{
		RENDER::MODEL::cStatic *pModel=mStatic[sNewfilename];

		if(pModel)
			return pModel;
		
		pModel=new RENDER::MODEL::cStatic();

		if(pModel->Load(sNewfilename))
		{
			mStatic[sNewfilename]=pModel;

			unsigned int i=0;
			unsigned int n=pModel->vMesh.size();
			for(i=0;i<n;i++)
				pRender->AddMaterial(pModel->vMesh[i]->sMaterial);
				//pModel->vMesh[i]->pMaterial = pRender->AddMaterial(pModel->vMesh[i]->sMaterial);
			
			return pModel;
		}

		return NULL;
	}

	RENDER::MODEL::cStatic *cLevel::GetModel(std::string sFilename)
	{
		RENDER::MODEL::cStatic *pModel=mStatic[sFilename];
		if(pModel)
			return pModel;
		
		std::cout<<"Couldn't find "<<sFilename<<std::endl;

		return NULL;
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
			std::list<PHYSICS::cPhysicsObject *>::iterator iter=pPhysics->GetObjectListBegin();
			std::list<PHYSICS::cPhysicsObject *>::iterator end=pPhysics->GetObjectListEnd();

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
				
				uiTriangles+=RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>((*iter++)->pModel), BREATHE::MATH::cVec3(0.0f, 0.0f, 0.0f));

				//pRender->SetMaterial();
			glPopMatrix();
		}

		n=vCubemap.size();
		for(i=0;i<n;i++)
		{
			glPushMatrix();
				glTranslatef(vCubemap[i]->v3Position.x, vCubemap[i]->v3Position.y, vCubemap[i]->v3Position.z);
				uiTriangles+=RenderStaticModel(GetModel("data/props/static/cubemap/mesh.3ds"), vCubemap[i]->v3Position);
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
					uiTriangles+=RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>(pVehicle->pModel), BREATHE::MATH::cVec3(0.0f, 0.0f, 0.0f), BREATHE::MATH::cColour(1.0f, 0.0f, 0.0f));
				glPopMatrix();


				glPushMatrix();
					glMultMatrixf(pVehicle->lfWheel_->m);
					uiTriangles+=RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>(pVehicle->lfWheel_->pModel), BREATHE::MATH::cVec3(0.0f, 0.0f, 0.0f));
				glPopMatrix();
				
				glPushMatrix();
					glMultMatrixf(pVehicle->lrWheel_->m);
					uiTriangles+=RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>(pVehicle->lfWheel_->pModel), BREATHE::MATH::cVec3(0.0f, 0.0f, 0.0f));
				glPopMatrix();
				
				
				BREATHE::MATH::cMat4 r;
				r.SetRotationZ(BREATHE::MATH::cPI);

				glPushMatrix();
					glMultMatrixf(pVehicle->rfWheel_->m*r);
					uiTriangles+=RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>(pVehicle->lfWheel_->pModel), BREATHE::MATH::cVec3(0.0f, 0.0f, 0.0f));
				glPopMatrix();

				glPushMatrix();
					glMultMatrixf(pVehicle->rrWheel_->m*r);
					uiTriangles+=RenderStaticModel(static_cast<BREATHE::RENDER::MODEL::cStatic *>(pVehicle->lfWheel_->pModel), BREATHE::MATH::cVec3(0.0f, 0.0f, 0.0f));
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
		pPhysics->AddPhysicsObject(d);
		lPhysicsObject.push_back(d);
	}

	void cLevel::RemovePhysicsObject(PHYSICS::cPhysicsObject *d)
	{
		pPhysics->RemovePhysicsObject(d);
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


	unsigned int cLevel::RenderStaticModel(RENDER::MODEL::cStatic *p, MATH::cVec3 pos, MATH::cColour colour)
	{	
		if(NULL==p)
			return 0;

		bool bUseColour=(0.0f!=colour.r || 0.0f!=colour.g || 0.0f!=colour.b);				

		if(bUseColour)
			pRender->SetColour(colour);

		unsigned int triangle=0;
		unsigned int texcoord=0;
		unsigned int vert=0;
		unsigned int mesh=0;
		unsigned int uiTriangles=0;
		unsigned int nMeshes=0;
		unsigned int nTotalTriangles=0;

		std::vector<RENDER::MODEL::cMesh*> vMesh=p->vMesh;
		RENDER::MODEL::cMesh *m=NULL;

		nMeshes=vMesh.size();

		for(mesh=0;mesh<nMeshes;mesh++)
		{
			m=vMesh[mesh];

			float *fVertices=&m->vVertex[0];
			float *fTextureCoords=&m->vTextureCoord[0];
			
			uiTriangles=m->uiTriangles;
			nTotalTriangles+=uiTriangles;

			
			if(NULL == m->pMaterial)
				m->pMaterial = pRender->GetMaterial(m->sMaterial);
			pRender->SetMaterial(m->pMaterial, pos);

				texcoord=0;
				vert=0;
				MATH::cVec3 v0;
				MATH::cVec3 v1;
				MATH::cVec3 v2;
				MATH::cVec3 n;

				glBegin(GL_TRIANGLES);
				
					if(1==pRender->uiActiveUnits)
						for(triangle=0;triangle<uiTriangles;triangle++, vert+=9, texcoord+=6)
						{						
							v0.Set(&fVertices[vert]);
							v1.Set(&fVertices[vert+3]);
							v2.Set(&fVertices[vert+6]);
							n.Cross(v0-v2, v2-v1);
							n.Normalize();

							glNormal3f(n.x, n.y, n.z);

							glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
							glVertex3f(fVertices[vert], fVertices[vert+1], fVertices[vert+2]);

							glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
							glVertex3f(fVertices[vert+3], fVertices[vert+4], fVertices[vert+5]);

							glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
							glVertex3f(fVertices[vert+6], fVertices[vert+7], fVertices[vert+8]);
						}
					else if(2==pRender->uiActiveUnits)
						for(triangle=0;triangle<uiTriangles;triangle++, vert+=9, texcoord+=6)
						{
							v0.Set(&fVertices[vert]);
							v1.Set(&fVertices[vert+3]);
							v2.Set(&fVertices[vert+6]);
							n.Cross(v0-v2, v2-v1);
							n.Normalize();

							glNormal3f(n.x, n.y, n.z);
							glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
							glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
							glVertex3f(fVertices[vert], fVertices[vert+1], fVertices[vert+2]);

							//n.Cross(v1-v0, v0-v2);

							//glNormal3f(n.x, n.y, n.z);
							glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
							glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
							glVertex3f(fVertices[vert+3], fVertices[vert+4], fVertices[vert+5]);

							//n.Cross(v2-v1, v1-v0);

							//glNormal3f(n.x, n.y, n.z);
							glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
							glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
							glVertex3f(fVertices[vert+6], fVertices[vert+7], fVertices[vert+8]);
						}
					else if(3==pRender->uiActiveUnits)
						for(triangle=0;triangle<uiTriangles;triangle++, vert+=9, texcoord+=6)
						{							
							glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
							glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
							glMultiTexCoord2f( GL_TEXTURE2, fTextureCoords[texcoord], fTextureCoords[texcoord+1]);
							glVertex3f(fVertices[vert], fVertices[vert+1], fVertices[vert+2]);

							glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
							glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
							glMultiTexCoord2f( GL_TEXTURE2, fTextureCoords[texcoord+2], fTextureCoords[texcoord+3]);
							glVertex3f(fVertices[vert+3], fVertices[vert+4], fVertices[vert+5]);

							glMultiTexCoord2f( GL_TEXTURE0, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
							glMultiTexCoord2f( GL_TEXTURE1, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
							glMultiTexCoord2f( GL_TEXTURE2, fTextureCoords[texcoord+4], fTextureCoords[texcoord+5]);
							glVertex3f(fVertices[vert+6], fVertices[vert+7], fVertices[vert+8]);
						}
					else if(pRender->bCubemap)
					{
						std::ostringstream t;
						t << pRender->uiActiveUnits;
						LOG.Error("RenderStaticModel", "Invalid texture unit count " + t.str());
					}
					
				glEnd();

		}

		if(bUseColour)
			pRender->ClearColour();
		
		return uiTriangles;
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

		XML::cNode* p=root.FindChild("node");

		if(p)
		{
			p->GetAttribute("crc", &sCRC);
			p->GetAttribute("name", &sName);
		}

		p=p->FirstChild();
		while(p)
		{
			if("fog" == p->sName)
			{
        p->GetAttribute("colour", &colourFog);
				p->GetAttribute("distance", &fFogDistance);
			}
			else if("models" == p->sName)
			{
				BREATHE::XML::cNode *pParent=p;
					p=p->FindChild("model");
					while(p)
					{
						std::string sPath;
						if(p->GetAttribute("path", &sPath))
						{
							cLevelModel* pModel = new cLevelModel();
							vModel.push_back(pModel);

							// Pre load the mesh for this model
							pModel->pModel = pLevel->AddModel(sPath + "/mesh.3ds");

							p->GetAttribute("position", &pModel->p);

							MATH::cVec3 v;
							if(p->GetAttribute("position", &v))
                pModel->m.SetTranslation(v);
						}

						p=p->Next("model");
					};
				p=pParent;
			}
			else if("cubemaps" == p->sName)
			{
				BREATHE::XML::cNode *pParent=p;
					p=p->FindChild("cubemap");
					while(p)
					{
						std::string sPath;
						if(p->GetAttribute("texture", &sPath))
							pLevel->LoadCubemap(sPath);

						//TODO: position="10.0, 10.0, 0.0"

						p=p->Next("cubemap");
					};
				p=pParent;
			}

			p=p->Next();
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

		uiTriangles+=pLevel->RenderStaticModel(pLevel->GetModel(sFilename + "mesh.3ds"), MATH::cVec3(0.0f, 0.0f, 0.0f));

		std::vector<BREATHE::cLevelModel*>::iterator iter = vModel.begin();
		std::vector<BREATHE::cLevelModel*>::iterator end = vModel.end();
		
		while(end != iter)
		{
			glPushMatrix();
				glMultMatrixf((*iter)->m);
				uiTriangles+=pLevel->RenderStaticModel((*iter)->pModel, MATH::cVec3(0.0f, 0.0f, 0.0f));
			glPopMatrix();

			iter++;
		}

		return uiTriangles;
	}
}
