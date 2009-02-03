#include <cassert>
#include <cmath>

#include <sstream>

#include <list>
#include <map>
#include <vector>

// Writing to and from a text file
#include <iostream>
#include <fstream>

// Boost includes
#include <boost/shared_ptr.hpp>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cSmartPtr.h>
#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/storage/filesystem.h>

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

#include <breathe/loader_3ds/file.h>
#include <breathe/loader_3ds/chunk.h>
#include <breathe/loader_3ds/light3ds.h>
#include <breathe/loader_3ds/material3ds.h>
#include <breathe/loader_3ds/mesh3ds.h>
#include <breathe/loader_3ds/camera3ds.h>

// Uses an Octree to partition the model
// If all of the triangles can be contained within a small radius,
// then they are all added to the root node and there are no children
namespace breathe
{
  namespace render
  {
    namespace model
    {
      float fScaleCamera=0.05f;
      int iVersionFile;
      int iVersionMesh;

      cStatic::cStatic() :
        cModel(),
        cOctree()
      {
        bFoundMeshes=false;

        uiTriangles=0;

        uiCurrentMesh=0;

        fScale=0.5f;
      }

      cStatic::~cStatic()
      {

      }

      void cStatic::ParseEditor3D(loader_3ds::Model3DSChunk c, const string_t& sFilename)
      {
        std::ostringstream t;
        for (loader_3ds::Model3DSChunk cc = c.Child(); cc.IsValid(); cc = cc.Sibling()) {
          switch(cc.ID()) {
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

      void cStatic::ParseEditObject(loader_3ds::Model3DSChunk c, const string_t& sFilename)
      {
        std::ostringstream t;
        string_t obj_name = c.Str();

        for (loader_3ds::Model3DSChunk cc = c.Child(); cc.IsValid(); cc = cc.Sibling()) {
          switch(cc.ID()) {
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


      void cStatic::ParseLight(const string_t& name , loader_3ds::Model3DSChunk c)
      {
        LOG.Error("c3ds", "object light");
      }


      void cStatic::ParseMaterial(loader_3ds::Model3DSChunk c)
      {
        LOG.Success("3ds", "Edit material");

        for (loader_3ds::Model3DSChunk cc = c.Child(); cc.IsValid(); cc = cc.Sibling()) {
          switch(cc.ID())  {
            case(0xa000):
              NewMaterial(cc);
            break;

            default:
            break;
          }
        }
      }

      void cStatic::NewMaterial(loader_3ds::Model3DSChunk c)
      {
        string_t mat_name(breathe::string::ToString_t(c.Str()));

        if (mat_name.find(TEXT(".mat")) != string_t::npos) LOG.Success("3ds", "Material: " + breathe::string::ToUTF8(mat_name));
        else LOG.Error("3ds", "Invalid material: " + breathe::string::ToUTF8(mat_name));

        vMaterial.push_back(mat_name);
      }


      void cStatic::ParseCamera(loader_3ds::Model3DSChunk c)
      {
        LOG.Success("3ds", "Camera");

        math::cFrustum *p=new math::cFrustum();

        p->eye.x=p->eyeIdeal.x= fScaleCamera * c.Float();
        p->eye.y=p->eyeIdeal.y= fScaleCamera * c.Float();
        p->eye.z=p->eyeIdeal.z= fScaleCamera * c.Float();

        p->target.x=p->targetIdeal.x= fScaleCamera * c.Float();
        p->target.y=p->targetIdeal.y= fScaleCamera * c.Float();
        p->target.z=p->targetIdeal.z= fScaleCamera * c.Float();

        //float bank_angle = fScaleCamera * c.Float();
        //float focus = fScaleCamera * c.Float();

        //vCamera.push_back(p);
      }

      void cStatic::ParseMesh(const string_t& sName , loader_3ds::Model3DSChunk c, const string_t& sFilename)
      {
        bFoundMeshes = true;

        LOG.Success("c3ds", "Mesh3DS::Parse(" + breathe::string::ToUTF8(sName) + ")");

        loader_3ds::Mesh3DSObject* pMesh = new loader_3ds::Mesh3DSObject(sName, c);
        if (pMesh == nullptr) return;

        vMesh.push_back(cMeshRef(new cMesh));

        pCurrentMesh = vMesh.back();
        pCurrentMesh->CreateNewMesh();

        std::vector<loader_3ds::Mesh3DSVertex> vVertex = pMesh->Vertices();
        std::vector<loader_3ds::Mesh3DSTextureCoord> vTextureCoord = pMesh->TextureCoords();
        std::vector<loader_3ds::Mesh3DSFace> vFaces = pMesh->Faces();

        pCurrentMesh->pMeshData->uiTriangles = (unsigned int)(vFaces.size());
        uiTriangles += pCurrentMesh->pMeshData->uiTriangles;

        //vMaterial[uiCurrentMesh];

        // This is a hack because for some reason the string gets corrupted, so we copy it back to itself,
        // try it, comment these lines out, it breaks.  I don't know why :(
        string_t sMeshMaterialName = breathe::string::ToString_t(pMesh->Material());

        breathe::filesystem::FindResourceFile(
          breathe::filesystem::GetPath(breathe::string::ToString_t(sFilename)) + sMeshMaterialName, pCurrentMesh->sMaterial
        );

        for (size_t face = 0; face < pCurrentMesh->pMeshData->uiTriangles; face++) {
          // 3ds files store faces as having 3 indexs in vertex arrays
          const loader_3ds::Mesh3DSFace& f = vFaces[face];

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

      int cStatic::Load3DS(const string_t& sFilename)
      {
        iVersionFile = 0;
        iVersionMesh = 0;
        fScale = 1.0f;

        LOG.Success("c3ds", "Loading " + breathe::string::ToUTF8(sFilename));


        loader_3ds::Model3DSFile file(sFilename);

        loader_3ds::Model3DSChunk root = file.Child();

        std::ostringstream t;
        for (loader_3ds::Model3DSChunk cc = root.Child(); cc.IsValid(); cc = cc.Sibling()) {
          switch(cc.ID()) {
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
        //  normal = new vector(0, 0, 0)
        //  for j in each shared polygon
        //  {
        //    normal = normal + polygons(j).normal
        //  }
        //
        //  vertices(i).normal = Normalise(normal) // no need to divide by the total number of polygons since normalising it will void that anyway
        //}

        return uiTriangles;
      }

      int cStatic::Load(const string_t& sFilename)
      {
        if (sFilename.find(TEXT(".3ds"))) return Load3DS(sFilename);

        return 0;
      }

      cMeshRef cStatic::GetMesh(unsigned int index)
      {
        assert(index < vMesh.size());

        return vMesh[index];
      }


        size_t cStatic::Render(math::cOctree* pNode)
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
        if (!pNode || frustum->CubeInFrustum(pNode->m_vCenter.x, pNode->m_vCenter.y,
          pNode->m_vCenter.z, pNode->m_Width / 2))
            return 0;

        size_t uiTriangles = 0;

        // Check if this node is subdivided. If so, then we need to recurse and draw it's nodes
        if (pNode->IsSubDivided())
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
          if (!pNode->m_pVertices) return 0;

          // Render the world data with triangles
          //glBegin(GL_TRIANGLES);

          // Turn the polygons green
          //glColor3ub(0, 255, 0);

          // Store the vertices in a local pointer to keep code more clean
          //cVec3* pVertices = pNode->m_pVertices;

          // Go through all of the vertices (the number of triangles * 3)
          //for (int i = 0; i < pNode->GetTriangleCount() * 3; i += 3)
          //{
            // Before we render the vertices we want to calculate the face normal
            // of the current polygon.  That way when lighting is turned on we can
            // see the definition of the terrain more clearly.  In reality you wouldn't do this.

            // Here we get a vector from each side of the triangle
            //cVec3 vVector1 = pVertices[i + 1] - pVertices[i];
            //cVec3 vVector2 = pVertices[i + 2] - pVertices[i];

            // Then we need to get the cross product of those 2 vectors (The normal's direction)
            //cVec3 vNormal = Cross(vVector1, vVector2);

            // Now we normalise the normal so it is a unit vector (length of 1)
            //vNormal = normalise(vNormal);

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

      size_t cStatic::Render()
      {


        return 0;
      }

      void cStatic::Update(sampletime_t currentTime)
      {

      }

      void cStatic::CopyFrom(const cStaticRef rhs)
      {
        vMesh.clear();

        size_t i;
        const size_t n = rhs->vMesh.size();
        for (i = 0; i < n; i++) {
          cMeshRef pMesh(new cMesh);
          pMesh->CopyFrom(rhs->vMesh[i]);
          vMesh.push_back(pMesh);
        }
      }
    }
  }
}
