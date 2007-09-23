#ifndef CAMERA3DS_H
#define CAMERA3DS_H

#include <vector>

#include "chunk.h"

namespace breathe
{
	namespace RENDER
	{
		class cCamera;
	}

	namespace LOADER_3DS
	{
		class Camera3DS
		{
		public:
			std::vector<render::cCamera * > vCamera;

			Camera3DS();
			~Camera3DS();

			void Parse(Model3DSChunk c);
		};
	}
}

#endif //CAMERA3DS_H
