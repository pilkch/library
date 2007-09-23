#include <cassert>
#include <cmath>

#include <sstream>

#include <list>
#include <map>
#include <vector>

// writing on a text file
#include <iostream>
#include <fstream>

// Breathe
#include <breathe/breathe.h>
#include <breathe/util/log.h>
#include <breathe/util/filesystem.h>

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

#include <breathe/loader_3ds/file.h>
#include <breathe/loader_3ds/chunk.h>
#include <breathe/loader_3ds/light3ds.h>
#include <breathe/loader_3ds/material3ds.h>
#include <breathe/loader_3ds/mesh3ds.h>
#include <breathe/loader_3ds/camera3ds.h>

//Uses an Octree to partition the model
//If all of the triangles can be contained within a small radius, 
//then they are all added to the root node and there are no children
namespace breathe
{
	namespace render
	{
		namespace model
		{
			float fScaleCamera=0.05f;
			int iVersionFile;
			int iVersionMesh;

			cStatic::cStatic()
				: cModel(), cOctree()
			{
				bFoundMeshes=false;

				uiTriangles=0;

				uiCurrentMesh=0;
				pCurrentMesh=NULL;

				fScale=0.5f;
			}

			cStatic::~cStatic()
			{

			}

			void cStatic::ParseEditor3D(LOADER_3DS::Model3DSChunk c, std::string sFilename)
			{
				std::ostringstream t;
				for(LOADER_3DS::Model3DSChunk cc = c.Child() ; cc ; cc = cc.Sibling())
				{
					switch(cc.ID())
					{
						case(0x3d3e):
							t.str("");
							iVersionMesh=cc.Int();
							t<<"Mesh Version " << iVersionMesh;
							LOG.Success("c3ds", t.str());
						break;

						case(0x100):
							t.str("");
							fScale=cc.Float();
							t<<"Master Scale "<<fScale;
							LOG.Success("c3ds", t.str());
							fScale*=0.5f;
						break;

						case(0xafff):
							ParseMaterial(cc);
						break;
						
						case(0x4000):
							ParseEditObject(cc, sFilename);
						break;
						
						default:
							t.str("");
							t<<"Unknown type1 0x" << std::hex << cc.ID();
							LOG.Error("c3ds", t.str());
						break;
					}
				}
			}

			void cStatic::ParseEditObject(LOADER_3DS::Model3DSChunk c, std::string sFilename)
			{
				std::ostringstream t;
				std::string obj_name = c.Str();
				
				for(LOADER_3DS::Model3DSChunk cc = c.Child() ; cc ; cc = cc.Sibling())
				{
					switch(cc.ID())
					{
						case(0x4100):
							ParseMesh(obj_name, cc, sFilename);
						break;
						
						case(0x4600):
							ParseLight(obj_name, cc);
						break;
						
						case(0x4700):
							ParseCamera(cc);
						break;
						
						default:
							t.str("");
							t<<"Unknown object 0x" << std::hex << cc.ID();
							LOG.Error("c3ds", t.str());
						break;
					}
				}
			}

			
			void cStatic::ParseLight(const std::string &name , LOADER_3DS::Model3DSChunk c)
			{
				LOG.Error("c3ds", "object light");
			}

			
			void cStatic::ParseMaterial(LOADER_3DS::Model3DSChunk c)
			{
				LOG.Success("3ds", "Edit material");
				
				for(LOADER_3DS::Model3DSChunk cc = c.Child() ; cc ; cc = cc.Sibling())
				{
					switch(cc.ID())
					{
						case(0xa000):
							NewMaterial(cc);
						break;
						
						default:
						break;
					}
				}
			}

			void cStatic::NewMaterial(LOADER_3DS::Model3DSChunk c)
			{
				std::string mat_name = c.Str();

				if(mat_name.find(".mat") != std::string::npos)
					LOG.Success("3ds", "Material: " + mat_name);
				else
					LOG.Error("3ds", "Invalid material: " + mat_name);

				vMaterial.push_back(mat_name);
			}


			void cStatic::ParseCamera(LOADER_3DS::Model3DSChunk c)
			{
				LOG.Success("3ds", "Camera");

				math::cFrustum *p=new math::cFrustum();

				p->eye.x=p->eyeIdeal.x= fScaleCamera * c.Float();
				p->eye.y=p->eyeIdeal.y= fScaleCamera * c.Float();
				p->eye.z=p->eyeIdeal.z= fScaleCamera * c.Float();

				p->target.x=p->targetIdeal.x= fScaleCamera * c.Float();
				p->target.y=p->targetIdeal.y= fScaleCamera * c.Float();
				p->target.z=p->targetIdeal.z= fScaleCamera * c.Float();
				
				float bank_angle = fScaleCamera * c.Float();
				float focus = fScaleCamera * c.Float();

				//vCamera.push_back(p);
			}

