#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdarg.h>
#include <vector>
#include <map>






#include <BREATHE/UTIL/cLog.h>
#include <BREATHE/cBreathe.h>


#include <cmath>
#include <BREATHE/MATH/cMath.h>
#include <BREATHE/MATH/cVec2.h>
#include <BREATHE/MATH/cVec3.h>
#include <BREATHE/MATH/cVec4.h>
#include <BREATHE/MATH/cMat4.h>
#include <BREATHE/MATH/cPlane.h>
#include <BREATHE/MATH/cQuaternion.h>
#include <BREATHE/MATH/cFrustum.h>
#include <BREATHE/MATH/cOctree.h>
#include <BREATHE/MATH/cColour.h>

#include <BREATHE/RENDER/cCamera.h>


namespace BREATHE
{
	namespace RENDER
	{
		cCamera::cCamera()
		{
			target.x=0.0f;
			target.y=0.0f;
			target.z=0.0f;

			eye.x=10.0f;
			eye.y=0.0f;
			eye.z=2.0f;

			up.x=0.0f;
			up.y=0.0f;
			up.z=1.0f;


			targetIdeal.x=0.0f;
			targetIdeal.y=0.0f;
			targetIdeal.z=0.0f;

			eyeIdeal.x=10.0f;
			eyeIdeal.y=0.0f;
			eyeIdeal.z=2.0f;

			upIdeal.x=0.0f;
			upIdeal.y=0.0f;
			upIdeal.z=1.0f;
		}

		cCamera::~cCamera()
		{

		}

		void cCamera::Update()
		{
			//float f=fabs(targetIdeal.GetLength()-targetIdeal.GetLength());
			//if(f>1.0f)
			//	f=0.01f;
			target=target.lerp(targetIdeal, 0.05f);
			eye=eye.lerp(eyeIdeal, 0.05f);
			up=up.lerp(upIdeal, 0.05f);

			m.LookAt(eye, target, up);
		}
	}
}