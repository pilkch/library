#ifndef LIGHT3DS_H
#define LIGHT3DS_H

#include <string>
#include <vector>

#include "chunk.h"

#include "light3dsobject.h"

namespace BREATHE
{
	namespace LOADER_3DS
	{
		class Light3DS
		{
		public:
			Light3DS();
			~Light3DS();

			void Parse(const std::string &name , Model3DSChunk c);

		private:
			std::vector<Light3DSObject> lights;
		};
	}
}

#endif