			void cStatic::ParseMesh(const std::string &sName , LOADER_3DS::Model3DSChunk c, std::string sFilename)
			{
				bFoundMeshes=true;

				LOG.Success("c3ds", "Mesh3DS::Parse(" + sName + ")");

				LOADER_3DS::Mesh3DSObject* pMesh = new LOADER_3DS::Mesh3DSObject(sName , c);

				if(pMesh)
				{
					vMesh.push_back(new cMesh());

					pCurrentMesh = vMesh.back();
          pCurrentMesh->CreateNewMesh();


					unsigned int face=0;
					unsigned int mesh=0;

					LOADER_3DS::Mesh3DSFace f;
					
					std::string sMaterial;
				

					std::vector<LOADER_3DS::Mesh3DSVertex> vVertex = pMesh->Vertices();
					std::vector<LOADER_3DS::Mesh3DSTextureCoord> vTextureCoord = pMesh->TextureCoords();
					std::vector<LOADER_3DS::Mesh3DSFace> vFaces = pMesh->Faces();
					
					pCurrentMesh->pMeshData->uiTriangles = vFaces.size();
					uiTriangles += pCurrentMesh->pMeshData->uiTriangles;

					//vMaterial[uiCurrentMesh];
					pCurrentMesh->sMaterial=breathe::filesystem::FindFile(breathe::filesystem::GetPath(sFilename) + pMesh->sMaterial);

					// This is a hack because for some reason the string gets corrupted, so we copy it back to itself, 
					// try it, comment these lines out, it breaks.  I don't know why :(
					breathe::unicode_char* c = (breathe::unicode_char*)(pCurrentMesh->sMaterial.c_str());
					pCurrentMesh->sMaterial = c;

					for(face=0;face<pCurrentMesh->pMeshData->uiTriangles;face++)
					{
						// 3ds files store faces as having 3 indexs in vertex arrays
						f = vFaces[face];

						pCurrentMesh->pMeshData->vVertex.push_back(fScale * vVertex[f.a].x);
						pCurrentMesh->pMeshData->vVertex.push_back(fScale * vVertex[f.a].y);
						pCurrentMesh->pMeshData->vVertex.push_back(fScale * vVertex[f.a].z);

						pCurrentMesh->pMeshData->vVertex.push_back(fScale * vVertex[f.b].x);
						pCurrentMesh->pMeshData->vVertex.push_back(fScale * vVertex[f.b].y);
						pCurrentMesh->pMeshData->vVertex.push_back(fScale * vVertex[f.b].z);
						
						pCurrentMesh->pMeshData->vVertex.push_back(fScale * vVertex[f.c].x);
						pCurrentMesh->pMeshData->vVertex.push_back(fScale * vVertex[f.c].y);
						pCurrentMesh->pMeshData->vVertex.push_back(fScale * vVertex[f.c].z);
						
						pCurrentMesh->pMeshData->vTextureCoord.push_back(vTextureCoord[f.a].u);
						pCurrentMesh->pMeshData->vTextureCoord.push_back(vTextureCoord[f.a].v);
						pCurrentMesh->pMeshData->vTextureCoord.push_back(vTextureCoord[f.b].u);
						pCurrentMesh->pMeshData->vTextureCoord.push_back(vTextureCoord[f.b].v);
						pCurrentMesh->pMeshData->vTextureCoord.push_back(vTextureCoord[f.c].u);
						pCurrentMesh->pMeshData->vTextureCoord.push_back(vTextureCoord[f.c].v);
					}

					uiCurrentMesh++;
				}
			}



			int cStatic::Load3DS(std::string sFilename)
			{
				iVersionFile=0;
				iVersionMesh=0;
				fScale=1.0f;

				LOG.Success("c3ds", "Loading " + sFilename);


				LOADER_3DS::Model3DSFile file(sFilename);

				LOADER_3DS::Model3DSChunk root = file.Child();

				std::ostringstream t;
				for(LOADER_3DS::Model3DSChunk cc = root.Child() ; cc ; cc = cc.Sibling())
				{
					switch(cc.ID())
					{
						case(0x0002):
							t.str("");
							iVersionFile=cc.Int();
							t<<"File Version " << iVersionFile;
							LOG.Success("c3ds", t.str());
						break;
						
						case(0x3d3d):
							ParseEditor3D(cc, sFilename);
						break;
						
						case(0xb000):
							t.str("");
							t<<"Keyframe Information";
							LOG.Success("c3ds", t.str());
						break;

						default:
							t.str("");
							t<<"Unknown type0 0x" << std::hex << cc.ID();
							LOG.Error("c3ds", t.str());
						break;
					}
				}




				//TODO: Generate normals
				//for i in each vertice
				//{
				//	normal = new vector(0, 0, 0)
				//	for j in each shared polygon
				//	{
				//		normal = normal + polygons(j).normal
				//	}
				//
				//	vertices(i).normal = normalize(normal) // no need to divide by the total number of polygons since normalizing it will void that anyway
				//}

				return uiTriangles;
			}

