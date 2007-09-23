#ifndef LIGHT3DSOBJECT_H
#define LIGHT3DSOBJECT_H

namespace BREATHE
{
	namespace LOADER_3DS
	{
		class Light3DSObject
		{
		public:
			Light3DSObject(const std::string &nname);
			~Light3DSObject();
		private:

			std::string name;
		};
	}
}

#endif