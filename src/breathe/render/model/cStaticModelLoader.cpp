#include <cstdio>
#include <cstring>
#include <cmath>
#include <cassert>

#include <list>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

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

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/file.h>
#include <spitfire/storage/filesystem.h>

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

#include <breathe/render/cTexture.h>
#include <breathe/render/cTextureAtlas.h>
#include <breathe/render/cRender.h>
#include <breathe/render/cVertexBufferObject.h>

#include <breathe/render/model/cStaticModelLoader.h>






















/*#include <cassert>
#include <cmath>

#include <sstream>

#include <list>
#include <map>
#include <vector>

// Writing to and from a text file
#include <iostream>
#include <fstream>

// Boost headers
#include <boost/shared_ptr.hpp>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cSmartPtr.h>
#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

#include <spitfire/storage/filesystem.h>

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

#include <breathe/loader_3ds/file.h>
#include <breathe/loader_3ds/chunk.h>
#include <breathe/loader_3ds/light3ds.h>
#include <breathe/loader_3ds/material3ds.h>
#include <breathe/loader_3ds/mesh3ds.h>
#include <breathe/loader_3ds/camera3ds.h>




#include <spitfire/math/cOctree.h>

#include <breathe/render/model/cMesh.h>
#include <breathe/render/model/cModel.h>

namespace breathe
{
  namespace loader_3ds
  {
    class Model3DSChunk;
    class Light3DSObject;
    class Mesh3DSObject;
  }
}*/

namespace breathe
{
  namespace render
  {
    namespace model
    {
      /*float fScaleCamera=0.05f;
      int iVersionFile;
      int iVersionMesh;


      class c3DSData : public cModel, protected math::cOctree
      {
      public:
        c3DSData();

        bool bFoundMeshes;
        bool bFoundVertices;
        bool bFoundTextureCoords;
        bool bFoundMaterials;

        cMeshRef pCurrentMesh;


        std::vector<string_t> vMaterial;

        unsigned int uiCurrentMesh;

        float fScale;
      };

      c3DSData::c3DSData() :
        cModel(),
        cOctree()
      {
        bFoundMeshes=false;

        uiTriangles=0;

        uiCurrentMesh=0;

        fScale=0.5f;
      }


      class cFileFormat3DS
      {
      public:
        bool Load(const string_t& sFilename, cStaticModel& model) const;
        bool Save(const string_t& sFilename, const cStaticModel& model) const;

      private:
        int Load3DS(const string_t& sFilename);

        void ParseEditor3D(loader_3ds::Model3DSChunk c, const string_t& sFilename);
        void ParseEditObject(loader_3ds::Model3DSChunk c, const string_t& sFilename);

        void ParseMesh(const string_t& name, loader_3ds::Model3DSChunk c, const string_t& sFilename);
        void ParseMaterial(loader_3ds::Model3DSChunk c);

        void ParseCamera(loader_3ds::Model3DSChunk c);
        void ParseLight(const string_t& name, loader_3ds::Model3DSChunk c);

        void NewMaterial(loader_3ds::Model3DSChunk c);
      };






      void cFileFormat3DS::ParseEditor3D(loader_3ds::Model3DSChunk c, const string_t& sFilename)
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
              fScale = cc.Float();
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

      void cFileFormat3DS::ParseEditObject(loader_3ds::Model3DSChunk c, const string_t& sFilename)
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


      void cFileFormat3DS::ParseLight(const string_t& name , loader_3ds::Model3DSChunk c)
      {
        LOG.Error("c3ds", "object light");
      }


      void cFileFormat3DS::ParseMaterial(loader_3ds::Model3DSChunk c)
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

      void cFileFormat3DS::NewMaterial(loader_3ds::Model3DSChunk c)
      {
        string_t mat_name(breathe::string::ToString_t(c.Str()));

        if (mat_name.find(TEXT(".mat")) != string_t::npos) LOG.Success("3ds", "Material: " + breathe::string::ToUTF8(mat_name));
        else LOG.Error("3ds", "Invalid material: " + breathe::string::ToUTF8(mat_name));

        vMaterial.push_back(mat_name);
      }


      void cFileFormat3DS::ParseCamera(loader_3ds::Model3DSChunk c)
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

      void cFileFormat3DS::ParseMesh(const string_t& sName , loader_3ds::Model3DSChunk c, const string_t& sFilename)
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

        if (!breathe::filesystem::FindFile(
          breathe::filesystem::GetPath(breathe::string::ToString_t(sFilename)) + sMeshMaterialName, pCurrentMesh->sMaterial
        )) {
          if (!breathe::filesystem::FindFile(sMeshMaterialName, pCurrentMesh->sMaterial)) {
            breathe::filesystem::FindFile(TEXT("materials/") + sMeshMaterialName, pCurrentMesh->sMaterial);
          }
        }

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

      int cFileFormat3DS::Load3DS(const string_t& sFilename)
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

      int cFileFormat3DS::Load(const string_t& sFilename)
      {
        if (sFilename.find(TEXT(".3ds"))) return Load3DS(sFilename);

        return 0;
      }

      cMeshRef cFileFormat3DS::GetMesh(unsigned int index)
      {
        assert(index < vMesh.size());

        return vMesh[index];
      }


        size_t cFileFormat3DS::Render(math::cOctree* pNode)
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

      size_t cFileFormat3DS::Render()
      {


        return 0;
      }

      void cFileFormat3DS::Update(sampletime_t currentTime)
      {

      }

      void cFileFormat3DS::CopyFrom(const cFileFormat3DSRef rhs)
      {
        vMesh.clear();

        size_t i;
        const size_t n = rhs->vMesh.size();
        for (i = 0; i < n; i++) {
          cMeshRef pMesh(new cMesh);
          pMesh->CopyFrom(rhs->vMesh[i]);
          vMesh.push_back(pMesh);
        }
      }*/