			int cStatic::Load(std::string sFilename)
			{
				if(sFilename.find(".3ds"))
					return Load3DS(sFilename);

				return 0;
			}

			cMesh* cStatic::GetMesh(unsigned int index)
			{
				assert(index < vMesh.size());

				return vMesh[index];
			}


			unsigned int cStatic::Render(math::cOctree *pNode)
			{
				// We should already have the octree created before we call this function.
				// This only goes through the nodes that are in our frustum, then renders those
				// vertices stored in their end nodes.  Before we draw a node we check to
				// make sure it is a subdivided node (from m_bSubdivided).  If it is, then
				// we haven't reaches the end and we need to keep recursing through the tree.
				// Once we get to a node that isn't subdivided we draw it's vertices.

				// Make sure a valid node was passed in and make sure we actually need to render.
				// We want to check if this node's cube is even in our frustum first.
				// To do that we pass in our center point of the node and 1/2 it's width to our 
				// CubeInFrustum() function.  This will return "true" if it is inside the frustum 
				// (camera's view), otherwise return false.  
				if(!pNode || frustum->CubeInFrustum(pNode->m_vCenter.x, pNode->m_vCenter.y, 
					pNode->m_vCenter.z, pNode->m_Width / 2)) 
						return 0;

				unsigned int uiTriangles = 0;

				// Check if this node is subdivided. If so, then we need to recurse and draw it's nodes
				if(pNode->IsSubDivided())
				{

					// Recurse to the bottom of these nodes and draw the end node's vertices
					// Like creating the octree, we need to recurse through each of the 8 nodes.
					uiTriangles += Render(pNode->m_pOctreeNodes[math::TOP_LEFT_FRONT]);
					uiTriangles += Render(pNode->m_pOctreeNodes[math::TOP_LEFT_BACK]);
					uiTriangles += Render(pNode->m_pOctreeNodes[math::TOP_RIGHT_BACK]);
					uiTriangles += Render(pNode->m_pOctreeNodes[math::TOP_RIGHT_FRONT]);
					uiTriangles += Render(pNode->m_pOctreeNodes[math::BOTTOM_LEFT_FRONT]);
					uiTriangles += Render(pNode->m_pOctreeNodes[math::BOTTOM_LEFT_BACK]);
					uiTriangles += Render(pNode->m_pOctreeNodes[math::BOTTOM_RIGHT_BACK]);
					uiTriangles += Render(pNode->m_pOctreeNodes[math::BOTTOM_RIGHT_FRONT]);
				}
				else
				{
					// Increase the amount of nodes in our viewing frustum (camera's view)
					g_TotalNodesDrawn++;

					// Make sure we have valid vertices assigned to this node
					if(!pNode->m_pVertices) return 0;

					// Render the world data with triangles
					//glBegin(GL_TRIANGLES);

					// Turn the polygons green
					//glColor3ub(0, 255, 0);

					// Store the vertices in a local pointer to keep code more clean
					//cVec3 *pVertices = pNode->m_pVertices;

					// Go through all of the vertices (the number of triangles * 3)
					//for(int i = 0; i < pNode->GetTriangleCount() * 3; i += 3)
					//{
						// Before we render the vertices we want to calculate the face normal
						// of the current polygon.  That way when lighting is turned on we can
						// see the definition of the terrain more clearly.  In reality you wouldn't do this.
						
						// Here we get a vector from each side of the triangle
						//cVec3 vVector1 = pVertices[i + 1] - pVertices[i];
						//cVec3 vVector2 = pVertices[i + 2] - pVertices[i];

						// Then we need to get the cross product of those 2 vectors (The normal's direction)
						//cVec3 vNormal = Cross(vVector1, vVector2);

						// Now we normalize the normal so it is a unit vector (length of 1)
						//vNormal = Normalize(vNormal);

						// Pass in the normal for this triangle so we can see better depth in the scene
						//glNormal3f(vNormal.x, vNormal.y, vNormal.z);

						// Render the first point in the triangle
						//glVertex3f(pVertices[i].x, pVertices[i].y, pVertices[i].z);

						// Render the next point in the triangle
						//glVertex3f(pVertices[i + 1].x, pVertices[i + 1].y, pVertices[i + 1].z);

						// Render the last point in the triangle to form the current triangle
						//glVertex3f(pVertices[i + 2].x, pVertices[i + 2].y, pVertices[i + 2].z);
					//}

					// Quit Drawing
					//glEnd();
				}

				return uiTriangles;
			}

			unsigned int cStatic::Render()
			{

		    
				return 0;
			}
			
			void cStatic::Update(float fCurrentTime)
			{

			}

			void cStatic::CloneTo(cStatic* rhs)
			{
				rhs->vMesh.clear();

				unsigned int i;
				unsigned int n = vMesh.size();
				for (i = 0; i < n; i++)
				{
					cMesh* pMesh = new cMesh();
					vMesh[i]->CloneTo(pMesh);
					rhs->vMesh.push_back(pMesh);
				}
			}
		}
	}
}
