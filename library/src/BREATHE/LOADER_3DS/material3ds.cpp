
// writing on a text file
#include <iostream>
#include <fstream>

#include <string>

// Breathe
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>

#include <BREATHE/LOADER_3DS/material3ds.h>
#include <BREATHE/LOADER_3DS/build3ds.h>


namespace BREATHE
{
	namespace LOADER_3DS
	{
		Material3DS::Material3DS()
		{

		}
		Material3DS::~Material3DS()
		{

		}

		void Material3DS::Parse(Model3DSChunk c)
		{
#ifdef DEBUG3DS
			pLog->Success("3ds", "Edit material");
#endif
			
			for(Model3DSChunk cc = c.Child() ; cc ; cc = cc.Sibling())
			{
				switch(cc.ID())
				{
					case(0xa000):
						NewMaterial(cc);
					break;
					
					default:
					break;
				}
			}
		}

		void Material3DS::NewMaterial(Model3DSChunk c)
		{
			std::string mat_name = c.Str();

#ifdef DEBUG3DS
			if(mat_name.find(".mat") != string::npos)
				pLog->Success("3ds", "Material: %s", mat_name.c_str());
			else
				pLog->Error("3ds", "Invalid material: %s", mat_name.c_str());
#endif

			materials.push_back(mat_name);
		}
	}
}