      class cFileFormat3DS
      {
      public:
        bool Load(const string_t& sFilename, cStaticModel& model) const;
        bool Save(const string_t& sFilename, const cStaticModel& model) const;
      };


      class cFileFormatASE
      {
      public:
        bool Load(const string_t& sFilename, cStaticModel& model) const;
        bool Save(const string_t& sFilename, const cStaticModel& model) const;
      };

      class cFileFormatLWO
      {
      public:
        bool Load(const string_t& sFilename, cStaticModel& model) const;
        bool Save(const string_t& sFilename, const cStaticModel& model) const;
      };

      class cFileFormatOBJ
      {
      public:
        bool Load(const string_t& sFilename, cStaticModel& model) const;
        bool Save(const string_t& sFilename, const cStaticModel& model) const;

      private:
        bool SkipJunk(const std::string& sFirstToken) const;

        bool LoadMaterialList(const string_t& sFilename, std::map<string_t, string_t>& materials) const;
        size_t LoadMesh(const std::vector<std::string> lines, size_t i, const size_t n, std::vector<float>& vertices, std::vector<float>& textureCoordinates, std::vector<float>& normals, cStaticModelMesh& mesh) const;
      };



      // *** cFileFormat3DS

      bool cFileFormat3DS::Load(const string_t& sFilename, cStaticModel& model) const
      {
        model.Clear();

        return false;
      }

      // *** cFileFormatASE

      bool cFileFormatASE::Load(const string_t& sFilename, cStaticModel& model) const
      {
        model.Clear();

        return false;
      }

      // *** cFileFormatLWO

      bool cFileFormatLWO::Load(const string_t& sFilename, cStaticModel& model) const
      {
        model.Clear();

        return false;
      }

      // *** cFileFormatOBJ

      bool cFileFormatOBJ::SkipJunk(const std::string& sFirstToken) const
      {
        return (sFirstToken == "#") || (sFirstToken == "mtllib");
      }

