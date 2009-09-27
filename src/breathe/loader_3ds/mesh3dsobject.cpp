#include <cstring>

#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <sstream>

// Spitfire headers
#include <spitfire/spitfire.h>

#include <spitfire/util/cString.h>
#include <spitfire/util/log.h>

// Breathe headers
#include <breathe/breathe.h>

#include <breathe/loader_3ds/mesh3dsobject.h>
#include <breathe/loader_3ds/build3ds.h>

namespace breathe
{
  namespace loader_3ds
  {
    const float fScale = 0.1f;

    Mesh3DSObject::Mesh3DSObject(const string_t& nname , Model3DSChunk c) :
      name(nname)
    {
      bFoundFaces=bFoundVertices=bTextureCoords=bFoundMaterials=false;

      for (Model3DSChunk cc = c.Child(); cc.IsValid(); cc = cc.Sibling()) {
        switch(cc.ID())
        {
          case(0x4160):
            ParseLocalCoordinateSystem(cc);
          break;

          case(0x4110):
            ParseVertices(cc);
          break;

          case(0x4140):
            ParseTextureCoords(cc);
          break;

          case(0x4120):
            ParseFaces(cc);
          break;

          default:
            std::ostringstream t;
            t<<"error : unknown chunk " << std::hex << cc.ID();
            LOG.Error("c3ds", t.str());
          break;
        }
      }
    }

    Mesh3DSObject::Mesh3DSObject(const Mesh3DSObject &mesh)
    {
      operator=(mesh);
    }

    Mesh3DSObject::~Mesh3DSObject()
    {
    }

    void Mesh3DSObject::operator=(const Mesh3DSObject& mesh)
    {
      name = mesh.name;
      matrix = mesh.matrix;
      vertices = mesh.vertices;
      texturecoords = mesh.texturecoords;
      faces = mesh.faces;
      material_faces = mesh.material_faces;
      sMaterial = mesh.sMaterial;
    }

    void Mesh3DSObject::ParseLocalCoordinateSystem(Model3DSChunk c)
    {
      // Populate matrix
      for (size_t i = 0 ; i < 4 ; i++) {
        for (size_t j = 0 ; j < 3 ; j++) {
          matrix.m[i][j] = c.Float();
        }
      }

      // Bottom row should always be (0 , 0 , 0 , 1) as in the identity matrix
      matrix.m[0][3] = 0;
      matrix.m[1][3] = 0;
      matrix.m[2][3] = 0;
      matrix.m[3][3] = 1;
    }

    void Mesh3DSObject::ParseVertices(Model3DSChunk c)
    {
      bFoundVertices = true;

      const size_t n_vertices = c.Short();

      LOG<<"c3ds n_vertices = "<<n_vertices<<std::endl;

      Mesh3DSVertex v;
      for (size_t i = 0 ; i < n_vertices ; i++) {
        v.x = fScale * c.Float();
        v.y = fScale * c.Float();
        v.z = fScale * c.Float();

        vertices.push_back(v);
      }
    }
    void Mesh3DSObject::ParseTextureCoords(Model3DSChunk c)
    {
      bTextureCoords = true;

      size_t n_texcoords = c.Short();

      Mesh3DSTextureCoord texcoord;
      texcoord.u = 0;
      texcoord.v = 0;

      std::ostringstream t;
      t<<"n_texcoords = " << n_texcoords;
      LOG.Success("c3ds", t.str());

      for (size_t i = 0 ; i < n_texcoords ; i++) {
        texcoord.u = c.Float();
        texcoord.v = c.Float();

        texturecoords.push_back(texcoord);
      }
    }
    void Mesh3DSObject::ParseFaces(Model3DSChunk c)
    {
      bFoundFaces = true;

      size_t n_faces = c.Short();

      Mesh3DSFace face;
      face.a = 0;
      face.b = 0;
      face.c = 0;

      std::ostringstream t;
      t<<"n_faces = " << n_faces;
      LOG.Success("c3ds", t.str());

      for (size_t i = 0 ; i < n_faces; i++)
      {
        face.a = c.Short();
        face.b = c.Short();
        face.c = c.Short();
        c.Short();  // read the crappy flag

        faces.push_back(face);
      }

      //unsigned long a=c.Position();
      //c.Goto(46366);

      //c.Finish();

      for (Model3DSChunk cc = c.Child(); cc.IsValid(); cc = cc.Sibling()) {
        switch(cc.ID()) {
          case(0x4130):
            ParseFacesMaterials(cc);
          break;

          default:
            t.str("");
            t<<"Unknown id: 0x" << std::hex << cc.ID();
            LOG.Error("c3ds", t.str());
          break;
        }
      }
    }

    void Mesh3DSObject::ParseFacesMaterials(Model3DSChunk c)
    {
      sMaterial = c.Str();

      LOG.Success("c3ds", "ParseFacesMaterials: " + breathe::string::ToUTF8(sMaterial));

      bFoundMaterials = true;

      const size_t n_faces = c.Short();

      int f = 0;

      std::vector<int> faces_applied;
      for (size_t i = 0 ; i < n_faces ; i++) {
        f = c.Short();

        faces_applied.push_back(f);
      }

      material_faces[sMaterial] = faces_applied;
    }

    const string_t& Mesh3DSObject::Name() const
    {
      return name;
    }

    const string_t& Mesh3DSObject::Material() const
    {
      return sMaterial;
    }

    Mesh3DSMatrix Mesh3DSObject::Matrix() const
    {
      return matrix;
    }

    const std::vector<Mesh3DSVertex>& Mesh3DSObject::Vertices() const
    {
      return vertices;
    }

    const std::vector<Mesh3DSTextureCoord>& Mesh3DSObject::TextureCoords() const
    {
      return texturecoords;
    }

    const std::vector<Mesh3DSFace>& Mesh3DSObject::Faces() const
    {
      return faces;
    }

    const std::map<string_t, std::vector<int> >& Mesh3DSObject::Materials() const
    {
      return material_faces;
    }
  }
}
