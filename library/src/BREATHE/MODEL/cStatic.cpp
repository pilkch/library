#include <cmath>

#include <sstream>
#include <map>
#include <vector>

#include <BREATHE/cMem.h>
#include <BREATHE/cLog.h>
#include <BREATHE/cFileSystem.h>

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

#include <BREATHE/RENDER/cCamera.h>

#include <BREATHE/cBase.h>
#include <BREATHE/MODEL/cMesh.h>
#include <BREATHE/MODEL/cModel.h>
#include <BREATHE/MODEL/cStatic.h>

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
	namespace MODEL
	{
		float fScaleCamera=0.05f;
		int iVersionFile;
		int iVersionMesh;

		cStatic::cStatic()
			: cModel()
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
						pLog->Success("c3ds", t.str());
					break;

					case(0x100):
						t.str("");
						fScale=cc.Float();
						t<<"Master Scale "<<fScale;
						pLog->Success("c3ds", t.str());
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
						pLog->Error("c3ds", t.str());
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
						pLog->Error("c3ds", t.str());
					break;
				}
			}
		}

		
		void cStatic::ParseLight(const std::string &name , LOADER_3DS::Model3DSChunk c)
		{
			pLog->Error("c3ds", "object light");
		}

		
		void cStatic::ParseMaterial(LOADER_3DS::Model3DSChunk c)
		{
			pLog->Success("3ds", "Edit material");
			
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
				pLog->Success("3ds", "Material: %s", mat_name.c_str());
			else
				pLog->Error("3ds", "Invalid material: %s", mat_name.c_str());

			vMaterial.push_back(mat_name);
		}


		void cStatic::ParseCamera(LOADER_3DS::Model3DSChunk c)
		{
			pLog->Success("3ds", "Camera");

			RENDER::cCamera *p=new RENDER::cCamera();

			p->eye.x=p->eyeIdeal.x= fScaleCamera * c.Float();
			p->eye.y=p->eyeIdeal.y= fScaleCamera * c.Float();
			p->eye.z=p->eyeIdeal.z= fScaleCamera * c.Float();

			p->target.x=p->targetIdeal.x= fScaleCamera * c.Float();
			p->target.y=p->targetIdeal.y= fScaleCamera * c.Float();
			p->target.z=p->targetIdeal.z= fScaleCamera * c.Float();
			
			float bank_angle = fScaleCamera * c.Float();
			float focus = fScaleCamera * c.Float();

			vCamera.push_back(p);
		}

		void cStatic::ParseMesh(const std::string &sName , LOADER_3DS::Model3DSChunk c, std::string sFilename)
		{
			bFoundMeshes=true;

			std::ostringstream t;
			t<<"Mesh3DS::Parse(" << sName.c_str() << ")";
			pLog->Success("c3ds", t.str());

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
				pCurrentMesh->sMaterial=pFileSystem->FindFile(pFileSystem->GetPath(sFilename) + m->sMaterial);

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


			std::ostringstream t;

			t<<"Loading "<<sFilename;
			pLog->Success("c3ds", (char*)t.str().c_str());


			LOADER_3DS::Model3DSFile file(sFilename);

			LOADER_3DS::Model3DSChunk root = file.Child();

			for(LOADER_3DS::Model3DSChunk cc = root.Child() ; cc ; cc = cc.Sibling())
			{
				switch(cc.ID())
				{
					case(0x0002):
						t.str("");
						iVersionFile=cc.Int();
						t<<"File Version " << iVersionFile;
						pLog->Success("c3ds", t.str());
					break;
					
					case(0x3d3d):
						ParseEditor3D(cc, sFilename);
					break;
					
					case(0xb000):
						t.str("");
						t<<"Keyframe Information";
						pLog->Success("c3ds", t.str());
					break;

					default:
						t.str("");
						t<<"Unknown type0 0x" << std::hex << cc.ID();
						pLog->Error("c3ds", t.str());
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

		int cStatic::Render()
		{
	    
			return 0;
		}
		
		void cStatic::Update(float fTime)
		{

		}
	}
}