      size_t cFileFormatOBJ::LoadMesh(const std::vector<std::string> lines, size_t i, const size_t n, std::vector<float>& vertices, std::vector<float>& textureCoordinates, std::vector<float>& normals, cStaticModelMesh& mesh) const
      {
        LOG<<"cFileFormatOBJ::LoadMesh i="<<i<<" n="<<n<<std::endl;

        std::vector<float> verticesIndices;
        std::vector<float> textureCoordinatesIndices;
        std::vector<float> normalsIndices;

        std::vector<std::string> tokens;

        // Read o
        for (; i < n; i++) {
          spitfire::string::Split(lines[i], ' ', tokens);
          if (tokens.empty()) continue;

          // Skip anything we can't parse
          if (SkipJunk(tokens[0])) continue;

          if (tokens[0] != "o") {
            LOG<<"lines["<<i<<"] (\""<<lines[i]<<"\") != \"o\""<<std::endl;
            break;
          }

          //if (tokens.size() == 4) {
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[1])));
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[2])));
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[3])));
          //}
        }

        // Read vertices, textureCoordinates and normals
        for (; i < n; i++) {
          spitfire::string::Split(lines[i], ' ', tokens);
          if (tokens.empty()) continue;

          // Skip anything we can't parse
          if (SkipJunk(tokens[0])) continue;

          const std::string sType = tokens[0];
          if (sType == "v") {
            // Vertex
            if (tokens.size() != 4) {
              LOG<<"unexpected number of arguments to \"v\" at lines["<<i<<"] (\""<<lines[i]<<"\")"<<std::endl;
            }

            if (tokens.size() == 4) {
              vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[1])));
              vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[2])));
              vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[3])));
            }
          } else if (sType == "vt") {
            // Texture Coordinate
            if (tokens.size() != 3) {
              LOG<<"unexpected number of arguments to \"vt\" at lines["<<i<<"] (\""<<lines[i]<<"\")"<<std::endl;
            }

            if (tokens.size() == 3) {
              // The y coordinate always seems to be flipped, ie. 1..0 instead of 0..1, so we cater for that by flipping it again
              textureCoordinates.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[1])));
              textureCoordinates.push_back(1.0f - spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[2])));
            }
          } else if (sType == "vn") {
            // Normal
            if (tokens.size() != 4) {
              LOG<<"unexpected number of arguments to \"vn\" at lines["<<i<<"] (\""<<lines[i]<<"\")"<<std::endl;
            }

            if (tokens.size() == 4) {
              normals.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[1])));
              normals.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[2])));
              normals.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[3])));
            }
          } else {
            // Unknown
            LOG<<"lines["<<i<<"] (\""<<lines[i]<<"\") != \"v\" or \"vt\" or \"vn\""<<std::endl;
            break;
          }
        }

        // Read usemtl
        // TODO: Actually read and use the material
        for (; i < n; i++) {
          spitfire::string::Split(lines[i], ' ', tokens);
          if (tokens.empty()) continue;

          // Skip anything we can't parse
          if (SkipJunk(tokens[0])) continue;

          if (tokens[0] != "usemtl") {
            LOG<<"lines["<<i<<"] (\""<<lines[i]<<"\") != \"usemtl\""<<std::endl;
            break;
          }


          if (tokens.size() != 2) {
            LOG<<"unexpected number of arguments to \"usemtl\" at lines["<<i<<"] (\""<<lines[i]<<"\")"<<std::endl;
          }

          if (tokens.size() == 2) {
            mesh.sMaterial = spitfire::string::ToString_t(tokens[1]);
          }
        }

        // Read "s off"
        // TODO: What is "s off"?
        for (; i < n; i++) {
          spitfire::string::Split(lines[i], ' ', tokens);
          if (tokens.empty()) continue;

          // Skip anything we can't parse
          if (SkipJunk(tokens[0])) continue;

          if (tokens[0] != "s") {
            LOG<<"lines["<<i<<"] (\""<<lines[i]<<"\") != \"s\""<<std::endl;
            break;
          }

          //if (tokens.size() != 4) {
          //  LOG<<"unexpected number of arguments to \"s\" at lines["<<i<<"] (\""<<lines[i]<<"\")"<<std::endl;
          //}

          //if (tokens.size() == 4) {
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[1])));
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[2])));
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[3])));
          //}
        }

        // Read faces into indices
        for (; i < n; i++) {
          spitfire::string::Split(lines[i], ' ', tokens);
          if (tokens.empty()) continue;

          // Skip anything we can't parse
          if (SkipJunk(tokens[0])) continue;

          if (tokens[0] != "f") {
            LOG<<"lines["<<i<<"] (\""<<lines[i]<<"\") != \"f\""<<std::endl;
            break;
          }

          if (tokens.size() == 3) {
            LOG<<"This is a line (2 vertices) not a face, we don't support lines for \"f\", skipping lines["<<i<<"] (\""<<lines[i]<<"\")"<<std::endl;
            continue;
          }

          if (tokens.size() != 4) {
            LOG<<"unexpected number of arguments to \"f\" at lines["<<i<<"] (\""<<lines[i]<<"\")"<<std::endl;
            ASSERT(false);
          }

          if (tokens.size() == 4) {
            // Triangle
            const size_t k = 3;
            for (size_t j = 0; j < k; j++) {
              std::vector<std::string> elements;
              spitfire::string::Split(tokens[j + 1], '/', elements);
              if (elements.empty()) {
                LOG<<"Pushing back \""<<tokens[j + 1]<<"\""<<std::endl;
                elements.push_back(tokens[j + 1]);
              }

              const size_t l = elements.size();
              ASSERT(l >= 1);

              size_t item = 0;
              size_t value = spitfire::string::ToUnsignedInt(spitfire::string::ToString_t(elements[item]));
              if (value == 0) value = 1;
              verticesIndices.push_back(value);
              item++;

              if (l >= 2) {
                if (!elements[item].empty()) {
                  size_t value = spitfire::string::ToUnsignedInt(spitfire::string::ToString_t(elements[item]));
                  if (value == 0) value = 1;
                  textureCoordinatesIndices.push_back(value);
                  item++;
                }
              }

              if (l >= 3) {
                if (!elements[item].empty()) {
                  size_t value = spitfire::string::ToUnsignedInt(spitfire::string::ToString_t(elements[item]));
                  if (value == 0) value = 1;
                  normalsIndices.push_back(value);
                  item++;
                }
              }
            }
          }
        }




        // Now convert from indices into vertices, texture coordinates and normals into actual vertices, texture coordinates and normals
        const size_t verticesSize = vertices.size();
        const size_t textureCoordinatesSize = textureCoordinates.size();
        const size_t normalsSize = normals.size();

        const size_t verticesIndicesSize = verticesIndices.size();
        const size_t textureCoordinatesIndicesSize = textureCoordinatesIndices.size();
        const size_t normalsIndicesSize = normalsIndices.size();
        LOG<<"Before Vertices="<<verticesSize<<", TextureCoordinates="<<textureCoordinatesSize<<", Normals="<<normalsSize<<" VerticesIndices="<<verticesIndicesSize<<", TextureCoordinatesIndices="<<textureCoordinatesIndicesSize<<", NormalsIndices="<<normalsIndicesSize<<std::endl;

        // Convert indices to vertices
        if (verticesIndicesSize != 0) {
          std::vector<float_t> tempVertices;

          for (size_t i = 0; i < verticesIndicesSize; i++) {
            size_t index = verticesIndices[i];
            if (index != 0) index--;

            for (size_t offset = 3 * index; offset < (3 * index) + 3; offset++) {
              //LOG<<"offset="<<offset<<std::endl;
              if (offset >= verticesSize) {
                LOG<<"index="<<index<<" offset="<<offset<<" verticesSize="<<verticesSize<<std::endl;
                ASSERT(offset < verticesSize);
              }
              ASSERT(offset < verticesSize);
              //LOG<<"vertices["<<offset<<"]="<<vertices[offset]<<std::endl;
              tempVertices.push_back(vertices[offset]);
            }
          }

          mesh.vertices = tempVertices;
        }


        // Convert indices to texture coordinates
        if (textureCoordinatesIndicesSize != 0) {
          std::vector<float_t> tempTextureCoordinates;

          for (size_t i = 0; i < textureCoordinatesIndicesSize; i++) {
            size_t index = textureCoordinatesIndices[i];
            if (index != 0) index--;

            for (size_t offset = 2 * index; offset < (2 * index) + 2; offset++) {
              //LOG<<"offset="<<offset<<std::endl;
              ASSERT(offset < textureCoordinatesSize);
              //LOG<<"textureCoordinates["<<offset<<"]="<<textureCoordinates[offset]<<std::endl;
              tempTextureCoordinates.push_back(textureCoordinates[offset]);
            }
          }

          mesh.textureCoordinates = tempTextureCoordinates;
        }

        // We always want to have enough texture coordinates so we invent more if needed
        // This is just a hack so that we never have to disable texturing, but the model should
        // really include enough texture coordinates instead
        const size_t nVertices = mesh.vertices.size() / 3;
        for (size_t iTextureCoordinate = mesh.textureCoordinates.size() / 2; iTextureCoordinate < nVertices; iTextureCoordinate++) {
          mesh.textureCoordinates.push_back(math::randomZeroToOnef());
          mesh.textureCoordinates.push_back(math::randomZeroToOnef());
        }


        // Convert indices to normals
        if (normalsIndicesSize != 0) {
          std::vector<float_t> tempNormals;

          for (size_t i = 0; i < normalsIndicesSize; i++) {
            size_t index = normalsIndices[i];
            if (index != 0) index--;

            for (size_t offset = 3 * index; offset < (3 * index) + 3; offset++) {
              //LOG<<"offset="<<offset<<std::endl;
              ASSERT(offset < normalsSize);
              //LOG<<"normals["<<offset<<"]="<<normals[offset]<<std::endl;
              tempNormals.push_back(normals[offset]);
            }
          }

          mesh.normals = tempNormals;
        }

        {
          const size_t a = mesh.vertices.size();
          const size_t b = mesh.textureCoordinates.size();
          const size_t c = mesh.normals.size();
          LOG<<"After Vertices="<<a<<", TextureCoordinates="<<b<<", Normals="<<c<<std::endl;
        }

        LOG<<"cFileFormatOBJ::LoadMesh returning"<<std::endl;

        return i;
      }

      // # http://www.royriggs.com/obj.html
      //
      // # Blender3D v248 OBJ File: rubbish_bin.blend
      // # www.blender3d.org
      // mtllib rubbish_bin.mtl
      // v 0.353553 0.000000 -0.353553

      // vt 0.000000 0.000000

      // vn 0.000000 -1.000000 0.000000

      // usemtl (null)
      // OR
      // usemtl Material.001_rubbish_bin.png
      // OR
      // usemtl rubbish_bin

      // s off
      // OR
      // s 1
      // OR
      // s 2 etc.

      // f 1 2 3
      // OR
      // f 65//1 1//1 2//1
      // OR
      // f 65/1/1 1/2/1 2/3/1

      // f v1[/vt1][/vn1] v2[/vt2][/vn2] v3[/vt3][/vn3] ...

      bool cFileFormatOBJ::Load(const string_t& sFilename, cStaticModel& model) const
      {
        LOG<<"cFileFormatOBJ::Load \""<<string::ToUTF8(sFilename)<<"\""<<std::endl;

        model.Clear();

        std::vector<std::string> lines;
        spitfire::storage::ReadText(sFilename, lines);

        if (lines.empty()) return false;

        string_t sDefaultMaterial = spitfire::filesystem::GetFileNoExtension(sFilename);
        LOG<<"cFileFormatOBJ::Load sDefaultMaterial=\""<<sDefaultMaterial<<"\""<<std::endl;

        std::vector<float> vertices;
        std::vector<float> textureCoordinates;
        std::vector<float> normals;

        const size_t n = lines.size();
        for (size_t i = 0; i < n; i++) {
          std::vector<std::string> tokens;
          spitfire::string::Split(lines[i], ' ', tokens);
          if (tokens.empty()) continue;

          // Skip anything we can't parse
          if (SkipJunk(tokens[0])) continue;


          // Ok, we might have a valid mesh
          cStaticModelMesh* pMesh = new cStaticModelMesh;

          // Default our material to the default mesh material
          pMesh->sMaterial = sDefaultMaterial;

          i = LoadMesh(lines, i, n, vertices, textureCoordinates, normals, *pMesh);

          if (pMesh->vertices.empty()) SAFE_DELETE(pMesh);
          else {
            // Resolve material name to a full file path
            // Something like "material"
            const string_t sMaterialFile = pMesh->sMaterial;
            const string_t sObjFileDirectory = spitfire::filesystem::GetPath(sFilename);
            spitfire::filesystem::path p(sObjFileDirectory, sMaterialFile);

            // Something like "shared/data/models/material"
            const string_t sMaterialFileFullPath = p.GetFullPath();

            // Something like "shared/data/models/material.mat"
            pMesh->sMaterial = sMaterialFileFullPath + TEXT(".mat");

            // Add the mesh to the mesh list
            model.mesh.push_back(pMesh);
          }

          // This is a dodgy hack to avoid incrementing i, which would accidentally skip the first "v" of the next model
          if (i != 0) i--;
        }

        const size_t nMeshes = model.mesh.size();
        LOG<<"Meshes="<<nMeshes<<std::endl;

        return true;
      }




      // *** cStaticModelLoader

      bool cStaticModelLoader::Load(const string_t& sFilename, cStaticModel& model) const
      {
        const string_t sExtension(spitfire::filesystem::GetExtension(sFilename));
        /*if (sExtension == TEXT("3ds")) {
            cFileFormat3DS loader;
            return loader.Load(sFilename, model);
        } else if (sExtension == TEXT("ase")) {
            cFileFormatASE loader;
            return loader.Load(sFilename, model);
        } else if (sExtension == TEXT("lwo")) {
            cFileFormatLWO loader;
            return loader.Load(sFilename, model);
        } else*/ if (sExtension == TEXT("obj")) {
            cFileFormatOBJ loader;
            return loader.Load(sFilename, model);
        }

        // The format is not supported
        LOG<<"cStaticModelLoader::Load Format is not supported \""<<sFilename<<"\""<<std::endl;
        ASSERT(false);
        return false;
      }



      void cStaticModelSceneNodeFactory::LoadFromFile(const spitfire::string_t& sFilename, std::vector<cStaticModelSceneNodeFactoryItem>& meshes) const
      {
        meshes.clear();


        breathe::render::model::cStaticModel model;

        breathe::render::model::cStaticModelLoader loader;
        loader.Load(sFilename, model);

        const size_t nMeshes = model.mesh.size();
        for (size_t iMesh = 0; iMesh < nMeshes; iMesh++) {
          cStaticModelSceneNodeFactoryItem item;

          item.pVBO.reset(new breathe::render::cVertexBufferObject);

          item.pVBO->SetVertices(model.mesh[iMesh]->vertices);
          item.pVBO->SetTextureCoordinates(model.mesh[iMesh]->textureCoordinates);

          item.pVBO->Compile();

          item.pMaterial = pRender->AddMaterial(model.mesh[iMesh]->sMaterial);

          meshes.push_back(item);
        }
      }

      void cStaticModelSceneNodeFactory::CreateSceneNodeAttachedTo(std::vector<cStaticModelSceneNodeFactoryItem>& meshes, breathe::scenegraph3d::cGroupNodeRef pGroupNode) const
      {
        const size_t n = meshes.size();
        for (size_t i = 0; i < n; i++) {
          breathe::scenegraph3d::cModelNodeRef pNode(new breathe::scenegraph3d::cModelNode);

          breathe::scenegraph3d::cStateSet& stateset = pNode->GetStateSet();
          stateset.SetStateFromMaterial(meshes[i].pMaterial);

          breathe::scenegraph_common::cStateVertexBufferObject& vertexBufferObject = stateset.GetVertexBufferObject();
          vertexBufferObject.SetVertexBufferObject(meshes[i].pVBO);
          vertexBufferObject.SetEnabled(true);
          vertexBufferObject.SetHasValidValue(true);

          pGroupNode->AttachChild(pNode);
        }
      }
    }
  }
}
