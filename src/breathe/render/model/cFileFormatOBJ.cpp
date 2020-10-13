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

#ifdef __WIN__
// OpenGL headers
#include <GL/GLee.h>
#endif

// Spitfire headers
#include <spitfire/spitfire.h>

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

#include <breathe/render/model/cFileFormatOBJ.h>

namespace breathe
{
  namespace render
  {
    namespace model
    {
      // *** cFileFormatOBJ

      bool cFileFormatOBJ::SkipJunk(const std::string& sFirstToken) const
      {
        return (sFirstToken == "#") || (sFirstToken == "mtllib");
      }

      size_t cFileFormatOBJ::LoadMesh(const std::vector<std::string> lines, size_t i, const size_t n, std::vector<float>& vertices, std::vector<float>& textureCoordinates, std::vector<float>& normals, cStaticModelMesh& mesh) const
      {
        LOG("i=", i, " n=", n);

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
            LOG("lines[", i, "] (\"", lines[i], "\") != \"o\"");
            break;
          }

          //if (tokens.size() == 4) {
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[1])));
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[2])));
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[3])));
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
              LOGERROR("unexpected number of arguments to \"v\" at lines[", i, "] (\"", lines[i], "\")");
            }

            if (tokens.size() == 4) {
              vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[1])));
              vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[2])));
              vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[3])));
            }
          } else if (sType == "vt") {
            // Texture Coordinate
            if (tokens.size() != 3) {
              LOGERROR("unexpected number of arguments to \"vt\" at lines[", i, "] (\"", lines[i], "\")");
            }

            if (tokens.size() == 3) {
              // The y coordinate always seems to be flipped, ie. 1..0 instead of 0..1, so we cater for that by flipping it again
              textureCoordinates.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[1])));
              textureCoordinates.push_back(1.0f - spitfire::string::ToFloat(spitfire::string::ToString(tokens[2])));
            }
          } else if (sType == "vn") {
            // Normal
            if (tokens.size() != 4) {
              LOGERROR("unexpected number of arguments to \"vn\" at lines[", i, "] (\"", lines[i], "\")");
            }

            if (tokens.size() == 4) {
              normals.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[1])));
              normals.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[2])));
              normals.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[3])));
            }
          } else {
            // Unknown
            LOG("lines[", i, "] (\"", lines[i], "\") != \"v\" or \"vt\" or \"vn\"");
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
            LOG("lines[", i, "] (\"", lines[i], "\") != \"usemtl\"");
            break;
          }


          if (tokens.size() != 2) {
            LOG("unexpected number of arguments to \"usemtl\" at lines[", i, "] (\"", lines[i], "\")");
          }

          if (tokens.size() == 2) {
            mesh.sMaterial = spitfire::string::ToString(tokens[1]);
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
            LOG("lines[", i, "] (\"", lines[i], "\") != \"s\"");
            break;
          }

          //if (tokens.size() != 4) {
          //  LOG("unexpected number of arguments to \"s\" at lines[", i, "] (\"", lines[i], "\")");
          //}

          //if (tokens.size() == 4) {
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[1])));
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[2])));
          //  vertices.push_back(spitfire::string::ToFloat(spitfire::string::ToString(tokens[3])));
          //}
        }

        // Read faces into indices
        for (; i < n; i++) {
          spitfire::string::Split(lines[i], ' ', tokens);
          if (tokens.empty()) continue;

          // Skip anything we can't parse
          if (SkipJunk(tokens[0])) continue;

          if (tokens[0] != "f") {
            LOG("lines[", i, "] (\"", lines[i], "\") != \"f\"");
            break;
          }

          if (tokens.size() == 3) {
            LOG("This is a line (2 vertices) not a face, we don't support lines for \"f\", skipping lines[", i, "] (\"", lines[i], "\")");
            continue;
          }

          if (tokens.size() != 4) {
            LOG("unexpected number of arguments to \"f\" at lines[", i, "] (\"", lines[i], "\")");
            ASSERT(false);
          }

          if (tokens.size() == 4) {
            // Triangle
            const size_t k = 3;
            for (size_t j = 0; j < k; j++) {
              std::vector<std::string> elements;
              spitfire::string::Split(tokens[j + 1], '/', elements);
              if (elements.empty()) {
                LOG("Pushing back \"", tokens[j + 1], "\"");
                elements.push_back(tokens[j + 1]);
              }

              const size_t l = elements.size();
              ASSERT(l >= 1);

              size_t item = 0;
              size_t value = spitfire::string::ToUnsignedInt(spitfire::string::ToString(elements[item]));
              if (value == 0) value = 1;
              verticesIndices.push_back(value);
              item++;

              if (l >= 2) {
                if (!elements[item].empty()) {
                  size_t value = spitfire::string::ToUnsignedInt(spitfire::string::ToString(elements[item]));
                  if (value == 0) value = 1;
                  textureCoordinatesIndices.push_back(value);
                  item++;
                }
              }

              if (l >= 3) {
                if (!elements[item].empty()) {
                  size_t value = spitfire::string::ToUnsignedInt(spitfire::string::ToString(elements[item]));
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
        #ifdef BUILD_DEBUG
        const size_t textureCoordinatesSize = textureCoordinates.size();
        const size_t normalsSize = normals.size();
        #endif

        const size_t verticesIndicesSize = verticesIndices.size();
        const size_t textureCoordinatesIndicesSize = textureCoordinatesIndices.size();
        const size_t normalsIndicesSize = normalsIndices.size();
        LOG("Before Vertices=", verticesSize, ", TextureCoordinates=", textureCoordinatesSize, ", Normals=", normalsSize, " VerticesIndices=", verticesIndicesSize, ", TextureCoordinatesIndices=", textureCoordinatesIndicesSize, ", NormalsIndices=", normalsIndicesSize);

        // Convert indices to vertices
        if (verticesIndicesSize != 0) {
          std::vector<float_t> tempVertices;

          for (size_t j = 0; j < verticesIndicesSize; j++) {
            size_t index = verticesIndices[j];
            if (index != 0) index--;

            for (size_t offset = 3 * index; offset < (3 * index) + 3; offset++) {
              //LOG("offset=", offset);
              if (offset >= verticesSize) {
                LOG("index=", index, " offset=", offset, " verticesSize=", verticesSize);
                ASSERT(offset < verticesSize);
              }
              ASSERT(offset < verticesSize);
              //LOG("vertices[", offset, "]=", vertices[offset]);
              tempVertices.push_back(vertices[offset]);
            }
          }

          mesh.vertices = tempVertices;
        }


        // Convert indices to texture coordinates
        if (textureCoordinatesIndicesSize != 0) {
          std::vector<float_t> tempTextureCoordinates;

          for (size_t j = 0; j < textureCoordinatesIndicesSize; j++) {
            size_t index = textureCoordinatesIndices[j];
            if (index != 0) index--;

            for (size_t offset = 2 * index; offset < (2 * index) + 2; offset++) {
              //LOG("offset=", offset);
              ASSERT(offset < textureCoordinatesSize);
              //LOG("textureCoordinates[", offset, "]=", textureCoordinates[offset]);
              tempTextureCoordinates.push_back(textureCoordinates[offset]);
            }
          }

          mesh.textureCoordinates = tempTextureCoordinates;
        }

        // We always want to have enough texture coordinates so we invent more if needed
        // This is just a hack so that we never have to disable texturing, but the model should
        // really include enough texture coordinates instead
        spitfire::math::cRand rand;
        const size_t nVertices = mesh.vertices.size() / 3;
        for (size_t iTextureCoordinate = mesh.textureCoordinates.size() / 2; iTextureCoordinate < nVertices; iTextureCoordinate++) {
          mesh.textureCoordinates.push_back(rand.randomZeroToOnef());
          mesh.textureCoordinates.push_back(rand.randomZeroToOnef());
        }


        // Convert indices to normals
        if (normalsIndicesSize != 0) {
          std::vector<float_t> tempNormals;

          for (size_t j = 0; j < normalsIndicesSize; j++) {
            size_t index = normalsIndices[j];
            if (index != 0) index--;

            for (size_t offset = 3 * index; offset < (3 * index) + 3; offset++) {
              //LOG("offset=", offset);
              ASSERT(offset < normalsSize);
              //LOG("normals[", offset, "]=", normals[offset]);
              tempNormals.push_back(normals[offset]);
            }
          }

          mesh.normals = tempNormals;
        }

        #ifdef BUILD_DEBUG
        const size_t a = mesh.vertices.size();
        const size_t b = mesh.textureCoordinates.size();
        const size_t c = mesh.normals.size();
        LOG("After Vertices=", a, ", TextureCoordinates=", b, ", Normals=", c);
        #endif

        LOG("cFileFormatOBJ::LoadMesh returning");

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

      bool cFileFormatOBJ::Load(const string_t& sFilePath, cStaticModel& model) const
      {
        LOG("cFileFormatOBJ::Load \"", string::ToUTF8(sFilePath), "\"");

        model.Clear();

        std::vector<std::string> lines;
        spitfire::storage::ReadText(sFilePath, lines);

        if (lines.empty()) return false;

        string_t sDefaultMaterial = spitfire::filesystem::GetFileNoExtension(sFilePath);
        LOG("cFileFormatOBJ::Load sDefaultMaterial=\"", sDefaultMaterial, "\"");

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
            const string_t sObjFileDirectory = spitfire::filesystem::GetFolder(sFilePath);
            spitfire::filesystem::cPath p(sObjFileDirectory, sMaterialFile);

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

        #ifdef BUILD_DEBUG
        const size_t nMeshes = model.mesh.size();
        LOG("Meshes=", nMeshes);
        #endif

        return true;
      }
    }
  }
}
