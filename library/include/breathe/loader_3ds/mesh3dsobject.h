#ifndef MESH3DSOBJECT_H
#define MESH3DSOBJECT_H

#include <map>
#include <string>
#include <vector>

#include "chunk.h"

namespace breathe
{
	namespace loader_3ds
	{
		struct Mesh3DSVertex
		{
			float x , y , z;
		};
		struct Mesh3DSTextureCoord
		{
			float u , v;
		};
		struct Mesh3DSFace
		{
			int a , b , c;
		};
		struct Mesh3DSMatrix
		{
			float m[4][4];
		};

		class Mesh3DSObject
		{
		public:
			Mesh3DSObject(const std::string &nname , Model3DSChunk c);
			Mesh3DSObject(const Mesh3DSObject &mesh);
			~Mesh3DSObject();
			
			void operator=(const Mesh3DSObject &mesh);

			const std::string &Name() const;
			const std::string& Material() const;
			Mesh3DSMatrix Matrix() const;	// adds last row (0,0,0,1)

			const std::vector<Mesh3DSVertex> &Vertices() const;
			const std::vector<Mesh3DSTextureCoord> &TextureCoords() const;
			const std::vector<Mesh3DSFace> &Faces() const;
			const std::map<std::string , std::vector<int> > &Materials() const;

			bool bFoundVertices, bFoundFaces, bTextureCoords, bFoundMaterials;
			
		private:
			
			void ParseLocalCoordinateSystem(Model3DSChunk c);
			void ParseVertices(Model3DSChunk c);
			void ParseTextureCoords(Model3DSChunk c);
			void ParseFaces(Model3DSChunk c);

			void ParseFacesMaterials(Model3DSChunk c);

			std::string name;
			std::string sMaterial;
			Mesh3DSMatrix matrix;

			std::vector<Mesh3DSVertex> vertices;
			std::vector<Mesh3DSTextureCoord> texturecoords;
			std::vector<Mesh3DSFace> faces;
			std::map<std::string , std::vector<int> > material_faces;
		};
	}
}

#endif
