// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <sstream>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include "breathe/loader_3ds/mesh3dsobject.h"
#include "breathe/loader_3ds/build3ds.h"



namespace breathe
{
	namespace loader_3ds
	{
		const float fScale=0.1f;

		Mesh3DSObject::Mesh3DSObject(const std::string &nname , Model3DSChunk c)
		: name(nname)
		{
			bFoundFaces=bFoundVertices=bTextureCoords=bFoundMaterials=false;

			for (Model3DSChunk cc = c.Child() ; cc ; cc = cc.Sibling())
			{
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

		void Mesh3DSObject::operator=(const Mesh3DSObject &mesh)
		{
			name = mesh.name;
			//float matrix[4][4];

			vertices = mesh.vertices;
			texturecoords = mesh.texturecoords;
			faces = mesh.faces;
			material_faces = mesh.material_faces;
			sMaterial = mesh.sMaterial;
		}

		void Mesh3DSObject::ParseLocalCoordinateSystem(Model3DSChunk c)
		{
			// bottom row should be (0 , 0 , 0 , 1)

			// populate matrix
			for (int i = 0 ; i < 4 ; i++)
			{
				for (int j = 0 ; j < 3 ; j++)
				{
					matrix.m[i][j] = c.Float();
				}
			}

			matrix.m[0][3] = 0;
			matrix.m[1][3] = 0;
			matrix.m[2][3] = 0;
			matrix.m[3][3] = 1;
		}

		void Mesh3DSObject::ParseVertices(Model3DSChunk c)
		{
			bFoundVertices=true;

			int n_vertices = c.Short();

			Mesh3DSVertex v;
			v.x=0;
			v.y=0;
			v.z=0;

			std::ostringstream t;
			t<<"n_vertices = " << n_vertices;
			LOG.Success("c3ds", t.str());

			for (int i = 0 ; i < n_vertices ; i++)
			{
				v.x = fScale*c.Float();
				v.y = fScale*c.Float();
				v.z = fScale*c.Float();
				
				vertices.push_back(v);
			}
		}
		void Mesh3DSObject::ParseTextureCoords(Model3DSChunk c)
		{
			bTextureCoords=true;

			int n_texcoords = c.Short();
			
			Mesh3DSTextureCoord texcoord;
			texcoord.u=0;
			texcoord.v=0;

			std::ostringstream t;
			t<<"n_texcoords = " << n_texcoords;
			LOG.Success("c3ds", t.str());

			for (int i = 0 ; i < n_texcoords ; i++)
			{
				texcoord.u = c.Float();
				texcoord.v = c.Float();
				
				texturecoords.push_back(texcoord);
			}
		}
		void Mesh3DSObject::ParseFaces(Model3DSChunk c)
		{
			bFoundFaces=true;
			
			unsigned int i=0;

			int n_faces = c.Short();

			Mesh3DSFace face;
			face.a=0;
			face.b=0;
			face.c=0;

			std::ostringstream t;
			t<<"n_faces = " << n_faces;
			LOG.Success("c3ds", t.str());

			for (i = 0 ; i < (unsigned int)(n_faces); i++)
			{
				face.a = c.Short();
				face.b = c.Short();
				face.c = c.Short();
				c.Short();	// read the crappy flag

				faces.push_back(face);
			}

			//unsigned long a=c.Position();
			//c.Goto(46366);

			//c.Finish();

			for (Model3DSChunk cc = c.Child() ; cc ; cc = cc.Sibling())
			{
				switch(cc.ID())
				{
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
			bFoundMaterials=true;

			sMaterial = c.Str();
			std::vector<int> faces_applied;

			LOG.Success("c3ds", "ParseFacesMaterials: " + sMaterial);

			int n_faces = c.Short();
			int f = 0;
			int i = 0;

			for (i = 0 ; i < n_faces ; i++)
			{
				f = c.Short();
				
				faces_applied.push_back(f);
			}

			material_faces[sMaterial] = faces_applied;
		}

		const std::string &Mesh3DSObject::Name()
		{
			return name;
		}
		Mesh3DSMatrix Mesh3DSObject::Matrix()
		{
			return matrix;
		}

		const std::vector<Mesh3DSVertex> &Mesh3DSObject::Vertices()
		{
			return vertices;
		}
		const std::vector<Mesh3DSTextureCoord> &Mesh3DSObject::TextureCoords()
		{
			return texturecoords;
		}
		const std::vector<Mesh3DSFace> &Mesh3DSObject::Faces()
		{
			return faces;
		}
		const std::map<std::string , std::vector<int> > &Mesh3DSObject::Materials()
		{
			return material_faces;
		}
	}
}