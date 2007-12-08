// writing on a text file
#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <string>

// Breathe
#include <breathe/breathe.h>

#include <breathe/util/cString.h>
#include <breathe/util/log.h>

#include <breathe/math/cVec3.h>
#include <breathe/math/cMat4.h>
#include <breathe/math/cFrustum.h>

#include "breathe/loader_3ds/build3ds.h"




#include <breathe/loader_3ds/camera3ds.h>

namespace breathe
{
	namespace loader_3ds
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

			math::cFrustum *p=new math::cFrustum();

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
