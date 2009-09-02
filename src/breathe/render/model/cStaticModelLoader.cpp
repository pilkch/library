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

namespace breathe
{
  namespace render
  {
    namespace model
    {
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

        mesh.Clear();


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

        // Read vertices
        for (; i < n; i++) {
          spitfire::string::Split(lines[i], ' ', tokens);
          if (tokens.empty()) continue;

          // Skip anything we can't parse
          if (SkipJunk(tokens[0])) continue;

          if (tokens[0] != "v") {
            LOG<<"lines["<<i<<"] (\""<<lines[i]<<"\") != \"v\""<<std::endl;
            break;
          }

          if (tokens.size() != 4) {
            LOG<<"unexpected number of arguments to \"v\" at lines["<<i<<"] (\""<<lines[i]<<"\")"<<std::endl;
          }

          if (tokens.size() == 4) {
            vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[1])));
            vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[2])));
            vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[3])));
          }
        }

        // Read texture coordinates
        for (; i < n; i++) {
          spitfire::string::Split(lines[i], ' ', tokens);
          if (tokens.empty()) continue;

          // Skip anything we can't parse
          if (SkipJunk(tokens[0])) continue;

          if (tokens[0] != "vt") {
            LOG<<"lines["<<i<<"] (\""<<lines[i]<<"\") != \"vt\""<<std::endl;
            break;
          }

          if (tokens.size() != 3) {
            LOG<<"unexpected number of arguments to \"vt\" at lines["<<i<<"] (\""<<lines[i]<<"\")"<<std::endl;
          }

          if (tokens.size() == 3) {
            textureCoordinates.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[1])));
            textureCoordinates.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[2])));
          }
        }

        // Read normals
        for (; i < n; i++) {
          LOG<<"lines["<<i<<"] = \""<<lines[i]<<"\""<<std::endl;
          spitfire::string::Split(lines[i], ' ', tokens);
          if (tokens.empty()) continue;

          // Skip anything we can't parse
          if (SkipJunk(tokens[0])) continue;

          if (tokens[0] != "vn") {
            LOG<<"lines["<<i<<"] (\""<<lines[i]<<"\") != \"vn\""<<std::endl;
            break;
          }

          if (tokens.size() != 4) {
            LOG<<"unexpected number of arguments to \"vn\" at lines["<<i<<"] (\""<<lines[i]<<"\")"<<std::endl;
          }

          if (tokens.size() == 4) {
            normals.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[1])));
            normals.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[2])));
            normals.push_back(spitfire::string::ToFloat(spitfire::string::ToString_t(tokens[3])));
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
                size_t value = spitfire::string::ToUnsignedInt(spitfire::string::ToString_t(elements[item]));
                if (value == 0) value = 1;
                textureCoordinatesIndices.push_back(value);
                item++;
              }

              if (l >= 3) {
                size_t value = spitfire::string::ToUnsignedInt(spitfire::string::ToString_t(elements[item]));
                if (value == 0) value = 1;
                normalsIndices.push_back(value);
                item++;
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

      // s off
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



      // *** cStaticModelMesh

      void cStaticModelMesh::Clear()
      {
        vertices.clear();
        normals.clear();
        indices.clear();

        sMaterial.clear();
      }

      void cStaticModel::Clear()
      {
        const size_t n = mesh.size();
        for (size_t i = 0; i < n; i++) SAFE_DELETE(mesh[i]);

        mesh.clear();
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
    }
  }
}
