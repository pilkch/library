#include <cmath>

#include <sstream>

#include <list>
#include <map>
#include <vector>

// writing on a text file
#include <iostream>
#include <fstream>

// Breathe
#include <BREATHE/cBreathe.h>
#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/UTIL/cFileSystem.h>

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

#include <BREATHE/LOADER_3DS/file.h>
#include <BREATHE/LOADER_3DS/chunk.h>
#include <BREATHE/LOADER_3DS/light3ds.h>
#include <BREATHE/LOADER_3DS/material3ds.h>
#include <BREATHE/LOADER_3DS/mesh3ds.h>
#include <BREATHE/LOADER_3DS/camera3ds.h>

//Uses an Octree to partition the model
//If all of the triangles can be contained within a small radius, 
//then they are all added to the root node and there are no children
namespace BREATHE
{
	namespace RENDER
	{
		namespace MODEL
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

				MATH::cFrustum *p=new MATH::cFrustum();

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

				LOADER_3DS::Mesh3DSObject *m=new LOADER_3DS::Mesh3DSObject(sName , c);

				if(m)
				{
					vMesh.push_back(new cMesh());

					pCurrentMesh=vMesh.back();

					



					unsigned int face=0;
					unsigned int mesh=0;

					LOADER_3DS::Mesh3DSFace f;		    
					
					std::string sMaterial;
				

					std::vector<LOADER_3DS::Mesh3DSVertex> vVertex=m->Vertices();
					std::vector<LOADER_3DS::Mesh3DSTextureCoord> vTextureCoord=m->TextureCoords();
					std::vector<LOADER_3DS::Mesh3DSFace> vFaces=m->Faces();
					
					pCurrentMesh->uiTriangles=vFaces.size();
					uiTriangles+=pCurrentMesh->uiTriangles;

					//vMaterial[uiCurrentMesh];
					pCurrentMesh->sMaterial=BREATHE::FILESYSTEM::FindFile(BREATHE::FILESYSTEM::GetPath(sFilename) + m->sMaterial);

					char *c=const_cast<char *>(pCurrentMesh->sMaterial.c_str());
					pCurrentMesh->sMaterial=c;

					for(face=0;face<pCurrentMesh->uiTriangles;face++)
					{
						// 3ds files store faces as having 3 indexs in vertex arrays
						f = vFaces[face];

						pCurrentMesh->vVertex.push_back(fScale * vVertex[f.a].x);
						pCurrentMesh->vVertex.push_back(fScale * vVertex[f.a].y);
						pCurrentMesh->vVertex.push_back(fScale * vVertex[f.a].z);

						pCurrentMesh->vVertex.push_back(fScale * vVertex[f.b].x);
						pCurrentMesh->vVertex.push_back(fScale * vVertex[f.b].y);
						pCurrentMesh->vVertex.push_back(fScale * vVertex[f.b].z);
						
						pCurrentMesh->vVertex.push_back(fScale * vVertex[f.c].x);
						pCurrentMesh->vVertex.push_back(fScale * vVertex[f.c].y);
						pCurrentMesh->vVertex.push_back(fScale * vVertex[f.c].z);
						
						pCurrentMesh->vTextureCoord.push_back(vTextureCoord[f.a].u);
						pCurrentMesh->vTextureCoord.push_back(vTextureCoord[f.a].v);
						pCurrentMesh->vTextureCoord.push_back(vTextureCoord[f.b].u);
						pCurrentMesh->vTextureCoord.push_back(vTextureCoord[f.b].v);
						pCurrentMesh->vTextureCoord.push_back(vTextureCoord[f.c].u);
						pCurrentMesh->vTextureCoord.push_back(vTextureCoord[f.c].v);
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


			unsigned int cStatic::Render(MATH::cOctree *pNode)
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
					uiTriangles += Render(pNode->m_pOctreeNodes[MATH::TOP_LEFT_FRONT]);
					uiTriangles += Render(pNode->m_pOctreeNodes[MATH::TOP_LEFT_BACK]);
					uiTriangles += Render(pNode->m_pOctreeNodes[MATH::TOP_RIGHT_BACK]);
					uiTriangles += Render(pNode->m_pOctreeNodes[MATH::TOP_RIGHT_FRONT]);
					uiTriangles += Render(pNode->m_pOctreeNodes[MATH::BOTTOM_LEFT_FRONT]);
					uiTriangles += Render(pNode->m_pOctreeNodes[MATH::BOTTOM_LEFT_BACK]);
					uiTriangles += Render(pNode->m_pOctreeNodes[MATH::BOTTOM_RIGHT_BACK]);
					uiTriangles += Render(pNode->m_pOctreeNodes[MATH::BOTTOM_RIGHT_FRONT]);
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
		}
	}
}
