// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <string>

// Breathe
#include <BREATHE/cBreathe.h>

#include <BREATHE/UTIL/cLog.h>

#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cFrustum.h>

#include "BREATHE/LOADER_3DS/build3ds.h"




#include <BREATHE/LOADER_3DS/camera3ds.h>

namespace BREATHE
{
	namespace LOADER_3DS
	{
		const float fScale=0.1f;

		Camera3DS::Camera3DS()
		{
					
		}

		Camera3DS::~Camera3DS()
		{

		}

		void Camera3DS::Parse(Model3DSChunk c)
		{
			LOG.Success("3ds", "Camera");

			MATH::cFrustum *p=new MATH::cFrustum();

			p->eye.x=p->eyeIdeal.x= fScale * c.Float();
			p->eye.y=p->eyeIdeal.y= fScale * c.Float();
			p->eye.z=p->eyeIdeal.z= fScale * c.Float();

			p->target.x=p->targetIdeal.x= fScale * c.Float();
			p->target.y=p->targetIdeal.y= fScale * c.Float();
			p->target.z=p->targetIdeal.z= fScale * c.Float();
			
			float bank_angle= fScale * c.Float();
			float focus= fScale * c.Float();

			//vCamera.push_back(p);
		}
	}
}